#pragma once

#include "webserv.hpp"
#include "Server.hpp"

#include "Request.hpp"

struct Client
{
  Server* server;
  int fd;
  int status;
  time_t timeout;
  std::string request;            /* stores the whole request */
  std::vector<Request> waitlist;
};
