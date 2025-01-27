#include "../includes/webserv.hpp"

void delete_response(Client& client) {

  std::string request_file(client.waitlist[0].start_line.begin() + 8, std::find(client.waitlist[0].start_line.begin() + 8, client.waitlist[0].start_line.end(), ' '));
  std::ifstream file(request_file);

  std::cout << request_file << std::endl;

  if (!file) {
    std::cout << "File not found" << std::endl;
    response_builder(client, client.waitlist[0].response, 204);
    return ;
  }

  if (std::remove(request_file.c_str()) == 0) {
    response_builder(client, client.waitlist[0].response, 200);
  } else {
      //TODO: add http response
      std::perror("Error deleting file");
  }
  return ;
}
