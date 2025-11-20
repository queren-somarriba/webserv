/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:49 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:27:48 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <csignal>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "Utils.hpp"

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define PURPLE "\e[35m"
#define RED "\033[31m"
#define YELLOW "\e[33m"

class Client;
class Server;

class Webserv
{
	protected:
		static std::vector<struct pollfd>									_pfds;
		static std::map<int, Client*>										_clients;
		static std::map<int, Server*>										_servers;
		static std::map<std::string, std::map<std::string, std::string> >	_server_sessions;
		int																	_fd;
		int																	_index;
	public:
		/* Canonical Form + Paramtric constructor */
																			Webserv();
																			Webserv(std::string FileName);
																			Webserv(const Webserv& srcs);
		Webserv&															operator=(const Webserv& rhs);
		virtual																~Webserv();
		/* Member Functions */
		void																ServerMaker(std::string & Config);
		void																CheckAvailablePorts(std::string currentIP, std::vector<std::string>& tempPorts);
		std::string															ExtractConfig(std::string Filename);
		void																throw_error(const char*);
		void																runWebserv();
		void																setIndex();
		void																clean_close();
		void																erase_client();
};

#endif