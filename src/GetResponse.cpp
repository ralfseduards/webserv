#include "../includes/webserv.hpp"



bool get_response(Client& client, Request& request) {
  struct stat stats;

  // 1) Check if the file actually exists using stat().
  //    If stat() < 0, the file doesn't exist => 404.
  if (stat(request.request_path.c_str(), &stats) < 0) {
      request.response.http_code = 404;
      request.response.has_content = false;
      http_response(client, request.response);
      return false;
  }

  // 2) If it’s a directory, and directory listing is not allowed => 403.
  if (S_ISDIR(stats.st_mode)) {
      request.response.http_code = 403;
      request.response.has_content = false;
      http_response(client, request.response);
      return false;
  }

  // 3) Check read permission. If not readable => 403.
  if (access(request.request_path.c_str(), R_OK) < 0) {
      request.response.http_code = 403;
      request.response.has_content = false;
      http_response(client, request.response);
      return false;
  }

  // 4) Now read and serve the file content normally.
  std::ifstream infile(request.request_path.c_str());
  if (!infile.is_open()) {
      // If we somehow fail to open even though stat/access succeeded => 403 is still appropriate.
      request.response.http_code = 403;
      request.response.has_content = false;
      http_response(client, request.response);
      return false;
  }

  request.response.file_content = std::string(
      (std::istreambuf_iterator<char>(infile)),
       std::istreambuf_iterator<char>());
  infile.close();

  request.response.request_path = request.request_path;
  request.response.has_content  = true;
  request.response.http_code    = 200;
  http_response(client, request.response);

  return false; // false indicates we do not close the connection here
}

