#include "../includes/webserv.hpp"

int prepareSocket(int *sock, sockaddr_in *addr) {

  *sock = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock == -1)
    return (1);

  addr->sin_family = AF_INET;
  addr->sin_port = htons(PORT);
  addr->sin_addr.s_addr = INADDR_ANY;

  bind(*sock, (sockaddr *)addr, sizeof(*addr));

  return (0);
}
