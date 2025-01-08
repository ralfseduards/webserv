#include "../includes/webserv.hpp"

int g_sig = 0;

int main(void) {
  signal(SIGINT, signal_handler);
  std::vector<pollfd> fd_vec;       //A map of all pollfds, Servers at front
  std::map<int, Server> server_map; //A map of the server data keyed to the fd
  std::map<int, Client> client_map; //A map of the client data keyed to the fd
  //TODO: Parse config file and put the servers in a vector maybe
  createServers(fd_vec, server_map);

  while (true && !g_sig) {   //main loop
    if (poll(fd_vec.data(), fd_vec.size(), -1) == -1)
      return (1);

    for (std::size_t i = 0; i < fd_vec.size(); ++i) {  //loop through the FDs

      if (fd_vec[i].revents & POLLNVAL) {
        client_error(i, fd_vec[i].fd, POLLINVALID);
        client_remove(i, client_map, fd_vec);
        continue;
      }

      if (fd_vec[i].revents & POLLERR) {
        client_error(i, fd_vec[i].fd, ERROR);
        client_remove(i, client_map, fd_vec);
        continue;
      }

      // if (fd_vec[i].revents & POLLHUP) { //potentially keep reading until EOF
      //   client_error(i, fd_vec[i].fd, HUNGUP);
      //   client_remove(i, client_map, fd_vec);
      //   continue;
      // }

      if (fd_vec[i].revents & POLLIN && i < server_map.size())
        new_client(fd_vec, server_map, client_map, i);
      if (fd_vec[i].revents & POLLIN && i >= server_map.size()) {
        if (chdir(client_map.at(fd_vec[i].fd).server->root_directory.c_str()) == -1)
          break;
        incoming_message(fd_vec, client_map, i);
      }
    }
  }
  close_fds(fd_vec);
  return (0);
}

void close_fds(std::vector<pollfd>& fd_vec) {
  for (size_t i = 0; i < fd_vec.size(); ++i) {
    close(fd_vec[i].fd);
  }
}

void signal_handler(int sig) {
  (void)sig;
  g_sig = 1;
}
