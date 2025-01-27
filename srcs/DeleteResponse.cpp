#include "../includes/webserv.hpp"

void delete_response(Client& client) {

  std::cout << client.waitlist[0].request_path << std::endl;
  std::ifstream file(client.waitlist[0].request_path);


  if (!file) {
    std::cout << "File not found" << std::endl;
    response_builder(client, client.waitlist[0].response, 204);
    return ;
  }

  if (std::remove(client.waitlist[0].request_path.c_str()) == 0) {
    response_builder(client, client.waitlist[0].response, 200);
  } else {
      //TODO: add http response
      std::perror("Error deleting file");
  }
  return ;
}
