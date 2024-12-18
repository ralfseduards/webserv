#include "../includes/webserv.hpp"

//Removes the Client from the Map and vector does NOT close any connections
void client_remove(size_t& i, std::map<int, Client>& client_map, std::vector<pollfd>& fd_vec) {
  std::clog << "Removing Client " << i << " with fd " << fd_vec[i].fd << std::endl;
  client_map.erase(fd_vec[i].fd);
  fd_vec.erase(fd_vec.begin() + i);
  --i;
  std::clog << "Client removed" << std::endl;
  return ;
}

void client_add_vec(int client_fd, std::vector<pollfd>& fd_vec) {
  pollfd client_poll_fd;
  client_poll_fd.fd = client_fd;
  client_poll_fd.events = POLLIN;
  client_poll_fd.revents = 0;
  fd_vec.push_back(client_poll_fd);
  std::clog << "New Client added: " << fd_vec.size() << " on fd " << client_fd << std::endl;
  return ;
}

void client_add_map(std::map<int, Client>& client_map, int fd, int server_fd) {
  Client new_client;

  new_client.fd = fd;
  new_client.status = OK;
  new_client.server_fd = server_fd;
  client_map.emplace(fd, new_client);
  return ;
}

void client_error(size_t i, int fd, int status) {

  switch (status)
  {
  case DISCONNECTED:
    std::clog << "Client " << i << " on fd " << fd << " Disconnected" << std::endl;
    close(fd);
    break;

  case ERROR:
    std::clog << "Client " << i << " on fd " << fd << " Recv Error" << std::endl;
    close(fd);
    break;

  //TODO: send error 400
  case HEADER_INVAL_COLON:
    std::clog << "Client " << i << " on fd " << fd << " Malformed Header" << std::endl;
    break;

  case HEADER_INVAL_REGEX_KEY:
    std::clog << "Client " << i << " on fd " << fd << " Malformed Header" << std::endl;
    break;

  case HEADER_INVAL_REGEX_VAL:
    std::clog << "Client " << i << " on fd " << fd << " Malformed Header" << std::endl;
    break;

  case HEADER_INVAL_SIZE:
    std::clog << "Client " << i << " on fd " << fd << " Malformed Header" << std::endl;
    break;

  //TODO: send error 413
  case BODY_TOO_LARGE:
    std::clog << "Client " << i << " on fd " << fd << " Content too large" << std::endl;

  default:
    std::cerr << "Unknown Error status on " << i << "with fd " << fd << "\nError: " << status << std::endl;
    break;
  }
  return ;
}
