#include "../includes/webserv.hpp"

bool check_redirection(Client& client, std::string& request_file, std::string& response) {
  if (client.server->redirection_table.find(request_file) != client.server->redirection_table.end()) {
    response = client.server->redirection_table.at(request_file);
    return (true);
  }
  return (false);
}

bool is_file_path(Request& request) {
  std::size_t slash = request.request_path.find_last_of("/");
  if (slash == 0 || slash == std::string::npos) {
    return (false);
  }
  request.request_path.erase(request.request_path.begin());
  return (true);
}


bool set_route(Client& client, std::string& request_file) {
  if (client.server->routing_table.find(request_file) != client.server->routing_table.end()) {
    request_file = client.server->routing_table.at(request_file);
    client.waitlist[0].was_routed = true;
    return (true);
  }
  client.waitlist[0].was_routed = false;
  return (false);
}

int check_method_server(Client& client) {
  if ((client.server->methods & client.waitlist[0].type) == 0)
    return (-1);
  return (OK);
}

// int check_method_route(Client& client) {

//   if (client.server->directories.count(client.waitlist[0].request_path))
// }

int set_root_dir(Client& client) {
  return (chdir(client.server->root_directory.c_str()));
}

void process_request(Client& client) {

  if (check_method_server(client) == -1) {
    std::clog << "Not implemented method: " << client.waitlist[0].type << std::endl;
    client.waitlist[0].type = INVALID;
  }

  if (set_root_dir(client) == -1) {
    std::clog << "Couldn't set root directory" << std::endl;
    client.waitlist[0].type = INVALID;
  }

  if (check_redirection(client, client.waitlist[0].request_path, client.waitlist[0].response) == true) {
    response_builder(client, client.waitlist[0].response, 301);
    return ;
  }

  std::cout << client.waitlist[0].request_path << std::endl;

  // Replaces the request path with the one in the routing table
  if (set_route(client, client.waitlist[0].request_path) == false) {
    client.waitlist[0].is_file_path = is_file_path(client.waitlist[0]);
  }

  switch (client.waitlist[0].type)
  {
  case GET:
    if (client.waitlist[0].was_routed == false && client.waitlist[0].is_file_path == false) {
      client.waitlist[0].request_path = client.server->page_directory + client.waitlist[0].request_path;
    }
    if (get_response(client, client.waitlist[0]) == true)
      client.status = CLOSE;
    break;

  case POST:
    if (client.waitlist[0].is_file_path == false) {
      client.waitlist[0].request_path = client.server->post_directory + client.waitlist[0].request_path;
    }
    //TODO: remove stoi
    client.waitlist[0].content_length = std::stoi(client.waitlist[0].header_map.at("Content-Length"));
    if (client.waitlist[0].content_length > client.server->max_body_size) {
      client.status = BODY_TOO_LARGE;
      return ;
    }
    post_response(client);
    if (client.status == RECEIVING)
      return ;
    break;

  case DELETE:
    if (client.waitlist[0].is_file_path == false) {
      client.waitlist[0].request_path = client.server->post_directory + client.waitlist[0].request_path;
    }
    delete_response(client);
    break;

  case HEAD:
    break;

  case INVALID:
    response_builder(client, client.waitlist[0].response, 501);
    break;

  default:
    response_builder(client, client.waitlist[0].response, 501);
    break;
  }

  std::clog << "///////////////////////////////\n" << "client.fd: " << client.fd << "\nResponse:\n" << client.waitlist[0].response << std::endl;
  //send the response and delete all temp data
  send(client.fd, client.waitlist[0].response.c_str(), client.waitlist[0].response.length(), 0);
  // if (client.waitlist[0].response.find("png") == std::string::npos)
  client.waitlist.erase(client.waitlist.begin());
}
