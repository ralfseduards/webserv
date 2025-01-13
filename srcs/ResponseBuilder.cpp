#include "../includes/webserv.hpp"

void response_builder(std::string& response, int code) {

  std::string path("www/01-pages");
  std::ifstream	infile;
  std::string file;

  switch (code)
  {
  case 201:
    infile.open(path + "/" + "201.html");
    break;

  case 301:
    break;

  case 404:
    infile.open(path + "/" + "404.html");
    break;

  case 403:
    infile.open(path + "/" + "403.html");
    break;

  case 501:
    infile.open(path + "/" + "501.html");
    break;

  default:
    infile.open(path + "/" + "500.html");
    return;
  }

  if (!infile.is_open() || code == 301) {

    response = "HTTP/1.1 301 Moved Permanently\r\nlocation: " + response + "\r\nConnection: close"+ "\r\n\r\n";
    return ;
  }

  file = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
  std::stringstream filesize;
  filesize << file.size();
  generate_header(response, code);
  response.append(filesize.str());
  response.append("\r\n\r\n");
  response.append(file);
}

void generate_header(std::string& header, std::size_t code) {

  header = "HTTP/1.1 ";

  switch (code)
  {
  case 200:
    header.append("200 OK");
    break;

  case 2001:
    header.append("200 OK");
    break;

  case 201:
    header.append("201 Created");
    break;

  case 204:
    header.append("204 No Content");
    break;

  case 301:
    header.append("301 Moved Permanently");
    break;

  case 400:
    header.append("400 Bad Request");
    break;

  case 404:
    header.append("404 Not Found");
    break;

  case 403:
    header.append("403 Forbidden");
    break;

  case 413:
    header.append("413 Content Too Large");
    break;

  case 501:
    header.append("501 Not Implemented");
    break;

  case 505:
    header.append("505 HTTP Version Not Supported");
    break;

  default:
    header.append("500 Internal Server Error");
    std::clog << "Error in header generation unknown case" << std::endl;
    break;
  }
  if (code == 2001)
    header.append("\r\nContent-Type: image/png\r\nContent-Length: ");
  else
    header.append("\r\nContent-Type: text/html\r\nContent-Length: ");
}
