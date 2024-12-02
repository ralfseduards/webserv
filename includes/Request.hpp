#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "webserv.hpp"

class Request
{
private:

public:

  Request(/* args */);
  ~Request();

  std::vector<char> raw_string;

  ssize_t type;
  ssize_t message_length;
  std::string path;
  std::string host;
  std::string user_agent;
  std::string connection;
  std::string accept;

};



#endif
