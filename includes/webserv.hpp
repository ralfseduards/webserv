#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <poll.h>
#include <signal.h>
#include <exception>
#include <filesystem>
#include <map>
#include <sstream>
#include <regex>
#include "Client.hpp"



enum client_status {
  OK,
  DISCONNECTED,
  ERROR,
  RECEIVING,
  HEADER_INVAL_COLON,
  HEADER_INVAL_REGEX_KEY,
  HEADER_INVAL_REGEX_VAL,
  HEADER_INVAL_SIZE,
};


enum port {
  PORT = 8080,
  MAX_CLIENTS = 100,
  BUFFER_SIZE = 8192,
  MAX_REQUEST_SIZE = 10 * BUFFER_SIZE
};

enum messages {
  GET = 0,
  POST = 1,
  DELETE = 2
};

void signal_handler(int sig);
int prepareSocket(int &listening_socket, sockaddr_in &addr, std::vector<pollfd>& fd_vec);

void client_add(int client_fd, std::vector<pollfd>& fd_vec);
void client_add_map( std::map<int, Client>& client_map, int fd);
void client_invalid(std::vector<pollfd>& fd_vec, size_t& i);

int receive_request(pollfd& client_socket, Client& client);
void process_request(Client& client);
void request_error(std::vector<pollfd>& fd_vec,  std::map<int, Client>& client_map, size_t& i, int status);
int read_header(std::string header, Request& new_request);

void get_response(std::string& request, std::string& response);
void post_response(Client& client);
