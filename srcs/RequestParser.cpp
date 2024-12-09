#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"
#include "../includes/Request.hpp"


int receive_request(pollfd& client_socket, Client& client) {

  ssize_t message_size;
  size_t header_length;

  char request_buffer[BUFFER_SIZE + 1];

  memset(request_buffer, 0, BUFFER_SIZE + 1);

  message_size = recv(client_socket.fd, request_buffer, BUFFER_SIZE, 0);

  if (message_size == 0)
    return (DISCONNECTED);
  if (message_size < 0)
    return (ERROR);

  client.request.append(request_buffer);

  header_length = client.request.find("\r\n\r\n");

  if (header_length != std::string::npos) {
    Request new_request;
    int status = parse_header(client.request.substr(0, client.request.find("\r\n\r\n")), new_request);
    if (client.request.length() > header_length + 5)
      client.request.erase(client.request.begin() + header_length + 5);
    client.waitlist.push_back(new_request);
    if (status != 0)
      return (status);
  }

  if (client.request.size() >= MAX_REQUEST_SIZE)
    return (HEADER_INVAL_SIZE);
  return (0);
}

bool validate_header_key(std::string& key) {
  const std::regex key_regex("^[!#$%&'*+.^_`|~0-9a-zA-Z-]+$");
  return (std::regex_match(key, key_regex));
}

bool validate_header_value(std::string& value) {
  const std::regex value_regex("^[\\t\\x20-\\x7E]*$");
  return (std::regex_match(value, value_regex));
}



int parse_header(std::string header, Request& new_request) {

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

    if (validate_header_key(key) == false)
      return (HEADER_INVAL_REGEX_KEY);

    if (validate_header_key(key) == false)
      return (HEADER_INVAL_REGEX_VAL);
  }
  return (0);
}
