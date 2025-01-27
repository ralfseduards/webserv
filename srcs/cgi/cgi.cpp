#include "../../includes/webserv.hpp"

#ifndef CGI_DIR
# define CGI_DIR "cgi-dir"
#endif

#define ENVP_SIZE 10

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
  newStr[i] = '\0';
  return (newStr);
}

/* splits the request string on \n and returns a vector with the lines*/
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

static void create_new_envp(const std::vector<std::string>& tokenVec, char **&custom_envp, const char **envp)
{
  custom_envp[0] = cpp_strdup(find_path(envp));   // PATH
  custom_envp[1] = cpp_strdup("DOCUMENT_ROOT=");   // TODO: get from Server
  parse_first_line(tokenVec[0], custom_envp);
}

int cgi_parse(const char **envp)
{
  std::string request = 
    "GET /cgi-bin/simple.py? HTTP/1.1\n"
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
    "Priority: u=0, i\n";

  std::vector<std::string>  tokenVec = tokenize_request(request);
  char  **custom_envp = new char *[ENVP_SIZE + 1]();       // hardcoded, because thats how many i choose to handle
  pid_t pid;
  int   pipefd[2];
  char *program_name;
  char *program_args[2];

  create_new_envp(tokenVec, custom_envp, envp);


  /*
  if (pipe(pipefd) == -1)
    return(cgi_error("cgi pipe():"));
  pid = fork()
  if (pid == -1)
    return (cgi_error("cgi fork():"))
  else if (pid == 0)
  {
    // dup2 
    // put the post content into pipe read end
    // TODO: do i need 2 pipes?
    program_args = {program_name, NULL};
    execve(program_name, program_args, custom_envp);
    cgi_error("cgi execve():");
    exit(1);
  }
  */ 
  // wait();
  // have a timeout
  // construct server response form pipep[0]


  for (int i = 0; custom_envp[i] != NULL && i < ENVP_SIZE + 1; ++i)
  {
    std::cout << custom_envp[i] << '\n';
    delete[] custom_envp[i];
  }

  return (0);
}

// TEST main()
int main(int argc, char **argv, char **envp)
{
  cgi_parse(const_cast<const char**>(envp));
  return(0);
}

