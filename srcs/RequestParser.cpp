#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"
#include "../includes/Request.hpp"


int receive_request(pollfd& client_socket, Client& client) {

  ssize_t message_size;
  size_t header_length;

  //Create a buffer and set it to 0
  char request_buffer[BUFFER_SIZE + 1];
  memset(request_buffer, 0, BUFFER_SIZE + 1);

  //Read from the socket buffer in request buffer
  message_size = recv(client_socket.fd, request_buffer, BUFFER_SIZE, 0);

  if (message_size == 0) {
    std::clog << '0' << std::endl;
    client.status = DISCONNECTED;
    return (DISCONNECTED);
  }
  else if (message_size < 0) {
    client.status = ERROR;
    return (ERROR);
  }
  else
    std::clog << request_buffer << std::endl;

  //Add the request buffer into a string
  client.request.append(request_buffer);

  //If the Client is currently receiving from previous POST, jump there
  if (client.status == RECEIVING) {
    post_response(client);
  }

  //Check if a full Header is present, and if so push a new request into the queue
  header_length = client.request.find("\r\n\r\n");
  if (header_length != std::string::npos) {
    Request new_request;
    client.status = read_header(client.request.substr(0, client.request.find("\r\n\r\n")), new_request);
    if (client.status != OK && client.status != RECEIVING)
      return (client.status);
    client.request.erase(0, client.request.find("\r\n\r\n") + 4);
    client.waitlist.push_back(new_request);
  }
  // If we got here there ws no header present and the client is not actively
  // receiving. If the header becomes too big return error
  if (client.request.size() >= MAX_REQUEST_SIZE)
    return (HEADER_INVAL_SIZE);

  return (0);
}

void process_request(Client& client) {

  //Search the first line for the type of request
  size_t prefix_len = client.waitlist[0].start_line.find(' ');
  //TODO: error handling
  if (prefix_len == std::string::npos)
    return ;
  client.waitlist[0].type = client.waitlist[0].start_line.substr(0, prefix_len);

  //Select type of response
  if (client.waitlist[0].type == "GET")
    get_response(client.waitlist[0].start_line, client.waitlist[0].response);
  else if (client.waitlist[0].type == "POST") {
    //TODO: remove stoi
    client.waitlist[0].content_length = std::stoi(client.waitlist[0].header_map["Content-Length"]);
    post_response(client);
  } else if (client.waitlist[0].type == "DELETE") {
    ;
  } else if (client.waitlist[0].type == "HEAD") {
    ;
  }
  else {
    //TODO: Error handling
    //TODO: send error 501
    std::cout << "Invalid Request" << std::endl;
    return ;
  }

  //send the response and delete all temp data
  send(client.fd, client.waitlist[0].response.c_str(), client.waitlist[0].response.length(), 0);
  client.waitlist.erase(client.waitlist.begin());
}

bool validate_header_key(std::string& key) {
  const std::regex key_regex("^[!#$%&'*+.^_`|~0-9a-zA-Z-]+$");
  return (std::regex_match(key, key_regex));
}

bool validate_header_value(std::string& value) {
  const std::regex value_regex("^[\\t\\x20-\\x7E]*$");
  return (std::regex_match(value, value_regex));
}

int read_header(std::string header, Request& new_request) {

  std::istringstream stream(header);
  std::string line;

  std::getline(stream, new_request.start_line);
  while (std::getline(stream, line)) {

    if (!line.empty() && *(line.end() - 1) == '\r')
      line.pop_back();

    std::replace(line.begin(), line.end(), '\r', ' ');

    size_t colon = line.find(':');
    if (colon == std::string::npos)
      return (HEADER_INVAL_COLON);

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");

    value = (start == std::string::npos || end == std::string::npos) ? "" : value.substr(start, end - start + 1);

    new_request.header_map.emplace(key, value);

    if (validate_header_key(key) == false)
      return (HEADER_INVAL_REGEX_KEY);

    if (validate_header_key(key) == false)
      return (HEADER_INVAL_REGEX_VAL);
  }
  //TODO: MULTIPART CHECK
  return (0);
}
