#include "../includes/webserv.hpp"

#ifndef CGI_DIR
# define CGI_DIR "cgi-dir"
#endif

// default cgi directory: cgi-dir
// simple script : ../cgi-dir/simple.py

// TODO: make it configurable in the parser

//  >>>>>>>>>>>>>>>> the cgi logic: <<<<<<<<<<<<<<<<
// 1. construct the environment
//    1. copy the real one
//    2. add the http header parsed as environ variables
//    3. add the required ones by the standard
// 2. pipe, fork, execve
//    1. write into stdin the message body of the http request
//    2. stdout is the response from the script
// 3. wait (maybe some no-hang/timeout logic)
// 4. parse the stdout of the script and make a http response


// TODO: POST requests
// TODO: GET requests


/* --===== the post request =====-- */

// package the contents of the request in a format:
// fieldname=value&fieldname1=value, like a QUERY_STRING

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

int cgi_parse(Client& client)
{

  // chdir to cgi-bin for relative scripts 


  return (0);
}


