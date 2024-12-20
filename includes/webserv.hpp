#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <signal.h>
#include <exception>
#include <filesystem>
#include <map>
#include <regex>
#include "Client.hpp"
#include "Server.hpp"


enum client_status {
  OK,
  RECEIVING,
  DISCONNECTED,
  HUNGUP,
  POLLINVALID,
  ERROR,
  HEADER_INVAL_COLON,
  HEADER_INVAL_REGEX_KEY,
  HEADER_INVAL_REGEX_VAL,
  HEADER_INVAL_SIZE,
  BODY_TOO_LARGE
};


enum port {
  PORT = 8080,
  MAX_CLIENTS = 100,
  BUFFER_SIZE = 8000,
  MAX_REQUEST_SIZE = 10 * 8000,
  SERVER_NUMBER = 1
};

enum messages {
  GET,
  POST,
  DELETE,
  HEAD,
  INVALID
};

void signal_handler(int sig);
int prepareSocket(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map);

void client_add_vec(int client_fd, std::vector<pollfd>& fd_vec);
void client_add_map(std::map<int, Client>& client_map, int fd, Server* server);
void client_error(size_t i, int fd, int status);
void client_remove(size_t& i, std::map<int, Client>& client_map, std::vector<pollfd>& fd_vec);

int receive_request(pollfd& client_socket, Client& client);
void process_request(Client& client);
int read_header(std::string header, Request& new_request);
bool validate_header_key(std::string& key);
bool validate_header_value(std::string& value);

void get_response(std::string& request, std::string& response);
void post_response(Client& client);
int post_request_header_parser(Client& client);
int post_request_part_handler(Request& request);
int post_request_simple_handler(Request& request);
void delete_response(Client& client);
