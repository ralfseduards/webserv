#include "../includes/webserv.hpp"


int main(void) {

  int listening_socket;
  int client_fd = -1;
  struct sockaddr_in addr;
  std::vector<pollfd> fd_list;
  pollfd server_fd;

  prepareSocket(&listening_socket, &addr);

  server_fd.fd = listening_socket;
  server_fd.events = POLLIN;
  server_fd.revents = 0;

  fd_list.push_back(server_fd);

  std::vector<char> request(8192); //Max size of requests 8 MB

  while (true) {
    if (poll(fd_list.data(), fd_list.size(), -1) == -1)
      return (1);
    for (size_t i = 0; i < fd_list.size(); ++i) {
      if (fd_list[i].revents & POLLIN) {
        if (fd_list[i].fd == listening_socket) {
          client_fd = accept(listening_socket, 0,0);
          if (client_fd < 0)
            continue;

          pollfd client_poll_fd;
          client_poll_fd.fd = client_fd;
          client_poll_fd.events = POLLIN;
          client_poll_fd.revents = 0;

          fd_list.push_back(client_poll_fd);
        } else {
          ssize_t message_length = recv(fd_list[i].fd, request.data(), request.size(), 0);

          if (message_length <= 0) {
            if (message_length == 0)
              std::clog << "Client disconnected" << std::endl;
            else
              std::cerr << "Recv failed" << std::endl;
            close(fd_list[i].fd);
            fd_list.erase(fd_list.begin() + i);
            --i;
          } else {
            std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));

            std::ifstream	infile;
            infile.open(request_file);

            std::string response((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

            //TODO: build response
            response =  "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: " + std::to_string(response.size()) + "\r\n"
                        "\r\n"
                        + response;
            send(fd_list[i].fd, response.c_str(), response.length(), 0);
          }
        }
      }
    }
  }
  close(listening_socket);
}

void error_handler() {

}
