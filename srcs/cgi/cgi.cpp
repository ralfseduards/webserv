#include "../../includes/webserv.hpp"

#ifndef CGI_DIR
# define CGI_DIR "cgi-dir"
#endif

#define ENVP_SIZE 10
#define MAX_WAIT 2000000      // 2 seconds

// default cgi directory: cgi-dir
// simple script : ../cgi-dir/simple.py

// TODO: make it configurable in the parser

//  >>>>>>>>>>>>>>>> the cgi logic: <<<<<<<<<<<<<<<<
// 1. construct the environment
//    1. copy the real one
//    2. add the http header parsed as environ variables
//    3. add the required ones by the standard
// 2. pipe, fork, execve
//    1. write into stdin the message body of the http request
//    2. stdout is the response from the script
// 3. wait (maybe some no-hang/timeout logic)
// 4. parse the stdout of the script and make a http response


// TODO: POST requests
// TODO: GET requests


/* --===== the post request =====-- */

// package the contents of the request in a format:
// fieldname=value&fieldname1=value, like a QUERY_STRING



// >>>>>>>>>>>>>>>> ENV to implement <<<<<<<<<<<<<<<<<
// PATH                     -- envp path                      -- envp
// DOCUMENT_ROOT            -- the root of the server         -- config file
// REQUEST_METHOD           -- from http 'GET/POST line'      -- http header
// SCRIPT_NAME              -- from http 'GET/POST line'      -- http header
// QUERY_STRING             -- from http 'GET/POST line'      -- http header
// CONTENT_TYPE             -- from http header var           -- http header
// CONTENT_LENGTH           -- from http header var           -- http header
// HTTP_USER_AGENT          -- from http header var           -- http header
// HTTP_HOST                -- from http header var           -- http header
// HTTP_REFFERER            -- from http header var           -- http header

static int cgi_error(const char *str)
{
  perror(str);
  return (1);     // TODO: return error
}

/* like strdup() in C but using strings and new */
static char* cpp_strdup(std::string str)
{
  char *newStr = new char[str.size() + 1];
  int i = 0;

  for (std::string::iterator it = str.begin(); it <= str.end(); ++it)
    newStr[i++] = *it;
  newStr[str.size()] = '\0';
  return (newStr);
}

/* Splits the request string on \n and returns a vector with the lines.
 * Returned vector doesnt contain newlines. Uses a string stream.
*/
static std::vector<std::string> tokenize_request(const std::string& request)
{
  std::stringstream         ss(request);
  std::string               token;
  std::vector<std::string>  result;

  while (std::getline(ss, token, '\n'))
    result.push_back(token);
  return (result);
}

/* function returns PATH=... with whatever value is set in envp.
 * (returns "PATH=" if there is no path set)
*/
static std::string find_path(const char **envp)
{
  std::string check_str = "PATH=";

  /* find PATH */
  for (int i = 0; envp[i] != NULL; ++i)
  {
    check_str = envp[i];
    if (check_str.substr(0, 5) == "PATH=")
      break ;
    check_str = "PATH=";
  }
  return(check_str);
}

/* Extracts REQUEST_METHOD, SCRIPT_NAME and QUERY_STRING from the frist
 * line of the http request. Sets them as customenvp[2,3,4]
*/
static void parse_first_line(const std::string& line, char **&custom_envp)
{
  std::size_t start = 0, end;
  std::vector<std::string> tokens;

  end = line.find(' ');
  /* gets the first 2 tokens, not the http version */
  while (end != std::string::npos)
  {
    tokens.push_back(line.substr(start, end - start));
    start = end + 1;
    end = line.find(' ', start);
  }
  custom_envp[2] = cpp_strdup("REQUEST_METHOD=" + tokens[0]);

  /* extract the query string if it exists */
  end = tokens[1].find('?');
  if(end != std::string::npos)
  {
    custom_envp[3] = cpp_strdup("SCRIPT_NAME=" + tokens[1].substr(0, end));
    custom_envp[4] = cpp_strdup("QUERY_STRING=" + tokens[1].substr(end+1));
  }
  else
  {
    custom_envp[3] = cpp_strdup("SCRIPT_NAME=" + tokens[1]);
    custom_envp[4] = cpp_strdup("QUERY_STRING=");
  }
}


/* Searches for a http header field from tokenVec, that starts with *to_find*.
 * If field is found, returns the value, if not, returns ""
*/
static std::string find_value(const std::vector<std::string>& tokenVec, const std::string to_find)
{
  for (std::vector<std::string>::const_iterator it = tokenVec.begin(); it < tokenVec.end(); ++it)
    if ((*it).substr(0, to_find.size()) == to_find)
      return ((*it).substr(to_find.size() + 1));    // skip the space
  return ("");
}


/* Takes my custom_envp and finds 2 values: SCRIPT_NAME and DOCUMENT_ROOT and combines
 * them to make a resulting string of the full absolute path of the executable.
 * SCRIPT_NAME and DOCUMENT_ROOT should always be available and have values.
*/
static void get_program_name(std::string& result, char **custom_envp)
{
  std::string script_name;
  std::string doc_root;
  std::string token;

  for(int i = 0; custom_envp[i] != NULL; ++i)
  {
    if (std::strncmp(custom_envp[i], "SCRIPT_NAME=", 12) == 0)
    {
      token = custom_envp[i];
      script_name = token.substr(13);   // exclude the starting '/'
    }
    else if (std::strncmp(custom_envp[i], "DOCUMENT_ROOT=", 14) == 0)
    {
      token = custom_envp[i];
      doc_root = token.substr(14);
    }
  }
  result = doc_root + "www/" + script_name;   //TODO: do i need to add www/ in production??
}

