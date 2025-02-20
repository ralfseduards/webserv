#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"
#include "../includes/Request.hpp"

/* The orchestrator of receiving and processing a request on the socket. */
int incoming_message(std::vector<pollfd>& fd_vec, std::map<int, Client>& client_map, std::size_t& i)
{
  (void) receive_request(fd_vec[i], client_map[fd_vec[i].fd]); //return value voided for clarity, new method uses state in struct

  if (client_map[fd_vec[i].fd].status != OK && client_map[fd_vec[i].fd].status != RECEIVING)
    return (1);
  else if (client_map[fd_vec[i].fd].waitlist.size() > 0)
    process_request(client_map[fd_vec[i].fd]);
  return (0);
}

/* Reads the request from the socket; populates the Client struct;
 * returns a status code.
 */
int receive_request(pollfd& client_socket, Client& client)
{
  ssize_t bytes_received;
  std::size_t header_length;

  //Create a buffer and set it to 0
  char request_buffer[BUFFER_SIZE + 1];
  memset(request_buffer, 0, BUFFER_SIZE + 1);

  //Read from the socket buffer in request buffer
  bytes_received = recv(client_socket.fd, request_buffer, BUFFER_SIZE, 0);

  if (bytes_received == 0)
  {
    std::clog << "Client Disconnected" << std::endl;
    client.status = DISCONNECTED;
    return (DISCONNECTED);
  }
  else if (bytes_received < 0)
  {
    std::cerr << "bytes received smaller 0" << std::endl;
    client.status = ERROR;
    return (ERROR);
  }
  else
    std::clog << request_buffer << std::endl;

  //Add the request buffer into a string
  client.request.append(request_buffer, bytes_received);

  //If the Client is currently receiving from previous POST, jump there
  if (client.status == RECEIVING)
  {
    if (client.request.size() > MAX_REQUEST_SIZE)
    {
      client.status = BODY_TOO_LARGE;
      return (BODY_TOO_LARGE);
    }
    post_response(client);
    if (client.status == RECEIVING)
      return (OK);
    send(client.fd, client.waitlist[0].response.c_str(), client.waitlist[0].response.length(), 0);
    client.waitlist.erase(client.waitlist.begin());
    client.status = OK;
    return (OK);
  }
  else
  {
    //Check if a full Header is present, and if so push a new request into the queue
    header_length = client.request.find("\r\n\r\n");
    if (header_length != std::string::npos)
    {
      Request new_request;
      new_request.type = 0;
      client.status = read_header(client.request.substr(0, client.request.find("\r\n\r\n")), new_request);
      if (client.status != OK && client.status != RECEIVING)
        return (client.status);
      client.request.erase(0, client.request.find("\r\n\r\n") + 4); // delete the header from the request
      client.waitlist.push_back(new_request);
    }
    // If we got here there was no header present and the client is not actively
    // receiving. If the header becomes too big return error
    if (client.request.size() >= MAX_REQUEST_SIZE)
    {
      std::clog << "client request size larger than max" << std::endl;
      return (HEADER_INVAL_SIZE);
    }
  }
  return (0);
}

/* parses the start_line for the first Request in the waitlist to
 * extract the request method.
*/
void set_type(Client& client)
{
  size_t prefix_len = client.waitlist[0].start_line.find(' ');
  // TODO: error handling

  if (prefix_len == std::string::npos)
  {
    client.waitlist[0].type = INVALID;
    return ;
  }
  std::string temp = client.waitlist[0].start_line.substr(0, prefix_len);
  if (temp == "GET")
    client.waitlist[0].type = GET;
  else if (temp == "POST")
    client.waitlist[0].type = POST;
  else if (temp == "DELETE")
    client.waitlist[0].type = DELETE;
  else if (temp == "HEAD")
    client.waitlist[0].type = HEAD;
  else
  {
    client.waitlist[0].type = INVALID;
    std::clog << "Unexpected request: " << temp << std::endl;
  }
}

int read_header(std::string header, Request& new_request) {

  std::istringstream stream(header);
  std::string line;

  std::getline(stream, new_request.start_line);
  while (std::getline(stream, line)) {

    //Since readline does not include the newline character, pop the remaining
    //Carriage return, then replace any loose carriage returns that may be
    //included in error.
    if (!line.empty() && *(line.end() - 1) == '\r')
      line.pop_back();
    std::replace(line.begin(), line.end(), '\r', ' ');

    if (line.empty())
      break;

    //Find the mandatory colon that seperates key from content
    size_t colon = line.find(':');
    if (colon == std::string::npos) {
      std::clog << "Header colon not found\n" << line << std:: endl;
      return (HEADER_INVAL_COLON);
    }

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");

    value = (start == std::string::npos || end == std::string::npos) ? "" : value.substr(start, end - start + 1);

    //Put the "key" and "value" into the hader map. The key does NOT have the colon
    new_request.header_map.emplace(key, value);

    if (validate_header_key(key) == false) {
      std::clog << "header key validation failed" << std::endl;
      return (HEADER_INVAL_REGEX_KEY);
    }
    if (validate_header_value(value) == false) {
      std::clog << "header value validation failed" << std::endl;
      return (HEADER_INVAL_REGEX_VAL);
    }
  }
  return (0);
}

//TODO: replace with 98 function
bool validate_header_key(std::string& key) {
  const std::regex key_regex("^[!#$%&'*+.^_`|~0-9a-zA-Z-]+$");
  return (std::regex_match(key, key_regex));
}

//TODO: replace with 98 function
bool validate_header_value(std::string& value) {
  const std::regex value_regex("^[\\t\\x20-\\x7E]*$");
  return (std::regex_match(value, value_regex));
}
