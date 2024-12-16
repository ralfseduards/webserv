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

  if (client.waitlist[0].header_map.find("boundary=") != client.waitlist[0].header_map.end()) {
    post_request_part_handler(client.waitlist[0]);
    return ;
  }


  std::size_t filename_position = client.waitlist[0].body.find("filename=");
  if (filename_position != std::string::npos) {
    try
    {
      std::string filename = client.waitlist[0].body.substr(filename_position + 1, client.waitlist[0].body.find('"', filename_position + 1));
      std::cout << filename << std::endl;
      std::ofstream outfile("www/02-received/Outfile");
      outfile << client.waitlist[0].body.substr(filename_position + 9) << std::endl;
      outfile.close();
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }

    // TODO: replace [] map indexing to prevent emplacing new values
    client.waitlist[0].response = "HTTP/1.1 201 Created\r\n"
               "Content-Type: " + client.waitlist[0].header_map["Content-Type"] + "\r\n" +
               "Content-Length: " + std::to_string(client.waitlist[0].content_length) + "\r\n\r\n";
  }
}

int post_request_part_handler(Request& request) {

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

  std::ofstream outfile("www/02-received/" + filename);
  std::remove(request.body.begin(), request.body.begin() + body_end, '\r');
  outfile << request.body.substr(0, body_end - 4);
  outfile.close();
  request.body.erase(request.body.begin(), request.body.begin() + body_end);
  std::cout << request.body << std::endl;

  return (0);
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
