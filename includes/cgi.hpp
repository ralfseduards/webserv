#pragma once
#include "webserv.hpp"

#ifndef CGI_DIR
# define CGI_DIR "cgi-dir"
#endif

#define ENVP_SIZE 10
#define MAX_WAIT 2000000      // 2 seconds

int cgi_error(const char *str);
void create_new_envp(Client& client, char **&custom_envp);
void get_program_name(std::string& result_prog, std::string& result_dir, char **custom_envp, Client& client);
