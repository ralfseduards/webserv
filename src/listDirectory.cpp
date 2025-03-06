#include "../includes/webserv.hpp"

/* This is a function that lists all files in our www directory recursively. */
static int list_files(std::string rootPath, Client& client)
{
  DIR *root;
  struct dirent *entry;

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

  client.waitlist[0].response.file_content.clear();
  client.waitlist[0].response.file_content += "<!DOCTYPE HTML>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n";
  client.waitlist[0].response.file_content += "<title>Directory listing for " + rootPath + "</title>\n</head>\n<body>\n";
  client.waitlist[0].response.file_content += "<h1>Directory listing for " + rootPath + "</h1>\n<hr>\n<ul>\n";
  while ((entry = readdir(root)) != NULL)
  {

    client.waitlist[0].response.file_content += "<li><a href=\"" + rootPath + "\">" + rootPath + "</a></li>\n";
    //if (entry->d_name[0] != '.')
    //{ 
    //  std::string fullpath = rootPath + '/' + entry->d_name;
    //  if (entry->d_type == DT_DIR)
    //  {
    //    exit_status = list_files(fullpath.c_str());
    //    if (exit_status != 0)  
    //      return (exit_status);
    //  }
    //  else
    //    std::cout << entry->d_name << '\n';
    //}
  }
  closedir(root);

  client.waitlist[0].response.file_content += "</ul>\n<hr>\n</body>\n</html>";

  return (0);
}

static void get_dirname(Client& client, std::string& dirname)
{
  std::stringstream ss(client.waitlist[0].start_line);
       
  /* the dirname is always going to be the second item */
  std::getline(ss, dirname, ' ');
  std::getline(ss, dirname, ' ');
}

/*
<!DOCTYPE HTML>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Directory listing for /</title>
</head>
<body>
<h1>Directory listing for /</h1>
<hr>
<ul>
<li><a href="01-pages/">01-pages/</a></li>
<li><a href="02-received/">02-received/</a></li>
<li><a href="03-stash/">03-stash/</a></li>
<li><a href="cgi-bin/">cgi-bin/</a></li>
<li><a href="hi">hi</a></li>
</ul>
<hr>
</body>
</html>
*/


// TODO: check autoindex
void list_directory(Client& client)
{
  std::string dirname;
  int         retval;

  get_dirname(client, dirname);
  std::clog << "-> this is the dirname:" << dirname << std::endl;
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

