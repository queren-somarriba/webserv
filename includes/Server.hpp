/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:21:13 by jpiech            #+#    #+#             */
/*   Updated: 2025/10/01 10:12:50 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class Client;
class Location;

class Server : public Webserv
{
	private: 
		std::map<std::string, Location*>	_locations;	
	protected:
		std::map<std::string, std::string>	_config;
		std::map<std::string, std::string>	_error_pages;
		std::map<std::string, std::string>	_cgis;
	public :
		/* Canonical Form + Paramtric constructor */
											Server();
											Server(std::string & Config);
											Server(const Server & srcs);
		Server &							operator=(const Server & rhs);
		virtual								~Server(); 
		/* Member Functions */
		void								ExtractBloc(std::string & Config, size_t it);
		void								CheckBeforeBracket(std::string Config, size_t & i);
		std::string							GetConfigKey(std::string Config, size_t & i);
		void								CheckDirective(std::string &key, bool mod);
		std::string							GetConfigValue(std::string Config, size_t & i);
		void								GetCGIConfig(std::string value);
		void								GetErrorPageConfig(std::string value);
		int									make_listening_socket();
		void								add_client_to_pollfds();
		/* Getters & setters*/
		std::map<std::string, std::string>	getConfig();
		std::map<std::string, std::string>	getCgi();
		std::map<std::string, std::string>	getErrPage();
		std::map<std::string, Location*>	getLocations();
		void								setPort(std::string port);
		void								setIP(std::string IP);
		void								printconfig();
};

#endif