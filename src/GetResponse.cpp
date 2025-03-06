#include "../includes/webserv.hpp"


bool get_response(Client& client)
{
  struct stat stats;

  stat(client.waitlist[0].request_path.c_str(), &stats);
  if (S_ISDIR(stats.st_mode) || access(client.waitlist[0].request_path.c_str(), R_OK) == -1)
    list_directory(client);
  else
  {
    std::ifstream	infile;
    infile.open(client.waitlist[0].request_path.c_str());
    client.waitlist[0].response.file_content = std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    client.waitlist[0].response.request_path = client.waitlist[0].request_path;
    client.waitlist[0].response.has_content = true;
    client.waitlist[0].response.http_code = 200;
  }
    http_response(client, client.waitlist[0].response);
  return (false);
}
