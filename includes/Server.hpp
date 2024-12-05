#pragma once

#include "webserv.hpp"

class Server
{
private:
  /* data */
public:
  Server(/* args */);
  ~Server();

  int listening_socket;
  std::vector<pollfd> fd_vec;

};
