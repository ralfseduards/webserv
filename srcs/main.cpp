#include "../includes/webserv.hpp"


int main(void) {

  int sock;
  int client_fd;
  struct sockaddr_in addr;

  prepareSocket(&sock, &addr);

  while (true) {

    if (listen(sock, 10) < 0)
      return (1);

    std::vector<char> request(8192);

    //TODO: put client FDs into a list to store
    client_fd = accept(sock, 0,0);
    if (client_fd == -1)
      return (1);

    //TODO: use poll on the FD list

    //TODO: actually parse the requests
    ssize_t message_length = recv(client_fd, request.data(), request.size(), 0);
    if (message_length == -1)
      return (1);
    if (message_length < static_cast<ssize_t>(request.size()))
      request.shrink_to_fit();


    std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));

    std::ifstream	infile;
    infile.open(request_file);

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
