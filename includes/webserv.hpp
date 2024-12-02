#ifndef WEBSERV_HPP_
#define WEBSERV_HPP_

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

enum port {
  PORT = 8080,
  MAX_CLIENTS = 100
};

enum messages {
  GET = 0,
  POST = 1,
  DELETE = 2
};

int prepareSocket(int &listening_socket, sockaddr_in &addr, std::vector<pollfd>& fd_vec);
void add_client(int client_fd, std::vector<pollfd>& fd_vec);
void build_response(std::vector<char>& request, std::string& response);
void signal_handler(int sig);
void process_request(std::vector<pollfd>& fd_vec, size_t& i);

#endif
