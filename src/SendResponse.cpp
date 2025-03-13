#include "../includes/webserv.hpp"

void queue_for_sending(Client& client, const std::string& data, std::vector<pollfd>& fd_vec) {
    client.output_buffer += data;
    client.ready_to_write = true;
    // Find client's pollfd and enable POLLOUT
    for (size_t i = 0; i < fd_vec.size(); i++) {
        if (fd_vec[i].fd == client.fd) {
            fd_vec[i].events |= POLLOUT;
            break;
        }
    }
}

void send_response(Client& client, Response& response, std::vector<pollfd>& fd_vec) {
    std::clog << "---=========================" << "RESPONSE FOR CLIENT " << client.fd << "=========================---\n";
	std::clog << response.content << std::endl;
    std::clog <<  " ---===========================================================================---" << std::endl;

    queue_for_sending(client, response.content, fd_vec);
}

void handle_client_write(size_t i, std::vector<pollfd>& fd_vec, std::map<int, Client>& client_map) {
	std::clog << "Handling client write" << std::endl;
    Client& client = client_map.at(fd_vec[i].fd);
    if (client.output_buffer.empty()) {
        // No data to send, disable POLLOUT
        fd_vec[i].events = POLLIN;
        client.ready_to_write = false;
        return;
    }
    // Try to send data
    ssize_t sent = send(fd_vec[i].fd, client.output_buffer.c_str(), client.output_buffer.size(), 0);
    if (sent > 0) {
        // Remove sent data from buffer
        client.output_buffer.erase(0, sent);
        // If all data sent, disable POLLOUT and clean up waitlist if needed
        if (client.output_buffer.empty()) {
            fd_vec[i].events = POLLIN;
            client.ready_to_write = false;
            // If we've sent a complete response for the first request, remove it
            if (!client.waitlist.empty()) {
                client.waitlist.erase(client.waitlist.begin());
            }
            // If status was set to CLOSE, close connection after sending
            if (client.status == CLOSE) {
                client.status = DISCONNECTED;
            }
        }
    } else if (sent == 0) {
        std::clog << "Client disconnected !!!" << std::endl;
        client.status = DISCONNECTED;
    } else {
		std::cerr << "Error sending data to client" << std::endl;
        client.status = ERROR;
    }
}
