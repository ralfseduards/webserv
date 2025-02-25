#include "../includes/webserv.hpp"

/* Functions validates if the key string only conatains valid characters.
 * The valid characters are all alphanumeric ones + the allowed_chars string.
*/
static bool validate_header_key(std::string& key)
{
  const std::string allowed_chars = "!#$%&'*+.^_`|~";

  for(std::string::iterator it = key.begin(); it != key.end(); ++it)
    if (std::isalnum(*it) == 0 && allowed_chars.find(*it) != std::string::npos)
      return (false);
  return (true);
}

/* Functions validates if the value string only conatains valid characters.
 * The valid characters are tabs and ascii from 32-126 inclusi
*/
static bool validate_header_value(std::string& value)
{
  for(std::string::iterator it = value.begin(); it != value.end(); ++it)
      if (*it != '\t' && !(*it >= 32 && *it <= 126))
        return (false);
  return (true);
}

/* Removes carriage returns from a single line from the request header */
static void sanitize_line(std::string& line) {
  // Since readline does not include the newline character, pop the remaining
  // Carriage return, then replace any loose carriage returns that may be
  // included in error.
  if (*(line.end() - 1) == '\r')
    line.pop_back();
  std::replace(line.begin(), line.end(), '\r', ' ');
}

static int parse_line(std::string& line, Request& new_request)
{
  //Find the mandatory colon that seperates key from content
  size_t colon = line.find(':');
  if (colon == std::string::npos)
  {
    std::clog << "Header colon not found\n" << line << std:: endl;
    return (HEADER_INVAL_COLON);
  }
  std::string key = line.substr(0, colon);
  std::string value = line.substr(colon + 1);

  // "strip" tabs or spaces from the start of the value
  size_t start = value.find_first_not_of(" \t");
  size_t end = value.find_last_not_of(" \t");
  value = (start == std::string::npos || end == std::string::npos) ? "" : value.substr(start, end - start + 1);

  if (validate_header_key(key) == false) {
    std::clog << "header key validation failed" << std::endl;
    return (HEADER_INVAL_KEY);
  }
  if (validate_header_value(value) == false) {
    std::clog << "header value validation failed" << std::endl;
    return (HEADER_INVAL_VAL);
  }

  //Put the "key" and "value" into the hader map. The key does NOT have the colon
  new_request.header_map.emplace(key, value);
  return (OK);
}

static void set_type(Request& request) {

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

/* Some more parsing for the Request::start_line */
static int set_request_path(Request& request) {
  std::size_t start = request.start_line.find("/");
  if (start == std::string::npos)
    return (HEADER_INVAL_VAL);
  std::size_t  end  = request.start_line.find(" ", start);
  if (end == std::string::npos)
    return (HEADER_INVAL_VAL);
  if (end < start)
    return (HEADER_INVAL_VAL);
  request.request_path = request.start_line.substr(start, end - (start));
  return (OK);
}

/* Parses the request header, that up until now has been contained in a single
 * string, into a new Request struct. */
int parse_header(std::string header, Request& new_request)
{
  std::istringstream stream(header);
  std::string line;
  int status = 0;

  std::getline(stream, new_request.start_line);
  while (std::getline(stream, line))
  {
    if (line.empty())
      break;
    sanitize_line(line);
    status = parse_line(line, new_request);
    if (status != OK)
      return (status);
  }
  set_type(new_request);
  status = set_request_path(new_request);
  return (status);
}
