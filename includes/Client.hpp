#pragma once

#include "webserv.hpp"
#include "Request.hpp"


struct Client
{

  int   fd;
  int status;
  time_t timeout;
  std::string request;
  std::vector<Request> waitlist;

};
