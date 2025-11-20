/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/10/14 10:58:57 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Server.hpp"

class Location : public Server
{
	private: 
		std::string		_location_name;
		
	public:
		/* Canonical Form + Paramtric constructor */
						Location();
						Location(const Location &);
						Location(std::string name);
		Location		&operator=(const Location &);
		virtual			~Location();
		/* Member Functions */
		void			ExtractLocBloc(std::string & Config, size_t & it);
};

#endif