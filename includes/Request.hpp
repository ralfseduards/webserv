#pragma once

#include "webserv.hpp"

struct Request {


  std::string start_line;
  std::map<std::string, std::string> header_map;

};


