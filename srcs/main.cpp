#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <string>

enum port {
  PORT = 8080
};

int main(void) {

  int sock;
  int client_fd;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    return (1);

  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;


  bind(sock, (struct sockaddr *)&addr, sizeof(addr));


  while (true) {

    if (listen(sock, 10) < 0)
      return (1);

    //TODO: dynamically allocate buffer size
    char request[256] = {0};

    //TODO: put client FDs into a list to store
    client_fd = accept(sock, 0,0);
    if (client_fd == -1)
      return (1);

    //TODO: use poll on the FD list


    //TODO: actually parse the requests
    ssize_t message_length = recv(client_fd, request, 256, 0);
    if (message_length == -1)
      return (1);

    char* file = request + 5; // Skip the GET / part of the request

    //TODO: replace with cpp function
    *strchr(file, ' ') = 0;

    std::ifstream	infile;
    infile.open(file);

    std::string response((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    response =  "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(response.size()) + "\r\n"
                "\r\n"
                + response;

    send(client_fd, response.c_str(), response.length(), 0);


    close(client_fd);
    infile.close();
    //TODO: potentially keep client FD open
  }

}
