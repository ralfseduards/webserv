#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

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
#include "Client.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "TrieNode.hpp"
#include "Config.hpp"



enum client_status {
  OK,
  RECEIVING,
  DISCONNECTED,
  HUNGUP,
  CLOSE,
  POLLINVALID,
  ERRPOLL,
  ERROR,
  HEADER_INVAL_COLON,
  HEADER_INVAL_REGEX_KEY,
  HEADER_INVAL_REGEX_VAL,
  HEADER_INVAL_SIZE,
  HEADER_INVAL_VERSION,
  HEADER_INVAL_CONTENT_LENGTH,
  HEADER_INVAL_DUPLICATE,
  BODY_TOO_LARGE,
  BAD_METHOD
};

enum methods {
  GET     = 1,
  POST    = 2,
  DELETE  = 4,
  HEAD    = 8,
  INVALID = 100,
  NOT_ALLOWED = 101

};

enum port {
  MAX_CLIENTS = 100,
  BUFFER_SIZE = 8000,
  MAX_REQUEST_SIZE = 10000 * 8000
};

#define http_version "HTTP/1.1"
#define LINE_DELIMITER "\r\n"


void signal_handler(int sig);
void close_fds(std::vector<pollfd>& fd_vec);
void printServer(const Server& srv);
int getSocket(std::vector<pollfd>& fd_vec, int port);
int createServersFromConfig(std::vector<pollfd>& fd_vec,
	std::map<int, Server>& server_map,
	const Config& config);


int getSocket(std::vector<pollfd>& fd_vec, int port);
int createServers(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map);



// int prepareSocket(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map);

int new_client(std::vector<pollfd>& fd_vec, std::map<int, Server>& server_map, std::map<int, Client>& client_map, std::size_t& i);
void client_error_message(size_t i, int fd, int status);
void client_remove(size_t& i, std::map<int, Client>& client_map, std::vector<pollfd>& fd_vec);

int incoming_message(pollfd& pollClient, Client& client);

void process_request(Client& client);
int parse_header(std::string header, Request& new_request);
bool validate_header_key(std::string& key);
bool validate_header_value(std::string& value);
int set_request_path(Request& request);
void set_type(Request& request);
bool get_response(Client& client, Request& request);
void delete_response(Client& client);
void post_response(Client& client);
void http_response(Client& client, Response& response);
void redirection_response (Response& response);
void content_response(Response& response);
std::string return_http_code(int code);
void send_response(Client& client, Response& response);
void load_http_code_page(Client& client, Response& response);
void insert(TrieNode* root, const std::string& path, unsigned char permissions);
void deleteTrie(TrieNode* root);
TrieNode* findBestMatch(TrieNode* root, const std::string& filepath);

void list_files(const char* rootPath);
int cgi_parse(Client& client);

void list_directory(Client& client);


#include <sstream>
#include <string>

template <typename T>
std::string to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
