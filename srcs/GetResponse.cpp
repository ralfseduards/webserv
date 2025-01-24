#include "../includes/webserv.hpp"

bool check_redirection(Client& client, std::string& request_file, std::string& response) {
  if (client.server->redirection_table.find(request_file) != client.server->redirection_table.end()) {
    response = client.server->redirection_table.at(request_file);
    return (true);
  }
  return (false);
}

bool set_route(Client& client, std::string& request_file) {
  if (client.server->routing_table.find(request_file) != client.server->routing_table.end()) {
    request_file = client.server->routing_table.at(request_file);
    return (true);
  }
  return (false);
}


// 1. checks if request is in redirection table
// 2. checks if request is in routing table
// 3. if not in routing, moves to page directory and searches file in plain
bool get_response(Client& client, std::string& request, std::string& response) {

  std::string request_file(request.begin() + 5, std::find(request.begin() + 5, request.end(), ' '));

  if (check_redirection(client, request_file, response) == true) {
    response_builder(response, 301);
    return (true);
  }

  if (set_route(client, request_file) == false) {
    std::clog << "No route found" << std::endl;
    if (chdir((client.server->root_directory + client.server->page_directory).c_str()) == -1) {
      std::cerr << "Page directory not accessible:" << client.server->page_directory << std::endl;
      return (false);
    }
  }

  struct stat stats;
  stat(request_file.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(request_file.c_str(), R_OK) == -1) {
    response_builder(response, 404);
  }
  else {
    std::string header;
    if (request_file.find(".ico") != std::string::npos || request_file.find(".png") != std::string::npos) {
      generate_header(header, 2001);
    }
    else
      generate_header(header, 200);

    std::ifstream	infile(request_file);
    response = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    response = header + std::to_string(response.size()) + "\r\n\r\n" + response;
  }
  return (false);
}
