#pragma once

#include "webserv.hpp"

struct Server
{
  int server_socket;
  std::string       server_name;
  std::string       root_directory;
  std::vector<int>  ports;
  unsigned char     methods;
  std::size_t       max_body_size;
  std::map<std::string, std::string> routing_table;
  std::map<std::string, std::string> redirection_table;

};


