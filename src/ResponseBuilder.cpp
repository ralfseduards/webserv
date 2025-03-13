#include "../includes/webserv.hpp"

void load_http_code_page(Client& client, Response& response)
{
  std::map<int, std::string>::iterator it =
    client.server->errorPages.find(response.http_code);
  if (it != client.server->errorPages.end()) {
    std::string customPath = it->second;
    std::string fullErrPath = client.server->root_directory + "/" + customPath;
    std::ifstream infile(fullErrPath.c_str());
    if (infile.is_open()) {
      // read custom error page
      response.file_content = std::string(
	(std::istreambuf_iterator<char>(infile)),
	std::istreambuf_iterator<char>());
      response.request_path = customPath;
      return;
    }
  }
  response.request_path = "error.html";

  //  Fallback logic if no custom error page is set in config
  switch (response.http_code) {
    case 400:
      response.file_content = "<html><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
      break;
    case 403:
      response.file_content = "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
      break;
    case 404:
      response.file_content = "<html><body><h1>404 Not Found</h1><p>The requested resource could not be found.</p></body></html>";
      break;
    case 405:
      response.file_content = "<html><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed for this resource.</p></body></html>";
      break;
    case 413:
      response.file_content = "<html><body><h1>413 Payload Too Large</h1><p>The request is too large for the server to process.</p></body></html>";
      break;
    case 500:
      response.file_content = "<html><body><h1>500 Internal Server Error</h1><p>Something went wrong on the server.</p></body></html>";
      break;
    case 501:
      response.file_content = "<html><body><h1>501 Not Implemented</h1><p>The requested method is not supported by the server.</p></body></html>";
      break;
    default:
      response.file_content = "<html><body><h1>Unknown Error</h1><p>An unexpected error occurred.</p></body></html>";
      break;
  }
}

static std::string getMimeType(const std::string &filename) {
  static std::map<std::string, std::string> mimeTypes;

  mimeTypes[".html"] = "text/html";
  mimeTypes[".css"] = "text/css";
  mimeTypes[".js"] = "application/javascript";
  mimeTypes[".json"] = "application/json";
  mimeTypes[".png"] = "image/png";
  mimeTypes[".ico"] = "image/png";
  mimeTypes[".jpg"] = "image/jpeg";
  mimeTypes[".gif"] = "image/gif";
  mimeTypes[".svg"] = "image/svg+xml";
  mimeTypes[".txt"] = "text/plain";
  mimeTypes[".pdf"] = "application/pdf";

  size_t dotPos = filename.rfind('.');
  if (dotPos != std::string::npos)
  {
    std::string extension = filename.substr(dotPos);
    std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
    if (it != mimeTypes.end())
      return (it->second);
  }
  return ("application/octet-stream");
}

/* Response builder for redirection */
static void redirection_response (Response& response) {
  response.content = http_version;
  response.content.append(" ");
  response.content.append(response.code_string);
  response.content.append(LINE_DELIMITER);
  response.content.append("Location: ");
  response.content.append(response.redirection_URL);
  response.content.append(LINE_DELIMITER);
  response.content.append("Connection: close");
  response.content.append(LINE_DELIMITER);
  response.content.append(LINE_DELIMITER);
}

/* Response builder for every request that doesnt involve redirection */
static void content_response(Response& response)
{
  response.content = http_version;
  response.content.append(" ");
  response.content.append(response.code_string);
  response.content.append(LINE_DELIMITER);
  response.content.append("Content-Type: ");
  response.content.append(response.content_type);
  response.content.append(LINE_DELIMITER);
  if (response.http_code > 400) {
    response.content.append("Connection close");
    response.content.append(LINE_DELIMITER);
  }
  response.content.append("Content-Length: ");
  std::stringstream ss;
  ss << response.file_content.length();
  response.content.append(ss.str());
  response.content.append(LINE_DELIMITER);
  response.content.append(LINE_DELIMITER);
  response.content.append(response.file_content);
  response.content.append(LINE_DELIMITER);
  response.content.append(LINE_DELIMITER);

}

/* Takes the integer repsonse code and returns a string with the full response status */
static std::string return_http_code(int code)
{
  switch (code)
  {
    case 200: return ("200 OK");
    case 201: return ("201 Created");
    case 204: return ("204 No Content");
    case 301: return ("301 Moved Permanently");
    case 307: return ("307 Temporary Redirect");
    case 308: return ("308 Permanent Redirect");
    case 400: return ("400 Bad Request");
    case 403: return ("403 Forbidden");
    case 404: return ("404 Not Found");
    case 405: return ("405 Method Not Allowed");
    case 413: return ("413 Content Too Large");
    case 500: return ("500 Internal Server Error");
    case 501: return ("501 Not Implemented");
    case 505: return ("505 HTTP Version Not Supported");
    default:  return ("404 Not Found");
  }
}

/* Main function for buliding a http response that later gets sent to the client by send_response() */
void http_response(Client& client, Response& response) {

  response.code_string = return_http_code(response.http_code);
  if (response.http_code == 301 || response.http_code == 302 || response.http_code == 307 || response.http_code == 308) {
    redirection_response(response);
    return ;
  }
  if (response.http_code >= 400 && response.has_content == false) {
    load_http_code_page(client, response);
  }

  if (response.manual_response == true)
	  response.content_type = "text/html";
  else
	  response.content_type = getMimeType(response.request_path);

  std::cout <<"mime type is "<< response.content_type << std::endl;
  content_response(response);
}
