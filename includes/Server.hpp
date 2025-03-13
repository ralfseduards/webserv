#pragma once

#include "TrieNode.hpp"
#include "webserv.hpp"

struct Server
{
  int server_socket;
  std::string       server_name;
  std::vector<int>  ports;
  unsigned char     methods;
  std::size_t       max_body_size;
  TrieNode*         root;
  std::string       root_directory;
  std::string       page_directory;
  std::string       post_directory;
  std::map<std::string, std::string> routing_table;
  std::map<std::string, Server> virtual_hosts;
  std::map<std::string, std::string> redirection_table;
  std::map<int, std::string> errorPages;
  bool 			autoindex;

  /* servers must be the same if they have the same name and are on the same socket */
  bool operator==(const Server& other)
  {
    return ((server_name == other.server_name) && (server_socket == other.server_socket));
  };
};
