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
    generate_header(header, 200, request.request_path);


    std::ifstream	infile(request.request_path);
    request.response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    request.response = header + std::to_string(request.response.size()) + "\r\n\r\n" + request.response;
  }
  return (false);
}
