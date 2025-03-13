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
  std::string request;
  std::vector<Request> waitlist;
  std::string output_buffer;
  bool ready_to_write;

  Client(): fd(-1),status(0),timeout(0),ready_to_write(false) {};
};
