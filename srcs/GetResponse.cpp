#include "../includes/webserv.hpp"


bool get_response(Client& client, Request& request) {

  std::cout << request.request_path << std::endl;

  struct stat stats;
  stat(request.request_path.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(request.request_path.c_str(), R_OK) == -1) {
    response_builder(client, request.response, 404);
  }
  else {
    std::string header;
    if (request.request_path.find(".ico") != std::string::npos || request.request_path.find(".png") != std::string::npos) {
      generate_header(header, 2001);
    }
    else
      generate_header(header, 200);

    std::ifstream	infile(request.request_path);
    request.response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    request.response = header + std::to_string(request.response.size()) + "\r\n\r\n" + request.response;
  }
  return (false);
}
