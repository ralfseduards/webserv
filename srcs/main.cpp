#include "../includes/webserv.hpp"

int g_sig = 0;

int main(void) {
  signal(SIGINT, signal_handler);

  std::vector<pollfd> fd_vec;       //A map of all pollfds, Servers at front
  std::map<int, Server> server_map; //A map of the server data keyed to the fd
  std::map<int, Client> client_map; //A map of the client data keyed to the fd

  //TODO: Parse config file

  createServers(fd_vec, server_map);

  // prepareSocket(fd_vec, server_map);

  //main loop
  while (true && !g_sig) {
    if (poll(fd_vec.data(), fd_vec.size(), -1) == -1)
      return (1);

    for (size_t i = 0; i < fd_vec.size(); ++i) {
      // if (fd_vec[i].revents & POLLHUP) {
      //   client_hung_up(fd_vec, i);
      //   continue;
      // }
      if (fd_vec[i].revents & POLLNVAL) {
        std::clog << "Client " << i << " with fd " << fd_vec[i].fd << " invalid." << std::endl;
        client_remove(i, client_map, fd_vec);
        continue;
      }
      if (fd_vec[i].revents & POLLIN) {
        if (i < server_map.size()) {
          std::map<int, Server>::iterator it = server_map.find(fd_vec[i].fd);
          int client_fd = accept((*it).second.server_socket, 0,0);
          if (client_fd < 0)
            continue;
          client_add_vec(client_fd, fd_vec);
          client_add_map(client_map, client_fd, &((*it).second));
        }
        else {
          (void)receive_request(fd_vec[i], client_map[fd_vec[i].fd]);
          if (client_map[fd_vec[i].fd].status != OK && client_map[fd_vec[i].fd].status != RECEIVING) {
            client_error(i, fd_vec[i].fd, client_map[fd_vec[i].fd].status);
            client_remove(i, client_map, fd_vec);
          }
          else if (client_map[fd_vec[i].fd].waitlist.size() > 0)
            process_request(client_map[fd_vec[i].fd]);
        }
      }
    }
  }
  for (size_t i = 0; i < fd_vec.size(); ++i) {
    close(fd_vec[i].fd);
  }
}

void signal_handler(int sig) {
  (void)sig;
  g_sig = 1;
}
