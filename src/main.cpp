#include <unistd.h>

#include <csignal>
#include <iostream>

#include "../includes/webserv.hpp"

volatile std::sig_atomic_t g_sig = 0;

void signal_handler(int sig) {
    g_sig = sig;
    signal(SIGINT, SIG_DFL);  // Restore default behavior
}

void close_fds(std::vector<pollfd>& fd_vec) {
    std::clog << "Shutting down" << std::endl;
    for (size_t i = 0; i < fd_vec.size(); ++i) {
        shutdown(fd_vec[i].fd, SHUT_RDWR);
        close(fd_vec[i].fd);
    }
}

void client_purge(std::size_t& i, std::vector<pollfd>& fd_vec, std::map<int, Client>& client_map, int status) {
    if (status == HEADER_INVAL_VERSION)
        std::cout << "Header invalid version" << std::endl;
    client_error_message(i, fd_vec[i].fd, status);
    if (status == BODY_TOO_LARGE) {
        Client& client = client_map.at(fd_vec[i].fd);
        Response errorResp;
        errorResp.http_code = 413;
        errorResp.has_content = true;

        // Load error page and format response
        load_http_code_page(client, errorResp);
        http_response(client, errorResp);

        // Send the response
        send_response(client, errorResp, fd_vec);
		// client.status = CLOSE;
		// return;
    }
    if (status != POLLINVALID) {
        shutdown(fd_vec[i].fd, SHUT_RDWR);
        close(fd_vec[i].fd);
    }
    client_remove(i, client_map, fd_vec);
}

int main(int argc, char **argv) {
    signal(SIGINT, signal_handler);

    std::vector<pollfd> fd_vec;
    std::map<int, Server> server_map;
    std::map<int, Client> client_map;

    std::string configPath = (argc == 2) ? argv[1] : "config/default.conf";
    Config config(configPath);
    // config.printConfig();

    g_sig = createServersFromConfig(fd_vec, server_map, config);
    while (true && !g_sig) {  // Main loop

        if (poll(fd_vec.data(), fd_vec.size(), -1) == -1) {
            if (g_sig != 0) continue;  // Interrupted by signal, continue loop
            perror("poll");
            break;
        }

        for (std::size_t i = 0; i < fd_vec.size(); ++i) {
            // SERVER SOCKET HANDLING
            if (i < server_map.size()) {
                // New connection
                if (fd_vec[i].revents & POLLIN) {
                    new_client(fd_vec, server_map, client_map, i);
                }

                // Handle server socket errors if needed
                if (fd_vec[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    std::cerr << "Error on server socket: " << fd_vec[i].fd << std::endl;
                    // Additional server error handling if needed
                }
            }
            // CLIENT SOCKET HANDLING
            else {
                // Invalid POLL
                if (fd_vec[i].revents & POLLNVAL) {
                    client_purge(i, fd_vec, client_map, POLLINVALID);
                    continue;
                }

                // Client error
                if (fd_vec[i].revents & POLLERR) {
                    client_purge(i, fd_vec, client_map, ERRPOLL);
                    continue;
                }

                // Client hung up
                if (fd_vec[i].revents & POLLHUP) {
                    client_purge(i, fd_vec, client_map, HUNGUP);
                    continue;
                }

                // Incoming message
                if (fd_vec[i].revents & POLLIN) {
                    std::cout << "Incoming message" << std::endl;
                    incoming_message(fd_vec[i], client_map.at(fd_vec[i].fd), fd_vec);
                }

                // Client write
                if (fd_vec[i].revents & POLLOUT) {
                    std::cout << "Client write" << std::endl;
                    handle_client_write(i, fd_vec, client_map);
                }

                // Check client status
                if (client_map.at(fd_vec[i].fd).status != OK &&
                    client_map.at(fd_vec[i].fd).status != RECEIVING) {
                    std::cout << "Client status: " << client_map.at(fd_vec[i].fd).status << std::endl;
                    client_purge(i, fd_vec, client_map, client_map.at(fd_vec[i].fd).status);
                }
            }
        }
    }

    close_fds(fd_vec);
    for (std::map<int, Server>::iterator it = server_map.begin(); it != server_map.end(); ++it) {
        for (std::map<std::string, Server>::iterator it2 = it->second.virtual_hosts.begin(); it2 != it->second.virtual_hosts.end(); ++it2) {
            if (it->second == it2->second)
                continue;
            else
                deleteTrie(it2->second.root);
        }
        deleteTrie(it->second.root);
    }

    std::clog << "Server terminated due to signal " << g_sig << std::endl;
    return (0);
}
