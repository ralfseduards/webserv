#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"

int g_sig = 0;

int main(void) {

  int listening_socket;

  std::vector<pollfd> fd_vec;
  std::map<int, Client> client_map; //A map of the client data keyed to the fd

  signal(SIGINT, signal_handler);
  prepareSocket(listening_socket, fd_vec);

  //main loop
  while (true && !g_sig) {
    if (poll(fd_vec.data(), fd_vec.size(), -1) == -1)
      return (1);
    //should be refactored
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
        if (fd_vec[i].fd == listening_socket) {
          int client_fd = accept(listening_socket, 0,0);
          if (client_fd < 0)
            continue;
          client_add_vec(client_fd, fd_vec);
          client_add_map(client_map, client_fd);
        }
        else {
          (void)receive_request(fd_vec[i], client_map[fd_vec[i].fd]);
          if (client_map[fd_vec[i].fd].status != OK && client_map[fd_vec[i].fd].status != RECEIVING) {
            client_error(i, fd_vec[i].fd, client_map[fd_vec[i].fd].status);
            client_remove(i, client_map, fd_vec);
          }
          else
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
