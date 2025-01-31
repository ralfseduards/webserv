#include "../includes/webserv.hpp"


int getSocket(std::vector<pollfd>& fd_vec, int port) {
  struct sockaddr_in addr;
  int listening_socket;

  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket == -1)
    return (-1);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  int option = 1;
  setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  if (bind(listening_socket, (sockaddr *)&addr, sizeof(addr)) == -1)
    return (-1);

  if (listen(listening_socket, MAX_CLIENTS) < 0)
    return (-1);

  pollfd server_fd;
  server_fd.fd = listening_socket;
  server_fd.events = POLLIN;
  server_fd.revents = 0;
  fd_vec.push_back(server_fd);
  return (listening_socket);
}

int createServers(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map) {

  Server new_server;
  new_server.methods |= (GET | POST | DELETE);
  new_server.ports = {8080};
  new_server.max_body_size = MAX_REQUEST_SIZE;
  new_server.server_socket = getSocket(fd_vec, 8080);
  new_server.server_name = "HARDCODED SERVER";
  new_server.root_directory = "/home/tsurma/Documents/common_core/webserv";
  new_server.page_directory = "www/01-pages";
  new_server.post_directory = "www/02-received";

  new_server.root = new TrieNode();
  insert(new_server.root, "www", (GET));
  insert(new_server.root, "www/01-pages", (GET));
  insert(new_server.root, "www/02-received", (GET | POST | DELETE));
  insert(new_server.root, "www/01-pages/nested", (0));
  insert(new_server.root, "www/03-stash", (GET | POST));

  new_server.routing_table = {
    // {"favicon.ico", "www/01-pages/favicon.ico"},
    {"/",          "www/01-pages/index.html"},
    {"/multipart", "www/01-pages/message.html"}
  };

  new_server.redirection_table = {
    {"/google", "http://google.com"}
  };

  if (new_server.server_socket == -1) {
    std::cerr << "Error in server creation" << std::endl;
    return (-1);
  }

  server_map.emplace(new_server.server_socket, new_server);

  return (OK);
}
