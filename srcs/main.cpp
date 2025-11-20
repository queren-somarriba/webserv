/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:16 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:30:33 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"
#include "Signals.hpp"
#include "Webserv.hpp"

int	main(int argc, char *argv[])
{
	try
	{
		if(argc > 2)
			throw std::invalid_argument("Wrong number of arguments, must be 1 or 0");
		std::signal(SIGINT, sigIntHandler);
		std::signal(SIGQUIT, sigQuitHandler);
		std::string Filename;
		if (argv[1])
			Filename = argv[1];
		Webserv ws(Filename);
		ws.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr  << BOLD << RED << e.what() << RESET << '\n';
	}
	return (0);
}
