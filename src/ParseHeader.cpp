#include "../includes/webserv.hpp"

#include <string>


//TODO: replace with 98 function
bool validate_header_key(std::string& key) {
  const std::regex key_regex("^[!#$%&'*+.^_`|~0-9a-zA-Z-]+$");
  return (std::regex_match(key, key_regex));
}

//TODO: replace with 98 function
bool validate_header_value(std::string& value) {
  const std::regex value_regex("^[\\t\\x20-\\x7E]*$");
  return (std::regex_match(value, value_regex));
}

bool validate_http_version(const std::string& start_line) {
    size_t http_pos = start_line.find("HTTP/");
    if (http_pos == std::string::npos) {
        return false;
    }
    std::string version = start_line.substr(http_pos);
    if (version[version.length() - 1] == '\r') {
        version = version.substr(0, version.length() - 1);
    }
    return (version == "HTTP/1.1" || version == "HTTP/1.0");
}
void sanitize_line(std::string& line) {
  // Since readline does not include the newline character, pop the remaining
  // Carriage return, then replace any loose carriage returns that may be
  // included in error.
  if (*(line.end() - 1) == '\r')
    line.pop_back();
  std::replace(line.begin(), line.end(), '\r', ' ');
}

int parse_line(std::string& line, Request& new_request) {

  //Find the mandatory colon that seperates key from content
  size_t colon = line.find(':');
  if (colon == std::string::npos) {
    std::clog << "Header colon not found\n" << line << std:: endl;
    return (HEADER_INVAL_COLON);
  }
  std::string key = line.substr(0, colon);
  std::string value = line.substr(colon + 1);

  size_t start = value.find_first_not_of(" \t");
  size_t end = value.find_last_not_of(" \t");
  value = (start == std::string::npos || end == std::string::npos) ? "" : value.substr(start, end - start + 1);

  if (validate_header_key(key) == false) {
    std::clog << "header key validation failed" << std::endl;
    return (HEADER_INVAL_REGEX_KEY);
  }
  if (validate_header_value(value) == false) {
    std::clog << "header value validation failed" << std::endl;
    return (HEADER_INVAL_REGEX_VAL);
  }

  new_request.header_map.emplace(key, value);
  return (OK);
}

void set_type(Request& request) {

  size_t prefix_len = request.start_line.find(' ');
  if (prefix_len == std::string::npos) {
    request.type = INVALID;
    return ;
  }
  std::string temp = request.start_line.substr(0, prefix_len);
  if (temp == "GET")
    request.type = GET;
  else if (temp == "POST")
    request.type = POST;
  else if (temp == "DELETE")
    request.type = DELETE;
  else if (temp == "HEAD")
    request.type = HEAD;
  else {
    request.type = INVALID;
    std::clog << "Unexpected request: " << temp << std::endl;
  }
}

int parse_header(std::string header, Request& new_request) {
  std::istringstream stream(header);
  std::string line;
  int status = 0;

  bool has_content_length = false;
  size_t content_length_value = 0;
  std::getline(stream, new_request.start_line);
  if (!validate_http_version(new_request.start_line))
	return (HEADER_INVAL_VERSION);
  while (std::getline(stream, line)) {

    if (line.empty())
      break;

    sanitize_line(line);
	size_t colon = line.find(':');
	if (colon != std::string::npos) {
		std::string key = line.substr(0, colon);
		if (key == "Content-Length") {
			std::string value = line.substr(colon + 1);
			// Trim spaces
			size_t start = value.find_first_not_of(" \t");
			size_t end = value.find_last_not_of(" \t");
			value = (start == std::string::npos || end == std::string::npos) ? "" : value.substr(start, end - start + 1);

			size_t new_content_length = 0;
			try {
			  new_content_length = std::strtoul(value.c_str(), NULL, 10);
			} catch (...) {
			  return (HEADER_INVAL_CONTENT_LENGTH);
			}

			// Check for duplicate with different value
			if (has_content_length && new_content_length != content_length_value) {
			  std::clog << "Duplicate Content-Length with different values" << std::endl;
			  return (HEADER_INVAL_DUPLICATE);
			}

			has_content_length = true;
			content_length_value = new_content_length;
		  }
	}

    status = parse_line(line, new_request);
    if (status != OK)
      return (status);

  }

  set_type(new_request);
  status = set_request_path(new_request);
  return (status);

}

int set_request_path(Request& request) {
  std::size_t start = request.start_line.find("/");
  if (start == std::string::npos)
    return (HEADER_INVAL_REGEX_VAL);
  std::size_t  end  = request.start_line.find(" ", start);
  if (end == std::string::npos)
    return (HEADER_INVAL_REGEX_VAL);
  if (end < start)
    return (HEADER_INVAL_REGEX_VAL);
  request.request_path = request.start_line.substr(start, end - (start));
  return (OK);
}
