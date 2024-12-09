#include "../includes/webserv.hpp"

void client_add(int client_fd, std::vector<pollfd>& fd_vec) {
  pollfd client_poll_fd;
  client_poll_fd.fd = client_fd;
  client_poll_fd.events = POLLIN;
  client_poll_fd.revents = 0;
  fd_vec.push_back(client_poll_fd);
  std::clog << "New Client added: " << fd_vec.size() << " on fd " << client_fd << "\n";
}

void client_add_map( std::map<int, Client>& client_map, int fd) {
  Client new_client;

  new_client.fd = fd;
  new_client.status = OK;
  client_map.emplace(fd, new_client);
  return ;
}

void client_invalid(std::vector<pollfd>& fd_vec, size_t& i) {
  std::clog << "Client " << i << " invalid on fd " << fd_vec[i].fd << std::endl;
  close(fd_vec[i].fd);
  fd_vec.erase(fd_vec.begin() + i);
  --i;
}

// void client_hung_up(std::vector<pollfd>& fd_vec, size_t& i) {
//   std::clog << "Client " << i << " hung up on fd " << fd_vec[i].fd << std::endl;
//   close(fd_vec[i].fd);
//   fd_vec.erase(fd_vec.begin() + i);
//   --i;
// }
