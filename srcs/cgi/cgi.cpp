#include "cgi.hpp"

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

int cgi_error(const char *str)
{
  perror(str);
  return (1);     // TODO: return error
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

  char  **custom_envp = new char *[ENVP_SIZE + 1];       // hardcoded, because thats how many i choose to handle
  std::vector<std::string> tokenVec = tokenize_request(request);
  pid_t pid;
  int   pipefd[2][2];       // pipefd[0] is the input pipe, pipefd[1] is for child output
  char *program_args[2];
  std::string program_name, program_dir;

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

    get_program_name(program_name, program_dir, custom_envp);

    if (chdir(program_dir.c_str()) != 0)
      cgi_error("cgi child chdir()");
    program_args[0] = const_cast<char *>(program_name.c_str());
    program_args[1] = NULL;

    execve(program_name.c_str(), program_args, custom_envp);
    cgi_error("cgi child execve()");
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

