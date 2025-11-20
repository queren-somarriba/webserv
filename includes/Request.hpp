/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/14 12:22:05 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <dirent.h>
#include <stdio.h>

#define CRLFCRLF "\r\n\r\n"
#define CRLF "\r\n"

class Request : public Client
{
	protected:
		size_t										_headers_len;
		size_t										_request_line_len;
		size_t										_body_len;
		std::map<std::string, std::string>			_headers;
		static std::map<std::string, std::string>	_files;
		std::vector<char>							_body;
		std::string									_methode;
		std::string									_path;
		std::string									_protocol;
		std::string									_responseStatus;
		std::string									_ogRoot;
		bool										_isCGI;
		std::string									_CGI_bin_path;
		std::string									_CGI_script;
		std::string									_CGI_pathInfo;
		std::string									_CGI_querry;
		std::string									_CGIinterpret;
		std::map<std::string, std::string>			_cookies;
		std::string									_session_id;

	public:
		/* Canonical Form + Paramtric constructor */
													Request();
													Request(const Client &);
													Request(const Request &);
		Request										&operator=(const Request &);
		virtual										~Request();
		/* Getters */
		std::string									getPath() const;
		std::string									getProtocol() const;
		std::vector<char>							getBody() const;
		std::map<std::string, std::string>			getHeaders() const;
		size_t										getBodyLen() const;
		size_t										getHeadersLen() const;
		size_t										getRequestLineLen() const;
		std::string									getRecieved() const;
		bool										get_isCGI() const;
		std::string									getStatus() const;
		void										getCgiScript();
		/* Member Functions */
		void										parsRequest();
		void										parsRequestLine(std::vector<char>&);
		void										resolvePath();
		void										parsHeaders(std::vector<char>&);
		void										parsBody();
		int											parsChunkedBody();
		int											parsChunk(std::vector<char>&);
		void										checkRequest();
		void										setStatus(std::string const &str);
		void										setRecieved(std::vector<char>& vect_bin);
		void		 								printURIConfig();
		void										parsMultipart();
		int											parsPart(std::vector<char>&, std::string&);
		void										check_cgi();
		void										checkCGIExt();
		void										set_isCGIFalse();
		int											extractPart(std::vector<char>& msg, const std::string &bound, std::vector<char>& part, size_t &sep_pos);
		std::map<std::string, std::string>			makeHeadersMap(std::vector<char> part, size_t& sep_pos);
		int											handleContent(std::map<std::string, std::string>& headers_map, std::vector<char>& body_part);
		void 										clearTmpFiles();
		void										parsCookie();
};

#endif