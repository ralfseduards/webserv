#include "../includes/webserv.hpp"

/* Checks if the method requested is supported by the server. Uses a bitwise "and".
 * Returns -1 if method not supported.
 */
int check_method_server(Client& client)
{
  if ((client.server->methods & client.waitlist[0].type) == 0)
    return (-1);
  return (OK);
}

// int check_method_route() {

// }

int set_root_dir(Client& client) {
  return (chdir(client.server->root_directory.c_str()));
}

void process_request(Client& client)
{
  /* set the request method */
  set_type(client);

  if (check_method_server(client) == -1)
  {
    std::clog << "Not implemented method: " << client.waitlist[0].type << std::endl;
    client.waitlist[0].type = INVALID;
  }

  if (set_root_dir(client) == -1)
  {
    std::clog << "Couldn't set root directory" << std::endl;
    client.waitlist[0].type = INVALID;
  }

  // TODO: add cgi here
  // if (is_cgi(client) == 1)

  switch (client.waitlist[0].type)
  {
  case GET:
    if (get_response(client, client.waitlist[0].start_line, client.waitlist[0].response) == true)
      client.status = CLOSE;
    break;

  case POST:
    // TODO: remove stoi
    client.waitlist[0].content_length = std::stoi(client.waitlist[0].header_map.at("Content-Length"));
    post_response(client);
    if (client.status == RECEIVING)
      return ;
    break;

  case DELETE:
    // delete_response(client);
    break;

  case HEAD:
    break;

  case INVALID:
    response_builder(client.waitlist[0].response, 501);
    break;

  default:
    response_builder(client.waitlist[0].response, 501);
    break;
  }

  // TODO: for testing ?
  std::clog << "///////////////////////////////\n" << "client.fd: " << client.fd << "\nResponse:\n" << client.waitlist[0].response << std::endl;

  /* send the response and delete all temp data */

  send(client.fd, client.waitlist[0].response.c_str(), client.waitlist[0].response.length(), 0);
  // if (client.waitlist[0].response.find("png") == std::string::npos)
  client.waitlist.erase(client.waitlist.begin());
}
