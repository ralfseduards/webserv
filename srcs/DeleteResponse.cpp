#include "../includes/webserv.hpp"
#include "../includes/CharArrays.hpp"

void delete_response(Client& client) {

  std::string request_file(client.waitlist[0].start_line.begin() + 5, std::find(client.waitlist[0].start_line.begin() + 5, client.waitlist[0].start_line.end(), ' '));
  std::ifstream file(request_file);

  if (!file) {
    client.waitlist[0].response = code204;
    return ;
  }

  if (std::remove(request_file.c_str()) == 0) {
    client.waitlist[0].response = code200;
  } else {
      //TODO: add http response
      std::perror("Error deleting file");
  }
  return ;
}
