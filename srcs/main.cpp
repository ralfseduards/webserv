#include "../includes/Config.hpp"

int main(int argc, char **argv)
{
	std::string configPath = (argc == 2) ? argv[1] : "config/webserv.conf";

	Config config(configPath);
	config.printConfig();

	return 0;
}
