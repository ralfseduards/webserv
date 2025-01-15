#include "../includes/webserv.hpp"

bool get_response(std::string& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));
  if (request_file.empty()) {
    request_file = "www/01-pages/index.html";
  }
  else if (request_file == "favicon.ico") {
    request_file = "www/01-pages/favicon.ico";
  }
  // if (check_redirection(request_file) == true) {
  //   response = "http://127.0.0.1:8080/index.html";
  //   response_builder(response, 301);
  //   return true;
  // }

  struct stat stats;
  stat(request_file.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(request_file.c_str(), R_OK) == -1) {
    response_builder(response, 404);
  }
  else {

    std::string header;
    if (request_file == "www/01-pages/favicon.ico") {
      generate_header(header, 2001);
    }
    else
      generate_header(header, 200);

    std::cout << request_file << std::endl;
    std::ifstream	infile(request_file);
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = header + std::to_string(response.size()) + "\r\n\r\n" + response;
  }
  return (false);
}

bool check_redirection(std::string& request_file) {
  if (request_file.empty())
    return (true);
  return (false);
}
