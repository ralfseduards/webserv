#pragma once

#include "webserv.hpp"

struct Response
{
  ssize_t http_code;            /* response code */
  std::string code_string;      /* full response (eg. 200 OK) */
  bool has_content;             /* did the server manage to get some file? */
  bool was_redirected;
  bool was_routed;
  bool manual_response;            /* was the response from cgi ? */
  std::string redirection_URL;
  std::string content;
  std::string file_content;      /* the content the file requested by the client */
  std::string content_type;      /* the content type header */
  std::string request_path;      /* the path of the file requested by client */

  Response() : http_code(0), has_content(false), was_redirected(false), was_routed(false), manual_response(false) {}
};
