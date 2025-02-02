
#include <unistd.h>
#include <stdio.h>

#define READ_SIZE 256

int main(int argc, char **argv, char **envp)
{
  char buffer[READ_SIZE];
  int bytes_read;

  getcwd(buffer, sizeof(buffer));
  printf("pwd: %s\n", buffer);

  // prints the stdin
  write(1, "printing the stdin:\n", 20);
  while ((bytes_read = read(0, buffer, READ_SIZE)) > 0)
    write(1, buffer, bytes_read);

  // prints the envp
  write(1, "\n\nprinting the envp:\n", 21);
  for (int i = 0; envp[i] != NULL; ++i)
    printf("%s", envp[i]);


  // write something to stdout
  printf("\n\nThis is a write test\n");
  printf("Content-Type: text/html\n");
  printf("<html>\n");
  printf("  <body>\n");
  printf("    <h1>hello world...</h1>\n");
  printf("  </body>\n");
  printf("</html>\n");
}
