#include "../includes/webserv.hpp"
#include <ctime>

#define MAX_LINE_LEN 100

/* extracts the file requested from the first request line */
static std::string get_file_requested(Client& client)
{
  std::stringstream ss(client.waitlist[0].start_line);
  std::string req_dir;

  /* the dirname is always going to be the second item */
  std::getline(ss, req_dir, ' ');
  std::getline(ss, req_dir, ' ');
  return (req_dir);
}

/* A function that receives a path of a directory as an arg, lists all files inside
 * of it and puts a formatted html page of theese files in response.file_content to
 * be returned to the client as directory listing.
*/
static int list_files(std::string rootPath, Client& client)
{
  DIR *root;
  struct dirent *entry;
  std::string file_requested;
  struct stat stat_buffer;

  std::clog << "==>> listing directory: " << rootPath << std::endl;
  root = opendir(rootPath.c_str());
  if (root == NULL)
  {
    switch (errno)
    {
      case EACCES:
        return(1);
      case ENOENT:
        return (2);
      default:
        return (3);   /* other errors (server error) */
    }
  }
  file_requested = get_file_requested(client);
  client.waitlist[0].response.file_content.clear();
  client.waitlist[0].response.file_content += "<!DOCTYPE HTML>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n";
  client.waitlist[0].response.file_content += "<title>Index of " + file_requested + "</title>\n</head>\n<body>\n";
  client.waitlist[0].response.file_content += "<h1>Index of " + file_requested + "</h1>\n<hr>\n<pre>\n";
  while ((entry = readdir(root)) != NULL)
  {
    std::string filename(entry->d_name);
    std::string full_file_path = rootPath + "/" + filename;
    std::stringstream ss;
    std::string timestr;

    stat(full_file_path.c_str(), &stat_buffer);
    timestr = std::ctime(&stat_buffer.st_mtim.tv_sec);
    ss.clear();

    // this writing the actual line here
    ss << filename << std::setw(MAX_LINE_LEN - filename.size()) <<  timestr.substr(0, timestr.size()-1)
      << "\t\t" << stat_buffer.st_size << std::endl;
    client.waitlist[0].response.file_content += ss.str();
  }
  closedir(root);

  client.waitlist[0].response.file_content += "</pre>\n<hr>\n</body>\n</html>";

  return (0);
}

/* Gets the full path to the file requested */
static void get_dirname(Client& client, std::string& dirname)
{
  dirname += client.server->root_directory;
  dirname += "/" + client.waitlist[0].request_path;
}

/* This function is used to handle the case when the client reqeusts a directory,
 * not a file.
 * If the server autoindex is ON in the config, then the function will return a html
 * page of the listed contents of the directory.
 * If the server autoindex is OFF, the function will still look for an index.html in
 * that directory, and if that doesnt exist, then its an error.
*/
void handle_directory(Client& client)
{
  std::string dirname;
  int         retval;

  get_dirname(client, dirname);

  // if autoindex is off, the still try to search for an index.html file
  if (client.server->autoindex == false)
  {
    client.waitlist[0].request_path += "/index.html";
    read_file(client, client.waitlist[0]);
    return ;
  }
  retval = list_files(dirname.c_str(), client);
  switch (retval)
  {
    case 0:
      client.waitlist[0].response.http_code = 200;
      break;
    case 1:
      client.waitlist[0].response.http_code = 403;
      break;
    case 2:
      client.waitlist[0].response.http_code = 404;
      break;
    default:
      client.waitlist[0].response.http_code = 500;
      break;
  }
  client.waitlist[0].response.has_content = true;
  client.waitlist[0].response.manual_response= true;
}
