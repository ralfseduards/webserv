#include "../includes/cgi.hpp"

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

// >>>>>>>>>>>>>>>> ENV implemented <<<<<<<<<<<<<<<<<
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

static void pass_request_body(std::string& body, int write_end)
{
  std::stringstream ss(body);

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

//TODO: protect my news(mallocs)

int cgi_parse(Client& client)
{
  char  **custom_envp;                                                    // hardcoded, because thats how many i choose to handle
  pid_t pid;
  int   pipefd[2][2];                                                     // pipefd[0] is the input pipe, pipefd[1] is for child output
  char *program_args[2];
  std::string program_name, program_dir;
  char read_buffer[256];
  int bytes_read;

  custom_envp = new char *[ENVP_SIZE + 1];

  create_new_envp(client, custom_envp);
  if (pipe(pipefd[0]) == -1)
    return(cgi_error("cgi pipe1()"));
  if (pipe(pipefd[1]) == -1)
    return(cgi_error("cgi pipe1()"));

  pass_request_body(client.request, pipefd[0][1]);

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

    get_program_name(program_name, program_dir, custom_envp, client);

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

  // close write end
  close(pipefd[1][1]);

  while ((bytes_read = read(pipefd[1][0], read_buffer, 256)) > 0)
  {
    std::string ret(read_buffer, bytes_read);
    client.waitlist[0].response.file_content += ret;
  }

  client.waitlist[0].response.has_content = true;
  client.waitlist[0].response.http_code = 200;
  client.waitlist[0].response.cgi_response = true;
  http_response(client, client.waitlist[0].response);

  close(pipefd[1][0]);

  for (int i = 0; i < ENVP_SIZE + 1; ++i)
    delete[] custom_envp[i];
  delete[] custom_envp;
  return (0);
}
