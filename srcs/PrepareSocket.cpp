#include "../includes/webserv.hpp"

int prepareSocket(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map) {

  struct sockaddr_in addr;
  int listening_socket;


  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket == -1)
    return (1);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  int option = 1;
  setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  if (bind(listening_socket, (sockaddr *)&addr, sizeof(addr)) == -1)
    return (1);

  if (listen(listening_socket, MAX_CLIENTS) < 0)
    return (1);

  pollfd server_fd;
  server_fd.fd = listening_socket;
  server_fd.events = POLLIN;
  server_fd.revents = 0;
  fd_vec.push_back(server_fd);

  Server new_server;
  new_server.methods = {GET, POST};
  new_server.ports = {PORT};
  new_server.max_body_size = MAX_REQUEST_SIZE;
  new_server.server_socket = listening_socket;
  new_server.server_name = "HARDCODED SERVER";
  new_server.root_directory = "";
  server_map.emplace(listening_socket, new_server);


  return (0);
}
