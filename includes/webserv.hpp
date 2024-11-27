#ifndef _WEBSERV_HPP_
#define _WEBSERV_HPP_

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

int prepareSocket(int &listening_socket, sockaddr_in &addr);
void add_client(int client_fd, std::vector<pollfd>& fd_list);
void build_response(std::vector<char>& request, std::string& response);
void signal_handler(int sig);


#endif
