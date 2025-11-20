/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:25 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/23 14:16:26 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Signals.hpp"

void	sigIntHandler(int signal)
{
	if (signal == SIGINT)
	{
		Webserv ws;
		ws.throw_error("CTRL + C caught!");
	}
}

void	sigQuitHandler(int signal)
{
	if (signal == SIGQUIT)
	{
		Webserv ws;
		ws.throw_error("CTRL + \\ caught!");
	}
}