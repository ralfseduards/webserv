#include "../includes/webserv.hpp"
#include "../includes/CharArrays.hpp"

void get_response(std::string& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));
  struct stat stats;
  stat(request_file.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(request_file.c_str(), R_OK) == -1) {
    std::ifstream infile(pathNotFound);
    //TODO: remove toString
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = code404 + std::to_string(response.size()) + "\r\n\r\n" + response;
  }
  else {
    std::ifstream	infile(request_file);
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = code200 + std::to_string(response.size()) + "\r\n\r\n" + response;
  }
}
