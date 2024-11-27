#include "../includes/webserv.hpp"

int g_sig = 0;

int main(void) {

  int listening_socket;
  int client_fd = -1;
  struct sockaddr_in addr;
  std::vector<pollfd> fd_list;
  std::vector<char> request(8192); //Max size of requests 8 MB
  signal(SIGINT, signal_handler);
  prepareSocket(listening_socket, addr);

  pollfd server_fd;
  server_fd.fd = listening_socket;
  server_fd.events = POLLIN;
  server_fd.revents = 0;
  fd_list.push_back(server_fd);

  //main loop
  while (true && !g_sig) {
    if (poll(fd_list.data(), fd_list.size(), -1) == -1)
      return (1);
    //should be refactored
    for (size_t i = 0; i < fd_list.size(); ++i) {
      if (fd_list[i].revents & POLLHUP) {
        std::clog << "Hang-up detected on fd " << fd_list[i].fd << std::endl;
        close(fd_list[i].fd);
        fd_list.erase(fd_list.begin() + i);
        --i;
        continue;
      }
      if (fd_list[i].revents & POLLIN) {
        if (fd_list[i].fd == listening_socket) {
          client_fd = accept(listening_socket, 0,0);
          if (client_fd < 0)
            continue;
          add_client(client_fd, fd_list);
        }
        else {
          ssize_t message_length = recv(fd_list[i].fd, request.data(), request.size(), 0);

          //extract to error handling
          if (message_length <= 0) {
            if (message_length == 0)
              std::clog << "Client disconnected" << std::endl;
            else
              std::cerr << "Recv failed" << std::endl;
            close(fd_list[i].fd);
            fd_list.erase(fd_list.begin() + i);
            --i;
          }
          else {
            //TODO: actually parse the request
            std::string response;
            build_response(request, response);
            send(fd_list[i].fd, response.c_str(), response.length(), 0);
          }
        }
      }
    }
  }

  for (size_t i = 0; i < fd_list.size(); ++i) {
    close(fd_list[i].fd);
  }
}

void build_response(std::vector<char>& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));
  struct stat stats;
  stat(request_file.c_str(), &stats);

  if (!S_ISREG(stats.st_mode)) {
    std::ifstream infile("404.html");
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(response.size()) + "\r\n"
               "\r\n"
              + response;
    infile.close();
  }
  else if (access(request_file.c_str(), F_OK) == -1) {
    std::ifstream infile("403.html");
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = "HTTP/1.1 403 Forbidden\r\n"
               "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(response.size()) + "\r\n"
               "\r\n"
              + response;
    infile.close();
  }
  else {
    std::ifstream	infile(request_file);
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response =  "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
               "Content-Length: " + std::to_string(response.size()) + "\r\n"
               "\r\n"
               + response;
    infile.close();
  }
}

void add_client(int client_fd, std::vector<pollfd>& fd_list) {
  pollfd client_poll_fd;
  client_poll_fd.fd = client_fd;
  client_poll_fd.events = POLLIN;
  client_poll_fd.revents = 0;
  fd_list.push_back(client_poll_fd);
  std::clog << "New Client added: " << client_fd << "\n";
}

void signal_handler(int sig) {
  (void)sig;
  g_sig = 1;
}


void error_handler() {

}
