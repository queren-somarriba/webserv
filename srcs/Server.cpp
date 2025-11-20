/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/10/16 10:32:59 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Location.hpp"
#include "Server.hpp"
  
/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/
Server::Server(): Webserv(), _locations(), _config(), _error_pages(), _cgis() {} 

Server::Server(const Server& srcs)
{
	*this = srcs;
}

Server	&Server::operator=(Server const& rhs)
{
	if (this != &rhs)
	{
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgis = rhs._cgis;
		this->_fd = rhs._fd;
		for(std::map<std::string, Location*>::const_iterator it = rhs._locations.begin(); it != rhs._locations.end(); it++)
			this->_locations[it->first] = new Location(*it->second);
	}
	return (*this);
}

Server::Server(std::string & Config) : Webserv(), _locations()
{
	size_t it = Config.find("server");
	for(size_t i = 0; i < it; i++)
	{
		if(!isspace(Config[i]))
			throw_error("Error in configuration file: directive is not allowed before server bloc !");
	}
	try
	{
		ExtractBloc(Config, it + 6);
	}
	catch (std::exception &e) 
	{
		for(std::map<std::string, Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
		{
			if(it->second)
				delete it->second;
		}
		throw_error(e.what());
	}	
	Config.erase(0, it + 6);
}

Server::~Server()
{
    this->_config.clear();
	for(std::map<std::string, Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		if(it->second)
			delete it->second;
	}
    this->_locations.clear();
}

/*****************	MEMBER		*******************/

void	Server::ExtractBloc(std::string & Config, size_t it)
{
	std::string key, value, location_name;
	size_t	i = it;
	CheckBeforeBracket(Config, i);	
	while (Config[i])
	{
		key = GetConfigKey(Config, i);
		if (key.empty())
			break;	
		CheckDirective(key, false);
		if (key == "location" )
		{
			location_name = GetConfigKey(Config, i);
			if(location_name.empty())
				break;
			if(_locations.find(location_name) != _locations.end())
				throw_error(std::string("Error in configuration file: location is a duplicate : " + key).c_str());		
			Location temp(location_name);
			temp.ExtractLocBloc(Config, i);
			_locations[location_name] = new Location(temp);
			continue;
		}
		value = GetConfigValue(Config, i);
		if (value.empty())
			break;
		if(key == "error_page")
			GetErrorPageConfig(value);
		else if (key == "cgi")
			GetCGIConfig(value);
		else if ((key == "autoindex") && (value.compare("on") && value.compare("ON") && value.compare("off") && value.compare("OFF")))
				throw_error(std::string("Error in configuration file : wrong autoindex value (" + value + ")").c_str());
		else
			this->_config[key] = value;
	}
	if (Config[i] && Config[i] == '}')
		Config.erase(it, (i - it + 1));		
	else
			throw_error("Error in configuration file.");
}

void	Server::GetCGIConfig(std::string value)
{
	std::string extension, path, check;
	std::istringstream ss(value);
	ss >> extension >> path >> check;
	if (!check.empty() || path.empty())
		throw_error(std::string("Error in configuration file : wrong cgi directive (" + extension + ")").c_str());
	this->_cgis[extension] = path;
}

void	Server::GetErrorPageConfig(std::string value)
{
	std::string code, uri, check;
	std::istringstream ss(value);
	ss >> code >> uri >> check;
	if (!check.empty() || uri.empty())
		throw_error(std::string("Error in configuration file : wrong error_page directive (" + code + ")").c_str());
	this->_error_pages[code] = uri;
}

void	Server::CheckBeforeBracket(std::string Config, size_t & i)
{
	while (std::isspace(Config[i]) && Config[i])
		i++;
	if (Config[i] != '{')
			throw_error("Error in configuration file: directive is not allowed between bloc start and '{'!");
	i++;	
}

std::string	Server::GetConfigKey(std::string Config, size_t & i)
{
	size_t j;

	while (std::isspace(Config[i]) && Config[i])
		i++;
	j = i;
	while (!std::isspace(Config[i]) && Config[i])
	{
		if (Config[i] == '}' || Config[i] == '{' || Config[i] == ';')
		{
			return("");
		}			
		i++;
	}
	return(Config.substr(j, (i - j)));
}

void	Server::CheckDirective(std::string & key, bool mod)
{
	std::vector <std::string> dirs;
	std::string directives, temp;
	size_t i = 0;
	if (mod == false)
	{
		directives = "listen index server_name error_page client_max_body_size location return root autoindex allowed_methods cgi upload_folder";	
		i = 12;
	}	
	else if (mod == true)
	{
		directives = "error_page index client_max_body_size return root autoindex allowed_methods cgi upload_folder";
		i = 9;
	}
	std::istringstream ss(directives);
	while(dirs.size() < i)
	{
		ss >> temp;
		dirs.push_back(temp);
		temp.clear();
	}
	i = 0;
	while (i < dirs.size())
	{
		if (key == dirs[i])
			break;
		i++;
		if (i == dirs.size())
			throw_error(std::string("Error in configuration file: directive is not allowed : " + key).c_str());			
	}
	if(_config.find(key) != _config.end())
		throw_error(std::string("Error in configuration file: directive is a duplicate : " + key).c_str());	
}

std::string	Server::GetConfigValue(std::string Config, size_t & i)
{
	size_t j;
	
	while (std::isspace(Config[i]) && Config[i])
		i++;
	j = i;
	while (Config[i] != ';' && Config[i])
	{
		if (Config[i] == '}' || Config[i] == '{' || Config[i] =='\n')
			return("");
		i++;
	}
	i++;
	return(Config.substr(j, (i-j) -1));
}

std::map<std::string, std::string>	Server::getConfig()
{
	return(this->_config);
}

std::map<std::string, Location*>	Server::getLocations()
{
	return(this->_locations);
}

std::map<std::string, std::string>	Server::getCgi()
{
	return(this->_cgis);
}

std::map<std::string, std::string>	Server::getErrPage()
{
	return(this->_error_pages);
}

void	Server::setPort(std::string port)
{
	this->_config["listen"] = port;
}

void	Server::setIP(std::string IP)
{
	this->_config["server_name"] = IP;
}

void	Server::printconfig()
{
	std::cout << BOLD << CYAN << "***************CONFIGURATION FOR SERVER  FD " << this->_fd << "**************"<< RESET << std::endl;
	for (std::map<std::string, std::string>::iterator pconf = this->_config.begin(); pconf != _config.end(); pconf++)
		std::cout << BOLD << pconf->first << RESET << " " << GREEN << pconf->second << RESET <<std::endl;
	if(!this->_error_pages.empty())
		std::cout << BOLD << RED << "______ERROR CONFIGURATION :"<< RESET << std::endl;
	for (std::map<std::string, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		std::cout << RED << "______Error : " << RED << it->first << " " << it->second << RESET  << std::endl;
	if(!this->_cgis.empty())
		std::cout << BOLD << PURPLE << "______CGI CONFIGURATION :"<< RESET << std::endl;
	for (std::map<std::string, std::string>::iterator it = _cgis.begin(); it != _cgis.end(); it++)
		std::cout << PURPLE << "______CGI : " << PURPLE << it->first << " " << it->second << RESET  << std::endl;
	if(!this->_locations.empty())
		std::cout << BOLD << CYAN << "***************LOCATIONS**************"<< RESET << std::endl;
	for (std::map<std::string, Location *>::iterator ploc = _locations.begin(); ploc != _locations.end(); ploc++)
	{
		std::cout << BOLD << YELLOW << "______LOCATION NAME : " << BOLD << YELLOW << ploc->first << RESET << std::endl;
		for (std::map<std::string, std::string>::iterator locconf = ploc->second->_config.begin(); locconf != ploc->second->_config.end(); locconf++)
		std::cout << BOLD << locconf->first << RESET << " " << GREEN << locconf->second << RESET << std::endl;	
		if(!ploc->second->_error_pages.empty())
			std::cout << BOLD << RED << "____________ERROR CONFIGURATION :"<< RESET << std::endl;
		for (std::map<std::string, std::string>::iterator it = ploc->second->_error_pages.begin(); it != ploc->second->_error_pages.end(); it++)
			std::cout << RED << "____________Error : " << RED << it->first << " " << it->second << RESET   << std::endl;
		if(!ploc->second->_cgis.empty())
			std::cout << BOLD << PURPLE << "____________CGI CONFIGURATION:"<< RESET << std::endl;
		for (std::map<std::string, std::string>::iterator it = ploc->second->_cgis.begin(); it != ploc->second->_cgis.end(); it++)
			std::cout <<PURPLE << "____________CGI : " << PURPLE << it->first << " " << it->second << RESET  << std::endl;
	}
}