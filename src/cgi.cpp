#include "../includes/cgi.hpp"

int cgi_error(const char *str, Client& client, int code)
{
  std::perror(str);

  client.waitlist[0].response.http_code = code;
  switch (code)
  {
    case 404:
      return (2);
    case 403:
      return (3);
    default:
      return (1);
  }
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
static int smart_wait(pid_t pid)
{
  unsigned int time_slept = 0;
  int status;

  while (time_slept < MAX_WAIT)
  {
    if (waitpid(pid, &status, WNOHANG) > 0)
      return (WEXITSTATUS(status));
    usleep(1000);
    time_slept += 1000;
  }
  kill(pid, SIGKILL);     // max_timeout
  return (status);
}

//TODO: protect my news(mallocs)

/* Processes the cgi request, that was called, becuase the file requested was from the
 * /cgi-dir/.
 *
 * >>>>>>>>>>>>>>>> the cgi logic: <<<<<<<<<<<<<<<<
 * 1. construct the environment
 *  1. copy the real one
 *  2. add the http header parsed as environ variables
 *  3. add the required ones by the standard
 * 2. pipe, fork, execve
 *  1. write into stdin the message body of the http request
 *  2. stdout is the response from the script
 * 3. wait (maybe some no-hang/timeout logic)
 * 4. parse the stdout of the script and make a http response
 *
 * >>>>>>>>>>>>>>>> ENV implemented <<<<<<<<<<<<<<<<<
 * PATH                     -- envp path                      -- envp
 * DOCUMENT_ROOT            -- the root of the server         -- config file
 * REQUEST_METHOD           -- from http 'GET/POST line'      -- http header
 * SCRIPT_NAME              -- from http 'GET/POST line'      -- http header
 * QUERY_STRING             -- from http 'GET/POST line'      -- http header
 * CONTENT_TYPE             -- from http header var           -- http header
 * CONTENT_LENGTH           -- from http header var           -- http header
 * HTTP_USER_AGENT          -- from http header var           -- http header
 * HTTP_HOST                -- from http header var           -- http header
 * HTTP_REFFERER            -- from http header var           -- http header
 *
 * Sets the http status code in client.waitlist[0].response.http_code
 * Function returns 0 on success and a http code on error.
*/
int cgi_parse(Client& client)
{
  char  **custom_envp;                                                    // hardcoded, because thats how many i choose to handle
  pid_t pid;
  int   pipefd[2][2];                                                     // pipefd[0] is the input pipe, pipefd[1] is for child output
  char *program_args[2];
  std::string program_name, program_dir;
  char read_buffer[256];
  int bytes_read, status;

  custom_envp = new char *[ENVP_SIZE + 1];

    create_new_envp(client, custom_envp);
  if (pipe(pipefd[0]) == -1)
    return(cgi_error("cgi pipe1()", client, 500));
  if (pipe(pipefd[1]) == -1)
    return(cgi_error("cgi pipe1()", client, 500));

  pass_request_body(client.request, pipefd[0][1]);

  pid = fork();
  if (pid == -1)
    return (cgi_error("cgi fork()", client, 500));
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
    exit(cgi_error("cgi child chdir()", client, 404));
  program_args[0] = const_cast<char *>(program_name.c_str());
  program_args[1] = NULL;

  execve(program_name.c_str(), program_args, custom_envp);
  switch (errno)
  {
    case ENOENT:
      exit(cgi_error("cgi child execve()", client, 404));
    case EACCES:
      exit(cgi_error("cgi child execve()", client, 403));
    default:
      exit(cgi_error("cgi child execve()", client, 500));
  }
}

  close(pipefd[0][0]); // close the read end of the read pipe
  status = smart_wait(pid);
  // close write end
  close(pipefd[1][1]);

  if (status != 0)
  {
    /* error in child process */
    switch (status)
    {
      case 2:
        client.waitlist[0].response.http_code = 404;
        break;
      case 3:
        client.waitlist[0].response.http_code = 403;
        break;
      default:
        client.waitlist[0].response.http_code = 500;
        break;
    }
    client.waitlist[0].response.has_content = false;
  }
  else
{
    /* normal execution */
    while ((bytes_read = read(pipefd[1][0], read_buffer, 256)) > 0)
    {
      std::string ret(read_buffer, bytes_read);
      client.waitlist[0].response.file_content += ret;
    }

    client.waitlist[0].response.has_content = true;
    client.waitlist[0].response.http_code = 200;
    client.waitlist[0].response.manual_response = true;
  }
  close(pipefd[1][0]);

  for (int i = 0; i < ENVP_SIZE + 1; ++i)
    delete[] custom_envp[i];
  delete[] custom_envp;
  return (0);
}
