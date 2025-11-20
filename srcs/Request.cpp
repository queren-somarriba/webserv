/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:19 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:39:04 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"

std::map<std::string, std::string>	Request::_files;

/*****************	CANONICAL	*******************/

Request::Request() : Client(), _headers_len(), _request_line_len(), _body_len(), _body(), _methode(), _path(),
_protocol(), _responseStatus("200"), _isCGI(), _CGI_bin_path(), _CGI_script(), _CGI_pathInfo(), _CGI_querry(), _CGIinterpret(),
_cookies(), _session_id() {}

Request::Request(const Request &src)
{
	*this = src;
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_isCGI = rhs._isCGI;
		this->_CGI_bin_path = rhs._CGI_bin_path;
		this->_CGI_script = rhs._CGI_script;
		this->_CGI_pathInfo = rhs._CGI_pathInfo;
		this->_CGI_querry = rhs._CGI_querry;
		this->_CGIinterpret = rhs._CGIinterpret;
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgis = rhs._cgis;
		this->_server_fd = rhs._server_fd;
		this->_ogRoot = rhs._ogRoot;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_methode = rhs._methode;
		this->_path = rhs._path;
		this->_protocol = rhs._protocol;
		this->_responseStatus = rhs._responseStatus;
		this->_body_len = rhs._body_len;
		this->_headers_len = rhs._headers_len;
		this->_request_line_len = rhs._request_line_len;
		this->_cookies = rhs._cookies;
		this->_session_id = rhs._session_id;
	}
	return (*this);
}

Request::Request(const Client &client) : Client(client), _headers_len(), _request_line_len(), _body_len(), _body(), _methode(),
_path(), _protocol(), _responseStatus("200"), _isCGI(), _CGI_bin_path(), _CGI_script(), _CGI_pathInfo(), _CGI_querry(), _CGIinterpret(),
_cookies(), _session_id() {}

Request::~Request() {}

/*****************	GETTERS / SETTERS		*******************/

std::string Request::getProtocol() const
{
	return (this->_protocol);
}

void Request::setRecieved(std::vector<char>& bin_vect)
{
	this->_recieved = bin_vect;
}

std::vector<char> Request::getBody() const
{
	return (this->_body);
}

std::string	Request::getPath() const
{
	return (this->_path);
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return (this->_headers);
}

size_t Request::getBodyLen() const
{
	return (this->_body_len);
}

size_t Request::getHeadersLen() const
{
	return (this->_headers_len);
}

size_t Request::getRequestLineLen() const
{
	return (this->_request_line_len);
}

void Request::setStatus(std::string const &str)
{
	if (!this->_responseStatus.empty() && this->_responseStatus == "200")
		this->_responseStatus = str;
}

bool	Request::get_isCGI() const
{
	return (this->_isCGI);
}

void	Request::set_isCGIFalse()
{
	this->_isCGI = false;
}

std::string	Request::getStatus() const
{
	return (this->_responseStatus);
}

