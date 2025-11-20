/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:55 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/14 11:03:59 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <fcntl.h>
#include <limits>
#include "Request.hpp"
#include "Utils.hpp"

class Response : public Request
{
	private:
		std::string									_response_msg;
		std::string									_fileName;
		std::string									_responseBody;
		static std::map<std::string, std::string>	_types;
		std::string									_autoIndex;
	public:
		/* Canonical Form + Paramtric constructor */
													Response();
													Response(Request &);
													Response(const Response &);
		Response									&operator=(const Response &);
		virtual										~Response();
		/* Member Functions */
		void										getMethode();
		void										postMethode();
		void										deleteMethode();
		void										autoIndex();
		void										readFile();
		std::string 								getContent_type();
		std::string									getFileType();
		int											HandlePath();
		void										callMethode();
		std::string									getFileExt(std::string value) const;
		void										setResponse();
		void 										setRedirect();
		void										setErrorPage();
		std::string									getResponseMsg() const;
		std::string									getPath() const;
		int											expandPath(struct stat);
		void										generateFileName(struct stat);
		void										setResponseMsg(std::string output);
		void 										postMultipart();
		std::string									setCookies() const;
};

#endif