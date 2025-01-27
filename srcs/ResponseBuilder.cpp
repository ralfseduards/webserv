#include "../includes/webserv.hpp"

void response_builder(Client& client, std::string& response, int code) {

  if (chdir((client.server->root_directory + client.server->page_directory).c_str()) == -1) {
    std::cerr << "Page directory not accessible:" << client.server->page_directory << std::endl;
    client.status = ERROR;
    return ;
  }
  std::ifstream	infile;
  std::string file;

  switch (code)
  {
  case 200:
    response = "HTTP/1.1 200 OK\r\n\r\n";
    return;

  case 201:
    infile.open("201.html");
    break;

  case 204:
    response = "HTTP/1.1 204 No Content\r\n\r\n";
    return;

  case 301:
    response = "HTTP/1.1 301 Moved Permanently\r\nlocation: " + response + "\r\nConnection: close"+ "\r\n\r\n";
    return;

  case 307:
    response = "HTTP/1.1 307 Temporary Redirect\r\nlocation: " + response + "\r\nConnection: close"+ "\r\n\r\n";
    return;

  case 308:
    response = "HTTP/1.1 307 Permanent Redirect\r\nlocation: " + response + "\r\nConnection: close"+ "\r\n\r\n";
    return;

  case 403:
    infile.open("403.html");
    break;

  case 404:
    infile.open("404.html");
    break;

  case 501:
    infile.open("501.html");
    break;

  default:
    infile.open("500.html");
    break;
  }

  if (!infile.is_open()) {
    file = "<html><body><h1>404 Not Found</h1></body></html>";
    code = 404;
  }
  else
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

  case 500:
    header.append("500 Internal Server Error");
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