/* Takes a 'token vector', envp; and fills the custom_envp with values. The logic is a little hardcoded,
 * taking into account the size of custom_envp and the custom env variable names that we need.
*/
static void create_new_envp(const std::vector<std::string>& tokenVec, char **&custom_envp, const char **envp)
{
  custom_envp[0] = cpp_strdup(find_path(envp));   // PATH
  custom_envp[1] = cpp_strdup("DOCUMENT_ROOT=/home/tom/Desktop/code/school/webserv/");   // TODO: get from Server
  parse_first_line(tokenVec[0], custom_envp);
  custom_envp[5] = cpp_strdup("CONTENT_TYPE=" + find_value(tokenVec, "Content-Type:"));
  custom_envp[6] = cpp_strdup("CONTENT_LENGHT=" + find_value(tokenVec, "Content-Lenght:"));
  custom_envp[7] = cpp_strdup("HTTP_USER_AGENT=" + find_value(tokenVec, "User-Agent:"));
  custom_envp[8] = cpp_strdup("HTTP_HOST=" + find_value(tokenVec, "Host:"));
  custom_envp[9] = cpp_strdup("HTTP_REFERRER=" + find_value(tokenVec, "Referrer:"));
  custom_envp[10] = NULL;
}


static void pass_request_body(std::vector<std::string>& tokenVec, int write_end)
{
 
  // 1. get the post body from the tokenVec
  // 2. write it into the write end
  // 3. close the write end

  std::stringstream ss;
  bool              append = false;
  for (int i = 0; i < tokenVec.size(); ++i)
  {
    if (append)
      ss << tokenVec[i];    // TODO: does each of the lines have to end with \r\n?
    if (tokenVec[i] == "")
      append = true;
  }
  // write into the pipe
  write(write_end, ss.str().c_str(), ss.str().size());
  // this will close it for the the parent and the child
  close(write_end);
}

/* Loops and sleeps until the MAX_WAIT sleeping time is reached. Checks if the pid has
 * exited on each iteration by checking the return value of waitpid. Sends SIGKILL if 
 * max timeout is reached by the child.
*/
static void smart_wait(pid_t pid)
{
  unsigned int time_slept = 0;
  
  while (time_slept < MAX_WAIT)
  {
    if (waitpid(pid, NULL, WNOHANG) > 0)
      return ;
    usleep(1000);
    time_slept += 1000;
  }
  kill(pid, SIGKILL);     // max_timeout
}

int cgi_parse(const char **envp)
{
  std::string request = 
    "GET /cgi-bin/a.out HTTP/1.1\n"
    "Host: localhost:8080\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\n"
    "Accept-Language: en-US,en;q=0.5\n"
    "Accept-Encoding: gzip, deflate, br, zstd\n"
    "Sec-GPC: 1\n"
    "Connection: keep-alive\n"
    "Upgrade-Insecure-Requests: 1\n"
    "Sec-Fetch-Dest: document\n"
    "Sec-Fetch-Mode: navigate\n"
    "Sec-Fetch-Site: none\n"
    "Sec-Fetch-User: ?1\n"
    "Priority: u=0, i\n"
    "\n"
    "thisis=data&test=true\n";

  std::vector<std::string> tokenVec = tokenize_request(request);
  char  **custom_envp = new char *[ENVP_SIZE + 1];       // hardcoded, because thats how many i choose to handle
  pid_t pid;
  int   pipefd[2][2];       // pipefd[0] is the input pipe, pipefd[1] is for child output
  std::string program_name;
  char *program_args[2];

  create_new_envp(tokenVec, custom_envp, envp);
  if (pipe(pipefd[0]) == -1)
    return(cgi_error("cgi pipe1()"));
  if (pipe(pipefd[1]) == -1)
    return(cgi_error("cgi pipe1()"));

  pass_request_body(tokenVec, pipefd[0][1]);

  pid = fork();
  if (pid == -1)
    return (cgi_error("cgi fork()"));
  else if (pid == 0)
  {
    // set input from parent (write end is already closed before fork)
    dup2(pipefd[0][0], 0);
    close(pipefd[0][0]);

    // close read end
    close(pipefd[1][0]);
    dup2(pipefd[1][1], 1);
    close(pipefd[1][1]);

    get_program_name(program_name, custom_envp);
    program_args[0] = const_cast<char *>(program_name.c_str());
    program_args[1] = NULL;

    execve(program_name.c_str(), program_args, custom_envp);
    cgi_error("cgi execve()");
    exit(1);
  }

  close(pipefd[0][0]); // close the read end of the read pipe
  smart_wait(pid);

  // ------------ TEST ---------------
  // close write end
  close(pipefd[1][1]);
  char read_buffer[256];

  // the pipe will not be open for reading until the child exits.
  std::cout << "this is what the child said:\n" << std::endl;
  int bytes_read;
  while ((bytes_read = read(pipefd[1][0], read_buffer, 256)) > 0)
    write(1, read_buffer, bytes_read);
  close(pipefd[1][0]);
  // --------------------------------
  

  for (int i = 0; i < ENVP_SIZE + 1; ++i)
    delete[] custom_envp[i];
  delete[] custom_envp;
  return (0);
}

// TEST main()
int main(int argc, char **argv, char **envp)
{
  cgi_parse(const_cast<const char**>(envp));
  return(0);
}

