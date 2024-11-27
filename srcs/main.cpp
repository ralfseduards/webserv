#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

enum port {
  PORT = 8080
};

int main(void) {

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    return (1);

  unsigned short host_port = htons(PORT);

  struct sockaddr_in addr = {
    AF_INET, //domain
    host_port, //Port 8080 in hex and flipped endian
    0
  };

  bind(sock, (struct sockaddr *)&addr, sizeof(addr));

  while (true) {

    if (listen(sock, 10) == -1)
      return (1);

    //TODO: dynamically allocate buffer size
    char buffer[256] = {0};

    //TODO: put client FDs into a list to store
    int client_fd = accept(sock, 0,0);
    if (client_fd == -1)
      return (1);

    //TODO: use poll on the FD list


    //TODO: actually parse the requests
    ssize_t  message_length = recv(client_fd, buffer, 256, 0);
    if (message_length == -1)
      return (1);

    char* file = buffer + 5; // Skip the GET / part of the request

    //TODO: replace with cpp function
    *strchr(file, ' ') = 0;

    std::ifstream	infile;
    infile.open(file);

    //TODO: add http headers
    std::string buf((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    send(client_fd, buf.c_str(), buf.length(), MSG_DONTWAIT);


    infile.close();

    //TODO: potentially keep client FD open
    close(client_fd);
  }

}
