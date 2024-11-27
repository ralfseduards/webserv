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

enum port {
  PORT = 8080,
  MAX_CLIENTS = 100
};

int prepareSocket(int *sock, sockaddr_in *addr);


#endif
