#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <iostream>

#include "../includes/webserv.hpp"

volatile std::sig_atomic_t g_sig = 0;

void signal_handler(int sig) {
  g_sig = sig;
  signal(SIGINT, SIG_DFL);  // Restore default behavior
}

void close_fds(std::vector<pollfd>& fd_vec) {
  std::clog << "Shutting down" << std::endl;
  for (size_t i = 0; i < fd_vec.size(); ++i) {
    shutdown(fd_vec[i].fd, SHUT_RDWR);
    close(fd_vec[i].fd);
  }
}

void client_purge(std::size_t& i, std::vector<pollfd>& fd_vec, std::map<int, Client>& client_map, int status) {
  client_error_message(i, fd_vec[i].fd, status);
  if (status != POLLINVALID) {
    shutdown(fd_vec[i].fd, SHUT_RDWR);
    close(fd_vec[i].fd);
  }
  client_remove(i, client_map, fd_vec);
}


int main(void) {
  signal(SIGINT, signal_handler);

  std::vector<pollfd> fd_vec;        // A vec of all pollfds, Servers at front
  std::map<int, Server> server_map;  // A map of the server data keyed to the fd
  std::map<int, Client> client_map;  // A map of the client data keyed to the fd

  // TODO: Parse config file and populate servers
  g_sig = createServers(fd_vec, server_map);

  while (true && !g_sig) {  // Main loop

    if (poll(fd_vec.data(), fd_vec.size(), -1) == -1) {
      if (g_sig != 0) continue;  // Interrupted by signal, continue loop
      perror("poll");
      break;
    }

    for (std::size_t i = 0; i < fd_vec.size(); ++i) {  // Loop through FDs

      // Invalid POLL
      if (fd_vec[i].revents & POLLNVAL) {
        client_purge(i, fd_vec, client_map, POLLINVALID);
        continue;
      }

      // Client error
      if (fd_vec[i].revents & (POLLERR)) {
        client_purge(i, fd_vec, client_map, ERRPOLL);
        continue;
      }

      // Client hung up
      if (fd_vec[i].revents & (POLLHUP)) {
        client_purge(i, fd_vec, client_map, HUNGUP);
        continue;
      }

      // New connection
      if ((fd_vec[i].revents & POLLIN) && (i < server_map.size())) {
        new_client(fd_vec, server_map, client_map, i);
      }

      // Message
      if (fd_vec[i].revents & POLLIN && i >= server_map.size()) {
        incoming_message(fd_vec[i], client_map.at(fd_vec[i].fd));

        if (client_map.at(fd_vec[i].fd).status != OK && client_map.at(fd_vec[i].fd).status != RECEIVING) {  // Check client status
          client_purge(i, fd_vec, client_map, client_map.at(fd_vec[i].fd).status);
        }
      }
    }
  }

  close_fds(fd_vec);
  std::clog << "Server terminated due to signal " << g_sig << std::endl;
  return (0);
}
