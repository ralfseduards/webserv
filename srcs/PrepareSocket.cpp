#include "../includes/webserv.hpp"

int prepareSocket(int &listening_socket, sockaddr_in &addr) {

  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket == -1)
    return (1);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(listening_socket, (sockaddr *)&addr, sizeof(addr)) == -1)
    return (1);

  if (listen(listening_socket, MAX_CLIENTS) < 0)
    return (1);

  return (0);
}
