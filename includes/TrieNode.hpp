#pragma once

#include "webserv.hpp"

struct TrieNode {
  std::map<std::string, TrieNode*> children;
  unsigned char permissions;

  TrieNode() : permissions(0) {}

  ~TrieNode() {
    for (std::pair<const std::string, TrieNode *>& pair : children) {
      delete pair.second;  // Recursively delete all children nodes
    }
  }
};
