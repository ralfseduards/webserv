#include "../includes/Server.hpp"
#include "../includes/Config.hpp"

// Constructor
Server::Server(const Config &config)
	: dir_list(""),
	  autoindex(config.getGlobalAutoIndex()),
	  client_max_body_size(config.getMaxBodySize()),
	  allowed_methods(config.getGlobalAllowedMethods())
{
	errorPages = config.getErrorPages();
}

// Copy Constructor
Server::Server(const Server &other)
{
	*this = other;
}

// Assignment Operator
Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		server_name = other.server_name;
		host = other.host;
		root = other.root;
		ports = other.ports;
		locations = other.locations;
		errorPages = other.errorPages;
		index = other.index;
		autoindex = other.autoindex;
		client_max_body_size = other.client_max_body_size;
		allowed_methods = other.allowed_methods;
	}
	return *this;
}

// Destructor
Server::~Server() {}

// Getters
const std::string &Server::getServerName() const
{
	return server_name;
}

const std::vector<std::string> &Server::getHost() const
{
	return host;
}

const std::string &Server::getDirList() const
{
	return dir_list;
}

const std::string &Server::getRoot() const
{
	return root;
}

const std::vector<int> &Server::getPorts() const
{
	return ports;
}

const std::map<std::string, Location> &Server::getLocations() const
{
	return locations;
}

const std::map<int, std::string> Server::getErrorPages() const
{
	return errorPages;
}

const std::string &Server::getIndex() const
{
	return index;
}

bool Server::getAutoIndex() const
{
	return autoindex;
}

size_t Server::getMaxBodySize() const
{
	return client_max_body_size;
}

const std::vector<std::string> &Server::getAllowedMethods() const
{
	return allowed_methods;
}

// Setters
void Server::setServerName(std::string name)
{
	server_name = name;
}

void Server::setHost(std::vector<std::string> host)
{
	this->host = host;
}

void Server::setDirList(std::string dir_list)
{
	this->dir_list = dir_list;
}

void Server::setRoot(std::string root)
{
	this->root = root;
}

void Server::setPort(std::vector<int> ports)
{
	this->ports = ports;
}

void Server::setLocations(std::map<std::string, Location> locations)
{
	this->locations = locations;
}

void Server::setErrorPages(std::map<int, std::string> error_pages)
{
	this->errorPages.insert(error_pages.begin(), error_pages.end());
}

void Server::setIndex(std::string index)
{
	this->index = index;
}

void Server::setAutoIndex(bool autoindex)
{
	this->autoindex = autoindex;
}

void Server::setMaxBodySize(size_t client_max_body_size)
{
	this->client_max_body_size = client_max_body_size;
}

void Server::setAllowedMethods(std::vector<std::string> allowed_methods)
{
	this->allowed_methods = allowed_methods;
}

// Add Location
void Server::addLocation(const std::string &path, const Location &location)
{
	locations[path] = location;
}

void Server::printServer() const
{
	std::cout << "Server Name: " << server_name << std::endl;
	std::cout << "Host(s): ";
	for (size_t i = 0; i < host.size(); ++i)
	{
		std::cout << host[i] << (i < host.size() - 1 ? ", " : "");
	}
	std::cout << std::endl;

	std::cout << "Directory Listing: " << dir_list << std::endl;
	std::cout << "Root: " << root << std::endl;

	std::cout << "Ports: ";
	for (size_t i = 0; i < ports.size(); ++i)
	{
		std::cout << ports[i] << (i < ports.size() - 1 ? ", " : "");
	}
	std::cout << std::endl;

	std::cout << "Index: " << index << std::endl;
	std::cout << "Autoindex: " << (autoindex ? "on" : "off") << std::endl;

	std::cout << "Allowed Methods: ";
	for (size_t i = 0; i < allowed_methods.size(); ++i)
	{
		std::cout << allowed_methods[i] << (i < allowed_methods.size() - 1 ? ", " : "");
	}
	std::cout << std::endl;

	std::cout << "Error Pages:" << std::endl;
	for (std::map<int, std::string>::const_iterator it = errorPages.begin();
		 it != errorPages.end(); ++it)
	{
		std::cout << "Error Code: " << it->first << " => Page: " << it->second << std::endl;
	}

	std::cout << "\n=== Locations ===" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = locations.begin();
		 it != locations.end(); ++it)
	{
		std::cout << "\nLocation Path: " << it->first << std::endl;
		it->second.printLocation();
	}
}
