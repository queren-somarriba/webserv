/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:39:12 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "Request.hpp"

void Request::checkRequest()
{
	if(this->_config.find("allowed_methods") != this->_config.end())
		if(this->_config["allowed_methods"].find(this->_methode) == std::string::npos)
			return (setStatus("405"));
	if (this->_methode.compare("GET") && this->_methode.compare("POST") && this->_methode.compare("DELETE"))
			return (setStatus("501"));
	if (this->_headers.find("HOST") == this->_headers.end())
		return (setStatus("400"));	
}

void Request::resolvePath()
{
	std::string finalPath, temPath = this->_path;
	std::vector<std::string> suffix;
	std::map<std::string, std::string>::iterator it = this->_config.find("root");
	std::map<std::string, Location*> temploc = _servers[this->_server_fd]->getLocations();
	if (it != this->_config.end())
		_ogRoot = it->second;
	while (!temPath.empty())
	{
		size_t i = temPath.rfind('/');
		if (i == std::string::npos)
		{
			temPath = "/" + temPath;
			i = temPath.rfind('/');
		}	
		suffix.push_back(temPath.substr(i));
	 	temPath = temPath.substr(0, i);
	}
	while (!suffix.empty())
	{
		temPath += *suffix.rbegin();
		finalPath += *suffix.rbegin();
		suffix.pop_back();
		std::map<std::string, Location*>::iterator MapLoc = temploc.find(temPath);
		if (MapLoc != temploc.end())
	 	{
			std::map<std::string, std::string> loc = MapLoc->second->getConfig();
	 		for (std::map<std::string, std::string>::iterator itLoc = loc.begin(); itLoc != loc.end(); itLoc++)
			{
				if(itLoc->first == "root")
				{
					if(itLoc->second != "/")
						finalPath = "/" + itLoc->second; 
					else
						finalPath.clear();
				}
	 			else
					this->_config[itLoc->first] = itLoc->second;
			}
			if (!MapLoc->second->getCgi().empty())
				this->_cgis = MapLoc->second->getCgi();
			if (!MapLoc->second->getErrPage().empty())
				this->_error_pages = MapLoc->second->getErrPage();
	 	}
	}
	finalPath = _ogRoot + finalPath;
	this->_path = finalPath;
}

void 	Request::printURIConfig()
{
	std::cout << "REQUESTED URI CONFIG = " << this->_path << std::endl; 
	for (std::map<std::string, std::string>::iterator it = _config.begin(); it != _config.end(); it++)
		std::cout << it->first << " " << it->second << std::endl;
}

void	Request::check_cgi()
{
	std::string temp, extension;
	temp = this->_path;
	size_t pos = temp.find(_ogRoot);
	if (temp.size() == _ogRoot.size())
		return;
	if (pos != std::string::npos)
		temp = temp.substr(_ogRoot.size() + 1);
	pos = temp.find("/");
	if (pos != std::string::npos)
		temp = temp.substr(0, pos);
	if (temp == "cgi-bin")
	{
		this->_CGI_bin_path = _ogRoot + "/" + temp + "/";
		this->getCgiScript();
		this->checkCGIExt();
	}
	else 
		this->_isCGI = false;
}

void 	Request::getCgiScript()
{
	std::string temp;
	if (this->_path.size() >= _CGI_bin_path.size())
		temp = this->_path.substr(_CGI_bin_path.size());
	else
		temp = this->_path.substr(_CGI_bin_path.size() - 1);
	size_t pos = temp.find("?");
	if (pos != std::string::npos)
	{
		this->_CGI_querry= temp.substr(pos + 1);
		temp = temp.substr(0, pos);
		pos = temp.find("/");
		if (pos != std::string::npos)
		{
			this->_CGI_pathInfo = temp.substr(pos + 1);
			temp = temp.substr(0, pos);
		}
	}
	else 
	{
		pos = temp.find("/");
		if (pos != std::string::npos)
		{
			this->_CGI_pathInfo = temp.substr(pos + 1);
			temp = temp.substr(0, pos);	
		}
	}
	this->_CGI_script = temp;
}

void	Request::checkCGIExt()
{
	size_t pos = this->_CGI_script.rfind(".");
	if (pos != std::string::npos)
	{
		std::string extension = this->_CGI_script.substr(pos, this->_CGI_script.size() - pos);
		std::map<std::string, std::string>::iterator it = this->_cgis.find(extension);
		if (it != this->_cgis.end())
		{
			this->_CGIinterpret = it->second;
			this->_isCGI = true;
			return ;
		}
	}
	this->_isCGI = false;
}

void Request::clearTmpFiles()
{
	for (std::map<std::string, std::string>::iterator it = this->_files.begin(); it != this->_files.end(); ++it)
		std::remove(it->second.c_str());
	_files.clear();
}