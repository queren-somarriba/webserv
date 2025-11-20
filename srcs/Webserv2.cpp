/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:15:37 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 14:38:34 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Webserv.hpp"

void Webserv::runWebserv()
{
	int status;
	
	if (_pfds.empty())
		throw_error("No server is running, exiting Webserv...");	 
	while (1)
	{
		for (nfds_t j = 0; j < _pfds.size(); ++j)
		{
			if (_pfds[j].fd && _clients[_pfds[j].fd] && _clients[_pfds[j].fd]->checkTimeout())
				break;			
			if (_pfds[j].fd && _clients[_pfds[j].fd])
				_clients[_pfds[j].fd]->checkStatusCGI();
		}
		status = poll(_pfds.data(), _pfds.size(), 500);
		if (status < 0)
			throw_error("Error in runWebserv : polling failed !");
		if (status == 0)
			continue;
		for (nfds_t j = 0; j < _pfds.size(); ++j)
		{
			if ((_pfds[j].revents & POLLIN) && _servers.find(_pfds[j].fd) != _servers.end())
				_servers[_pfds[j].fd]->add_client_to_pollfds();
			else if ((_pfds[j].revents & POLLIN) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->handle_request();
			else if ((_pfds[j].revents & POLLOUT) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->send_answer();
		}
	}
}

void Webserv::erase_client()
{
	if (close(_pfds[this->_index].fd) < 0)
		throw std::string(std::string("Error in erase_client : close failed : ") + std::strerror(errno)).c_str();
	_pfds.erase(_pfds.begin() + this->_index);
	std::map<int, Client *>::iterator it = _clients.find(this->_fd);
	it->second->clearClient();
	delete it->second;
	_clients.erase(it);
	setIndex();
}

void Webserv::setIndex()
{
	std::map<int, Server *>::iterator itServer = _servers.begin();
	while (itServer != _servers.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itServer->first == _pfds[i].fd)
			{
				itServer->second->_index = i;
				break;
			}
		}
		itServer++;
	}
	std::map<int, Client *>::iterator itClient = _clients.begin();
	while (itClient != _clients.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itClient->first == _pfds[i].fd)
			{
				if (itClient->second)
					itClient->second->_index = i;
				break;
			}
		}
		itClient++;
	}
}