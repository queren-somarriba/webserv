/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:52 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/14 11:04:34 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include "Webserv.hpp"

void	sigQuitHandler(int signal);

void	sigIntHandler(int signal);

#endif