#pragma once

#include "webserv.hpp"
#include "Response.hpp"

struct Request {

  unsigned char type;                             /* request type - GET, POST ... */
  bool was_routed;
  bool is_file_path;
  std::size_t header_length;
  std::size_t content_length;
  std::string start_line;                         /* stores the request line */
  std::string body;                               /* used in post_response() */
  std::string request_path;                       /* the path of the file from the request line */
  Response response;                              /* request response made by the server */
  std::map<std::string, std::string> header_map;  /* request headers stored in a map */
};
