#include "../includes/webserv.hpp"
#include "../includes/Client.hpp"
#include "../includes/Request.hpp"

bool search_header(Client& client) {
  std::size_t header_length;
  header_length = client.request.find("\r\n\r\n");
  if (header_length == std::string::npos)
    return (false);
  return (true);
}

bool new_request(Client& client, std::vector<pollfd>& fd_vec) {
    Request new_req;
    new_req.type = 0;
    new_req.was_routed = false;
    new_req.is_file_path = false;

    client.status = parse_header(
        client.request.substr(0, client.request.find("\r\n\r\n")),
        new_req
    );
    if (client.status != OK && client.status != RECEIVING) {

        if (client.status == HEADER_INVAL_COLON ||
            client.status == HEADER_INVAL_REGEX_KEY ||
            client.status == HEADER_INVAL_REGEX_VAL ||
            client.status == HEADER_INVAL_VERSION ||
			client.status == HEADER_INVAL_CONTENT_LENGTH ||
			client.status == HEADER_INVAL_DUPLICATE ||
            client.status == HEADER_INVAL_SIZE ){
            new_req.response.http_code = 400;
            new_req.response.has_content = false;
            http_response(client, new_req.response);
            queue_for_sending(client, new_req.response.content, fd_vec);
            client.status = CLOSE;
        }
        return false;
    }
    // If header parse was OK or we are still receiving (chunked POST?), proceed
    // Erase the parsed header from the raw request buffer
    client.request.erase(0, client.request.find("\r\n\r\n") + 4);
    client.waitlist.push_back(new_req);
    return true;
}


static int receive_request(pollfd& client_socket, Client& client)
{
    ssize_t bytes_received;
    // Create a buffer and set it to 0
    char request_buffer[BUFFER_SIZE + 1];
    memset(request_buffer, 0, BUFFER_SIZE + 1);
    
    // Read from the socket buffer in request buffer
    bytes_received = recv(client_socket.fd, request_buffer, BUFFER_SIZE, 0);
    
    if (bytes_received == 0) {
        std::clog << "Client Disconnected" << std::endl;
        client.status = DISCONNECTED;
        return (DISCONNECTED);
    }
    else if (bytes_received < 0) {
        std::cerr << "bytes received smaller 0" << std::endl;
        client.status = ERROR;
        return (ERROR);
    }
    std::clog << "Received " << bytes_received << " bytes" << std::endl;
    
    // add the request buffer into a string
    client.request.append(request_buffer, bytes_received);
    std::cout << "Total client request size: " << client.request.size() << " bytes" << std::endl;
    
    return (OK);
}

int incoming_message(pollfd& client_socket, Client& client, std::vector<pollfd>& fd_vec) {

  int result = receive_request(client_socket, client);
  if (result != OK) {
    return result;
  }

  //If the Client is currently receiving from previous POST, jump there
  if (client.status == RECEIVING) {
      if (client.request.size() > MAX_REQUEST_SIZE) {
        client.status = BODY_TOO_LARGE;
        return (1);
      }
    post_response(client);
    if (client.status == RECEIVING)
      return (OK);
    queue_for_sending(client, client.waitlist[0].response.content, fd_vec);
    client.status = OK;
    return (OK);
  }

  if (search_header(client) == true) {
    if (new_request(client, fd_vec) == false)
      return (1);
  }
  else if (client.request.size() >= MAX_REQUEST_SIZE) {
    std::clog << "client request size larger than max" << std::endl;
    client.status = HEADER_INVAL_SIZE;
  }
  std::cout	<< "Client request: " << client.request << std::endl;
  if (client.status != OK && client.status != RECEIVING)
    return (1);

  if (client.waitlist.size() > 0)
    process_request(client, fd_vec);
  std::cout << "Client status: " << client.status << std::endl;
  return (OK);
}

