#include "../includes/cgi.hpp"

/* like strdup() in C but using strings and new */
static char* cpp_strdup(std::string str)
{
  char *newStr = new char[str.size() + 1];
  int i = 0;

  for (std::string::iterator it = str.begin(); it <= str.end(); ++it)
    newStr[i++] = *it;
  newStr[str.size()] = '\0';
  return (newStr);
}

/* function returns PATH=... with whatever value is set in envp.
 * (returns "PATH=" if there is no path set)
*/
static std::string find_path(char **envp)
{
  std::string check_str = "PATH=";

  /* find PATH */
  for (int i = 0; envp[i] != NULL; ++i)
  {
    check_str = envp[i];
    if (check_str.substr(0, 5) == "PATH=")
      break ;
    check_str = "PATH=";
  }
  return(check_str);
}

/* Extracts REQUEST_METHOD, SCRIPT_NAME and QUERY_STRING from the frist
 * line of the http request. Sets them as customenvp[2,3,4]
*/
static void parse_first_line(const std::string& line, char **&custom_envp)
{
  std::size_t start = 0, end;
  std::vector<std::string> tokens;

  end = line.find(' ');
  /* gets the first 2 tokens, not the http version */
  while (end != std::string::npos)
  {
    tokens.push_back(line.substr(start, end - start));
    start = end + 1;
    end = line.find(' ', start);
  }
  custom_envp[2] = cpp_strdup("REQUEST_METHOD=" + tokens[0]);

  /* set the tokens[1] to be the actual file name and everything after (the query string) */
  size_t last_pos = tokens[1].find_last_of("/");
  tokens[1] = tokens[1].substr(last_pos);

  /* extract the query string if it exists */
  end = tokens[1].find('?');
  if(end != std::string::npos)
  {
    custom_envp[3] = cpp_strdup("SCRIPT_NAME=" + tokens[1].substr(0, end));
    custom_envp[4] = cpp_strdup("QUERY_STRING=" + tokens[1].substr(end+1));
  }
  else
{
    custom_envp[3] = cpp_strdup("SCRIPT_NAME=" + tokens[1]);
    custom_envp[4] = cpp_strdup("QUERY_STRING=");
  }
}

/* Takes my custom_envp and finds 2 values: SCRIPT_NAME and DOCUMENT_ROOT and combines
 * them to make a resulting string of the full absolute path of the executable.
 * SCRIPT_NAME and DOCUMENT_ROOT should always be available and have values.
*/
void get_program_name(std::string& result_prog, std::string& result_dir, char **custom_envp, Client& client)
{
  std::string script_name;
  std::string doc_root;
  std::string token;

  for(int i = 0; custom_envp[i] != NULL; ++i)
  {
    if (std::strncmp(custom_envp[i], "SCRIPT_NAME=", 12) == 0)
    {
      token = custom_envp[i];
      script_name = token.substr(13);   // exclude the starting '/'
    }
    else if (std::strncmp(custom_envp[i], "DOCUMENT_ROOT=", 14) == 0)
    {
      token = custom_envp[i];
      doc_root = token.substr(14);
    }
  }
  if (doc_root[doc_root.size() - 1] != '/')
    doc_root.push_back('/');

  result_prog = doc_root + client.server->routing_table["/cgi-bin"];
  if (result_prog[result_prog.size() - 1] != '/')
    result_prog.push_back('/');
  result_prog += script_name;

  result_dir = doc_root + client.server->routing_table["/cgi-bin"];
}

/* Takes a 'token vector', envp; and fills the custom_envp with values. The logic is a little hardcoded,
 * taking into account the size of custom_envp and the custom env variable names that we need.
 *
 * USES THE environ GLOBAL FOR GETTING THE ENVIRONMENT
*/
void create_new_envp(Client& client, char **&custom_envp)
{
  // a shorthand for client.waitlist[0]
  Request req = client.waitlist[0];

  custom_envp[0] = cpp_strdup(find_path(environ));
  custom_envp[1] = cpp_strdup("DOCUMENT_ROOT=" + client.server->root_directory);
  parse_first_line(req.start_line, custom_envp);
  custom_envp[5] = cpp_strdup("CONTENT_TYPE=" + req.header_map["Content-Type"]);
  custom_envp[6] = cpp_strdup("CONTENT_LENGHT=" + req.header_map["Content-Lenght"]);
  custom_envp[7] = cpp_strdup("HTTP_USER_AGENT=" + req.header_map["User-Agent"]);
  custom_envp[8] = cpp_strdup("HTTP_HOST=" + req.header_map["Host"]);
  custom_envp[9] = cpp_strdup("HTTP_REFERRER=" + req.header_map["Referrer:"]);
  custom_envp[10] = NULL;
}
