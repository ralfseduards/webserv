#ifndef PARSEUTILS_HPP
#define PARSEUTILS_HPP

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <set>

std::string removeInlineComments(const std::string& str);
std::string trim(const std::string& str);
size_t parseBodySize(const std::string &str);
std::map<int, std::string> parseErrorPage(const std::string &str);
std::vector<std::string> parseAllowedMethods(const std::string& str);
bool parseAutoIndex(const std::string& str);
bool isValidClosingBlock(const std::string& line);
bool isValidOpeningBlock(const std::string& line, const std::string& directive);
bool isValidOpeningBlockLocation(const std::string &line, const std::string &directive);
std::vector<std::string> parseHost(const std::string& str) ;
std::vector<int> parsePort(const std::string& str);
std::string parseServerName(std::string& str);
std::string parseDirList(const std::string &str);
std::string parseRoot(const std::string& str);
std::string parseIndex(const std::string& str);
bool parseCgi(const std::string &str);
std::map<int, std::string> parseReturnUrl(const std::string &str);
std::string parseCgiPass(const std::string &str);
std::string parseCgiExtension(const std::string& str);


#endif
