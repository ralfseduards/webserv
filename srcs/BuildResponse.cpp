#include "../includes/webserv.hpp"

void build_response(std::string& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));
  struct stat stats;
  stat(request_file.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(request_file.c_str(), R_OK) == -1) {
    std::ifstream infile("404.html");
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(response.size()) + "\r\n"
               "\r\n"
              + response;
    infile.close();
  }
  else {
    std::ifstream	infile(request_file);
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response =  "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
               "Content-Length: " + std::to_string(response.size()) + "\r\n"
               "\r\n"
               + response;
    infile.close();
  }
}
