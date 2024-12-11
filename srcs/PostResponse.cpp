#include "../includes/webserv.hpp"

void post_response(Client& client) {

  if (client.request.size() < client.waitlist[0].content_length) {
    client.status = RECEIVING;
    return ;
  }

  // Extract the body from the request string
  client.waitlist[0].body = client.request.substr(0, client.waitlist[0].content_length);
  client.request.erase(0, client.waitlist[0].content_length);
  client.status = OK;

  std::size_t filename_position = client.waitlist[0].body.find("filename=");
  if (filename_position != std::string::npos) {
    std::ofstream outfile("Outfile");
    outfile << client.waitlist[0].body.substr(filename_position + 9) << std::endl;
    outfile.close();
    client.waitlist[0].response = "HTTP/1.1 201 Created\r\n"
               "Content-Type: " + client.waitlist[0].header_map["Content-Type"] + "\r\n" +
               "Content-Length: " + std::to_string(client.waitlist[0].content_length) + "\r\n\r\n";
  }
}
