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
  OK = 0,
  DISCONNECTED = 1,
  ERROR = 2,
  HEADER_INVAL_COLON = 3,
  HEADER_INVAL_REGEX_KEY = 4,
  HEADER_INVAL_REGEX_VAL = 5,
  HEADER_INVAL_SIZE = 6,
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

int prepareSocket(int &listening_socket, sockaddr_in &addr, std::vector<pollfd>& fd_vec);
void add_client(int client_fd, std::vector<pollfd>& fd_vec);
void build_response(std::string& request, std::string& response);
void signal_handler(int sig);
void process_request(Client& client);
int receive_request(pollfd& client_socket, Client& client);
void request_error(std::vector<pollfd>& fd_vec,  std::map<int, Client>& client_map, size_t& i, int status);
void add_client_map( std::map<int, Client>& client_map, int fd);
int parse_header(std::string header, Request& new_request);
