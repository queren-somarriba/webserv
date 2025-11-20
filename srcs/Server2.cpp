/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:12:41 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 14:35:19 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"

int	Server::make_listening_socket()
{
	struct addrinfo	hint;
	struct addrinfo	*addr;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(_config["server_name"].c_str(), _config["listen"].c_str(), &hint, &addr) != 0)
		return (std::cerr << RED << BOLD << "Error in make_listening_socket : Could not resolve "  << RESET  << BOLD << _config["server_name"] << RESET << std::endl, 0);
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		return (std::cerr << RED << "Error in make_listening_socket : Could not create a socket for " << RESET << BOLD << _config["server_name"] << RESET << std::endl, 0);
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		if (close(socket_fd))
			throw std::runtime_error(std::string(std::string("Error inServer::make_listening_socket : close failed: ") + std::strerror(errno)).c_str());
		freeaddrinfo(addr);
		return (std::cerr << BOLD << RED <<  "Error in make_listening_socket : Could not bind " << RESET <<  _config["server_name"] << BOLD << RED <<  " on port " << RESET << _config["listen"] << BOLD << RED << " : " << std::strerror(errno) << RESET << std::endl, 0);
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
		return (std::cerr << RED << BOLD << "Error in make_listening_socket : Listen failed on fd " << RESET << socket_fd << BOLD << RED << "for server " << RESET << _config["server_name"] << BOLD << RED << ":" << RESET << _config["listen"] << BOLD << RED << " : " << std::strerror(errno) << RESET << std::endl, 0);
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	this->_fd = socket_fd;
	this->_index = _pfds.size()-1;
	std::cout << BOLD << GREEN << "[" << socket_fd << "] new socket Server created\n" << RESET;
	return(1);
}

void	Server::add_client_to_pollfds()
{
	int	socket_fd;

	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		throw_error("Error in add_client_to_pollfds : accept failure !");
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	Client *newClient = new Client(socket_fd, _pfds.size() - 1, *this);
	_clients[socket_fd] = newClient;
	std::cout << BOLD << GREEN << "[" << socket_fd << "] Client new connection accept!\n" << RESET;
}