#pragma once

#include "webserv.hpp"
#include "Request.hpp"


struct Client
{

  int   fd;
  std::string request;

  std::vector<Request> waitlist;

};
