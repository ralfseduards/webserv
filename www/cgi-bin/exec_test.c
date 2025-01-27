
#include <unistd.h>
#include <stdio.h>

/* a simple test for executing a python script */

int main(int argc, char **argv)
{
  if (argc != 2)
    return (1);

  char *custom_envp[] = {
    "HOME=/home/tom/Desktop/code/school/webserv/cgi-bin",
    "TEST=true",
    "PATH=/home/tom/Desktop/code/school/webserv/cgi-bin",
    NULL
  };

  execve(argv[1], (char *const[]){argv[1], NULL}, custom_envp);
  perror(NULL);
  return (1);
}
