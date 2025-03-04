#pragma once

#include "webserv.hpp"
#include "Server.hpp"

#include "Request.hpp"

struct Server;

struct Client
{
  Server* server;
  int fd;
  int status;
  time_t timeout;
  std::string request;            /* stores the just he body of the request */
  std::vector<Request> waitlist;
};
