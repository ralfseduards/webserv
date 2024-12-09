#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"

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
  std::map<int, Client> client_map;
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
          add_client_map(client_map, client_fd);
        }
        else {
          int status = receive_request(fd_vec[i], client_map[fd_vec[i].fd]);
          if (status != 0)
            request_error(fd_vec, client_map, i, status);
          process_request(client_map[fd_vec[i].fd]);
        }
      }
    }
  }
  for (size_t i = 0; i < fd_vec.size(); ++i) {
    close(fd_vec[i].fd);
  }
}

void add_client_map( std::map<int, Client>& client_map, int fd) {
  Client new_client;

  new_client.fd = fd;
  client_map.emplace(fd, new_client);
  return ;
}

void request_error(std::vector<pollfd>& fd_vec,  std::map<int, Client>& client_map, size_t& i, int status) {
    if (status == DISCONNECTED)
      std::clog << "Client " << i << " disconnected" << std::endl;
    else
      std::cerr << "Client " << i << " Recv failed" << std::endl;
    close(fd_vec[i].fd);
    client_map.erase(fd_vec[i].fd);
    fd_vec.erase(fd_vec.begin() + i);
    --i;
}



void process_request(Client& client) {
  std::string response;

  build_response(client.waitlist[0].start_line, response);
  send(client.fd, response.c_str(), response.length(), 0);
  client.waitlist.erase(client.waitlist.begin());
  client.request.clear();
    //TODO: actually parse the request

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
