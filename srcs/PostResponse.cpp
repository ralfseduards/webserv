#include "../includes/webserv.hpp"
#include "../includes/CharArrays.hpp"


void post_response(Client& client) {

  if (client.request.size() < client.waitlist[0].content_length) {
    client.status = RECEIVING;
    return ;
  }
  client.status = post_request_header_parser(client);
  if (client.status != OK) {
    std::clog << "Client Status: " << client.status << std::endl;
    return;
  }

  // Extract the body from the request string
  client.waitlist[0].body = client.request.substr(0, client.waitlist[0].content_length);
  client.request.erase(0, client.waitlist[0].content_length);
  client.status = OK;

  if (client.waitlist[0].header_map.find("boundary=") != client.waitlist[0].header_map.end()) {
    post_request_part_handler(client.waitlist[0]);
  } else {
    post_request_simple_handler(client.waitlist[0]);
  }

  response_builder(client.waitlist[0].response, 201);
}

int post_request_simple_handler(Request& request) {

  std::ofstream outfile("www/02-received/Outfile");
  outfile << request.body;
  outfile.close();
  return (0);
}

int post_request_part_handler(Request& request) {

  while (request.body != request.header_map.at("boundary=") + "--\r\n") {

    std::size_t begin;
    std::size_t end;
    std::size_t filename_pos;
    std::size_t filename_end;
    std::size_t body_end;
    std::string filename;
    std::string subheader;

    begin = request.body.find(request.header_map.at("boundary=")) + request.header_map.at("boundary=").length();
    end = request.body.find("\r\n\r\n", begin) - 1;

    subheader = request.body.substr(begin, end);
    request.body.erase(request.body.begin(), request.body.begin() + end  + 5);

    filename_pos = subheader.find("filename=") + 9;
    filename_end = subheader.find('"', filename_pos + 1);
    filename = subheader.substr(filename_pos, filename_end - filename_pos);
    filename.erase(std::remove(filename.begin(), filename.end(), '"'), filename.end());

    body_end = request.body.find(request.header_map.at("boundary="));
    std::remove(request.body.begin(), request.body.begin() + body_end, '\r');

    std::ofstream outfile("www/02-received/" + filename, std::ios::out | std::ios::binary);
    outfile << request.body.substr(0, body_end - 2);
    outfile.close();
    request.body.erase(request.body.begin(), request.body.begin() + body_end);
  }
  return (0);
}

int post_request_header_parser(Client& client) {

  if (client.waitlist[0].header_map.count("Content-Type") == 0)
    return (HEADER_INVAL_REGEX_KEY);
  //Checking for multipart and boundary
  if (client.waitlist[0].header_map.at("Content-Type").find("multipart") != std::string::npos) {
      std::size_t boundary_position = client.waitlist[0].header_map.at("Content-Type").find("boundary=");
      if (boundary_position == std::string::npos)
        return (HEADER_INVAL_REGEX_VAL);
      std::string boundary = client.waitlist[0].header_map.at("Content-Type").substr(boundary_position + 9, std::string::npos);
      client.waitlist[0].header_map.emplace("boundary=", "--" + boundary);
  }
  return (OK);
}
