#ifndef SERVER_HPP_
#define SERVER_HPP_

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



#endif
