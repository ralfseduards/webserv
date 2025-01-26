#include "../includes/webserv.hpp"

#ifndef CGI_DIR
# define CGI_DIR cgi-dir
#endif

// default cgi directory: cgi-dir
// simple script : ../cgi-dir/simple.py

// TODO: make it configurable in the parser

// TODO: implement python extension
// how to check for interpreters?
//  1. check with the file extension
//  2. if no file extension just run and hope for a shebang
//  -- maybe check mime types ??

// TODO: POST requests
// TODO: GET requests


/* --===== the post request =====-- */

// package the contents of the request in a format:
// fieldname=value&fieldname1=value, like a QUERY_STRING

// i need the request.

// GET /cgi-bin/simple HTTP/1.1
// Host: localhost:8080
// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br, zstd
// Sec-GPC: 1
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// Priority: u=0, i

int cgi_parse(void)
{
  std::string request;


  return (0);
}


