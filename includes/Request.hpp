#pragma once

#include "webserv.hpp"

struct Request {

  std::string type;
  std::string start_line;
  size_t header_length;
  size_t content_length;
  std::map<std::string, std::string> header_map;
  std::string body;
  std::string response;

};


