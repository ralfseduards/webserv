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
