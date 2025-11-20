/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:29 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:41:31 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Webserv.hpp"

std::vector<struct pollfd>									Webserv::_pfds;
std::map<int, Client *>										Webserv::_clients;
std::map<int, Server *>										Webserv::_servers;
std::map<std::string, std::map<std::string, std::string> >	Webserv::_server_sessions;

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Webserv::Webserv() : _fd(), _index() {}

Webserv::Webserv(const Webserv &srcs)
{
	*this = srcs;
}

Webserv &Webserv::operator=(Webserv const &rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_index = rhs._index;
	}
	return (*this);
}

Webserv::Webserv(std::string FileName) : _fd(), _index()
{
	std::string Config;
	if (FileName.empty())
		FileName = "conf/1.conf";	
	Config = ExtractConfig(FileName);
	if(Config.find("server") == std::string::npos)
			throw_error(std::string(std::string("Error in Webserv constructor : ") + FileName + " has no server bloc !" ).c_str());
	while (Config.find("server") != std::string::npos)
		ServerMaker(Config);
	for (size_t i = 0; i < Config.size(); i++)
	{
		if (!isspace(Config[i]))
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
	}
}

Webserv::~Webserv() {}

/*****************	MEMBER		*******************/

std::string Webserv::ExtractConfig(std::string FileName)
{
	std::string Config, line;
	std::ifstream ConfigFile(FileName.c_str());
	struct stat s;
	if (stat(FileName.c_str(), &s) == 0)
		if(s.st_mode & S_IFDIR)
			throw_error(std::string(std::string("Error in ExtractConfig : ") + FileName + " is a directory !" ).c_str());
	if (!ConfigFile.is_open())
		throw_error(std::string(std::string("Error in ExtractConfig : ") + FileName + " : " + std::strerror(errno)).c_str());
	while (!ConfigFile.eof())
	{
		std::getline(ConfigFile, line);
		Config += line + "\n";
	}
	ConfigFile.close();
	return (Config);
}

void Webserv::ServerMaker(std::string &Config)
{
	std::vector<Server *> servs;
	Server *tempServer = new Server(Config);
	std::string temp;
	std::map<std::string, std::string> conf = tempServer->getConfig();
	std::map<std::string, std::string>::iterator itPort = conf.find("listen");
	std::vector<std::string> ports;
	if (itPort == conf.end())
		temp = "8080";
	else
		temp = itPort->second;
	std::istringstream iss(temp);
	temp.clear();
	while (iss >> temp)
		ports.push_back(temp);
	std::map<std::string, std::string>::iterator itIP = conf.find("server_name");
	std::vector<std::string> IPs;
	if (itIP == conf.end())
		temp = "localhost";
	else
		temp = itIP->second;
	std::istringstream iss2(temp);
	temp.clear();
	while (iss2 >> temp)
		IPs.push_back(temp);
	while (IPs.size() > 0)
	{
		std::vector<std::string> tempPorts = ports;
		CheckAvailablePorts(*IPs.rbegin(), tempPorts);
		while (tempPorts.size() > 0)
		{
			Server *newServer = new Server(*tempServer);
			newServer->setPort(*tempPorts.rbegin());
			newServer->setIP(*IPs.rbegin());
			tempPorts.pop_back();
			servs.push_back(newServer);
		}
		IPs.pop_back();
	}
	delete tempServer;
	for (size_t i = 0; i < servs.size(); i++)
	{
		if (servs[i]->make_listening_socket())
			_servers[servs[i]->_fd] = servs[i];
		else
			delete servs[i];
	}
}

void Webserv::CheckAvailablePorts(std::string currentIP, std::vector<std::string> &tempPorts)
{
	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second->getConfig()["server_name"] == currentIP)
		{
			if (it->second->getConfig()["listen"] == *tempPorts.rbegin())
			{
				std::cerr << RED << "Error in configuration file : conflicting server name \"" << RESET << currentIP << RED << "\" on port " << RESET << *tempPorts.rbegin() << RED << ", ignored" << RESET << std::endl;
				tempPorts.pop_back();
			}
		}
	}
}

void Webserv::throw_error(const char *msg)
{
	clean_close();
	throw std::runtime_error(msg);
}

void Webserv::clean_close()
{
	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second)
			delete it->second;
	}
	_servers.clear();
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second)
		{
			it->second->clearClient();
			delete it->second;
		}
	}
	_clients.clear();
	for (unsigned int i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd > 0)
		{
			if (close(_pfds[i].fd) < 0)
				throw std::runtime_error(std::string(std::string("Error in clean_close : close failed : ") + std::strerror(errno)).c_str());
			else
				_pfds[i].fd = -1;
		}
	}
}
