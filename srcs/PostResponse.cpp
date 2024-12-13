#include "../includes/webserv.hpp"

void post_response(Client& client) {

  if (client.request.size() < client.waitlist[0].content_length) {
    client.status = RECEIVING;
    return ;
  }

  client.status = post_request_header_parser(client);
  if (client.status != OK)
    return;

  // Extract the body from the request string
  client.waitlist[0].body = client.request.substr(0, client.waitlist[0].content_length);
  client.request.erase(0, client.waitlist[0].content_length);
  client.status = OK;

  // TODO: split multiparts into seperate streams on boundary

  // if (client.waitlist[0].header_map.find("boundary=") != client.waitlist[0].header_map.end()) {

  // }

  std::size_t filename_position = client.waitlist[0].body.find("filename=");
  if (filename_position != std::string::npos) {
    std::ofstream outfile("Outfile");
    outfile << client.waitlist[0].body.substr(filename_position + 9) << std::endl;
    outfile.close();
    // TODO: replace [] map indexing to prevent emplacing new values
    client.waitlist[0].response = "HTTP/1.1 201 Created\r\n"
               "Content-Type: " + client.waitlist[0].header_map["Content-Type"] + "\r\n" +
               "Content-Length: " + std::to_string(client.waitlist[0].content_length) + "\r\n\r\n";
  }
}

int post_request_header_parser(Client& client) {

  if (client.waitlist[0].header_map.count("Content-Type") == 0)
    return (HEADER_INVAL_REGEX_KEY);
  //Checking for multipart and boundary
  // TODO: replace [] map indexing to prevent emplacing new values
  if (client.waitlist[0].header_map["Content-Type"].find("multipart") != std::string::npos) {
      std::size_t boundary_position = client.waitlist[0].header_map["Content-Type"].find("boundary=");
      if (boundary_position == std::string::npos)
        return (HEADER_INVAL_REGEX_VAL);
      std::string boundary = client.waitlist[0].header_map["Content-Type"].substr(boundary_position + 9, std::string::npos);
      client.waitlist[0].header_map.emplace("boundary=", boundary);
  }
  return (OK);
}
