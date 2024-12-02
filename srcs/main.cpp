#include "../includes/webserv.hpp"
#include "../includes/Request.hpp"

int g_sig = 0;

void client_hangup(std::vector<pollfd>& fd_vec, size_t& i) {
  std::clog << "Hang-up detected on fd " << fd_vec[i].fd << std::endl;
  close(fd_vec[i].fd);
  fd_vec.erase(fd_vec.begin() + i);
  --i;
}

int main(void) {

  int listening_socket;
  struct sockaddr_in addr;
  std::vector<pollfd> fd_vec;
  signal(SIGINT, signal_handler);
  prepareSocket(listening_socket, addr, fd_vec);

  //main loop
  while (true && !g_sig) {
    if (poll(fd_vec.data(), fd_vec.size(), -1) == -1)
      return (1);
    //should be refactored
    for (size_t i = 0; i < fd_vec.size(); ++i) {
      if (fd_vec[i].revents & POLLHUP) {
        client_hangup(fd_vec, i);
        continue;
      }
      if (fd_vec[i].revents & POLLIN) {
        if (fd_vec[i].fd == listening_socket) {
          int client_fd = accept(listening_socket, 0,0);
          if (client_fd < 0)
            continue;
          add_client(client_fd, fd_vec);
        }
        else
          process_request(fd_vec, i);
      }
    }
  }
  for (size_t i = 0; i < fd_vec.size(); ++i) {
    close(fd_vec[i].fd);
  }
}

void request_error(std::vector<pollfd>& fd_vec, size_t& i, ssize_t& message_length) {
    if (message_length == 0)
      std::clog << "Client disconnected" << std::endl;
    else
      std::cerr << "Recv failed" << std::endl;
    close(fd_vec[i].fd);
    fd_vec.erase(fd_vec.begin() + i);
    --i;
  }

void process_request(std::vector<pollfd>& fd_vec, size_t& i) {
  Request request;

  request.message_length = recv(fd_vec[i].fd, request.raw_string.data(), request.raw_string.size(), 0);
  if (request.message_length <= 0) {
    request_error(fd_vec, i, request.message_length);
    request.raw_string.clear();
  } else {
    //TODO: actually parse the request
    std::string response;
    build_response(request.raw_string, response);
    send(fd_vec[i].fd, response.c_str(), response.length(), 0);
  }
}

void build_response(std::vector<char>& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));
  struct stat stats;
  stat(request_file.c_str(), &stats);

  if (!S_ISREG(stats.st_mode) || access(request_file.c_str(), F_OK) == -1 ) {
    std::ifstream infile("404.html");
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = "HTTP/1.1 404 Not Found\r\n"
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

void add_client(int client_fd, std::vector<pollfd>& fd_vec) {
  pollfd client_poll_fd;
  client_poll_fd.fd = client_fd;
  client_poll_fd.events = POLLIN;
  client_poll_fd.revents = 0;
  fd_vec.push_back(client_poll_fd);
  std::clog << "New Client added: " << client_fd << "\n";
}

void signal_handler(int sig) {
  (void)sig;
  g_sig = 1;
}

void error_handler() {

}
