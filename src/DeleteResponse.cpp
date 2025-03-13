#include "../includes/webserv.hpp"

void delete_response(Client& client) {

  std::cout << client.waitlist[0].request_path << std::endl;
  std::ifstream file(client.waitlist[0].request_path.c_str());
  if (access(client.waitlist[0].request_path.c_str(), F_OK) != 0) {
    std::cout << "File not found" << std::endl;
    client.waitlist[0].response.http_code = 404;
    client.waitlist[0].response.has_content = false;
    http_response(client, client.waitlist[0].response);
    return ;
  }
  if (access(client.waitlist[0].request_path.c_str(), W_OK) != 0) {
    std::cout << "Permission denied" << std::endl;
    client.waitlist[0].response.http_code = 403;
    client.waitlist[0].response.has_content = false;
    http_response(client, client.waitlist[0].response);
    return ;
  }

  if (::remove(client.waitlist[0].request_path.c_str()) == 0) {
    client.waitlist[0].response.http_code = 200;
    client.waitlist[0].response.has_content = false;
    http_response(client, client.waitlist[0].response);
  } else {
    std::perror("Error deleting file");
    client.waitlist[0].response.http_code = 500;
    client.waitlist[0].response.has_content = false;
    http_response(client, client.waitlist[0].response);
  }
}
