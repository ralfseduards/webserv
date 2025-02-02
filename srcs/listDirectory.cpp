#include "../includes/webserv.hpp"

/* This is a function that lists all files in our www directory recursively. */

void list_files(const char* rootPath)
{
  DIR *root = opendir(rootPath);
  struct dirent *entry;

  while ((entry = readdir(root)) != NULL)
  {
    if (entry->d_name[0] != '.')
    {
      std::string fullpath = std::string(rootPath) + '/' + entry->d_name;
      if (entry->d_type == DT_DIR)
        list_files(fullpath.c_str());
      else
        std::cout << entry->d_name << '\n';
    }
  }
  closedir(root);
}

// TEST main
// int main()
// {
//   list_files("/home/tom/Desktop/code/school/webserv/www");
//   return(0);
// }
