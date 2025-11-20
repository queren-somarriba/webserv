/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:06 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:51:09 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"

std::map<std::string, std::string> makeTypesMap();
std::map<std::string, std::string> Response::_types = makeTypesMap();

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Response::Response() : Request(), _response_msg(), _fileName(), _responseBody(), _autoIndex() {}

Response::Response(const Response &src)
{
	*this = src;
}

Response &Response::operator=(const Response &rhs)
{
	if (this != &rhs)
	{
		this->_autoIndex = rhs._autoIndex;
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgis = rhs._cgis;
		this->_server_fd = rhs._server_fd;
		this->_response_msg = rhs._response_msg;
		this->_fileName = rhs._fileName;
		this->_responseBody = rhs._responseBody;
	}
	return (*this);
}

Response::Response(Request &request) : Request(request), _response_msg(), _fileName(), _responseBody()
{
	if(this->_config.find("autoindex") != this->_config.end())
		this->_autoIndex = this->_config["autoindex"];
	else
		this->_autoIndex = "off";
	if(this->_config.find("return") != this->_config.end())
		this->setStatus("302");
}

Response::~Response() {}

/*****************	CLASS UTILS	*******************/

std::string Response::getPath() const
{
	return (this->_path);
}

std::string Response::getResponseMsg() const
{
	return (this->_response_msg);
}

std::map<std::string, std::string> makeTypesMap()
{
	std::map<std::string, std::string> types;

	types[".aac"] = "audio/aac";
	types[".abw"] = "application/x-abiword";
	types[".apng"] = "image/apng";
	types[".arc"] = "application/x-freearc";
	types[".avif"] = "image/avif";
	types[".avi"] = "video/x-msvideo";
	types[".azw"] = "application/vnd.amazon.ebook";
	types[".bin"] = "application/octet-stream";
	types[".bmp"] = "image/bmp";
	types[".bz"] = "application/x-bzip";
	types[".bz2"] = "application/x-bzip2";
	types[".cda"] = "application/x-cdf";
	types[".csh"] = "application/x-csh";
	types[".css"] = "text/css";
	types[".csv"] = "text/csv";
	types[".doc"] = "application/msword";
	types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	types[".eot"] = "application/vnd.ms-fontobject";
	types[".epub"] = "application/epub+zip";
	types[".gz"] = "application/gzip";
	types[".gif"] = "image/gif";
	types[".htm"] = "text/html";
	types[".html"] = "text/html";
	types[".ico"] = "image/vnd.microsoft.icon";
	types[".ics"] = "text/calendar";
	types[".jar"] = "application/java-archive";
	types[".jpeg"] = "image/jpeg";
	types[".jpg"] = "image/jpeg";
	types[".js"] = "text/javascript";
	types[".json"] = "application/json";
	types[".jsonld"] = "application/ld+json";
	types[".md"] = "text/markdown";
	types[".mid"] = "audio/midi";
	types[".midi"] = "audio/midi";
	types[".mjs"] = "text/javascript";
	types[".mp3"] = "audio/mpeg";
	types[".mp4"] = "video/mp4";
	types[".mpeg"] = "video/mpeg";
	types[".mpkg"] = "application/vnd.apple.installer+xml";
	types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	types[".odt"] = "application/vnd.oasis.opendocument.text";
	types[".oga"] = "audio/ogg";
	types[".ogv"] = "video/ogg";
	types[".ogx"] = "application/ogg";
	types[".opus"] = "audio/ogg";
	types[".otf"] = "font/otf";
	types[".png"] = "image/png";
	types[".pdf"] = "application/pdf";
	types[".php"] = "application/x-httpd-php";
	types[".ppt"] = "application/vnd.ms-powerpoint";
	types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	types[".rar"] = "application/vnd.rar";
	types[".rtf"] = "application/rtf";
	types[".sh"] = "application/x-sh";
	types[".svg"] = "image/svg+xml";
	types[".tar"] = "application/x-tar";
	types[".tif"] = "image/tiff";
	types[".tiff"] = "image/tiff";
	types[".ts"] = "video/mp2t";
	types[".ttf"] = "font/ttf";
	types[".txt"] = "text/plain";
	types[".vsd"] = "application/vnd.visio";
	types[".wav"] = "audio/wav";
	types[".weba"] = "audio/webm";
	types[".webm"] = "video/webm";
	types[".webmanifest"] = "application/manifest+json";
	types[".webp"] = "image/webp";
	types[".woff"] = "font/woff";
	types[".woff2"] = "font/woff2";
	types[".xhtml"] = "application/xhtml+xml";
	types[".xls"] = "application/vnd.ms-excel";
	types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	types[".xml"] = "application/xml";
	types[".xul"] = "application/vnd.mozilla.xul+xml";
	types[".zip"] = "application/zip";
	types[".3gp"] = "video/3gpp";
	types[".3g2"] = "video/3gpp2";
	types[".7z"] = "application/x-7z-compressed";
	return types;
}

/*****************	GETTERS		*******************/

std::string Response::getFileExt(std::string value) const
{
	for (std::map<std::string, std::string>::const_iterator it = _types.begin(); it != _types.end(); ++it)
		if (it->second == value)
			return (it->first);
	return ("");
}

std::string Response::getFileType()
{
	size_t pos = this->_fileName.rfind(".");

	if (pos != std::string::npos)
		return (this->_fileName.substr(pos, this->_fileName.size() - pos));
	return ("");
}

std::string Response::getContent_type()
{
	if (_types.find(this->getFileType()) != _types.end())
		return (_types[this->getFileType()]);
	return ("application/octet-stream");
}

void	Response::setResponseMsg(std::string output)
{
	this->_response_msg = output;	
}

std::string	Response::setCookies() const
{
	std::string res("Set-Cookie: ");

	res += "session_id_" + getTime() + "=";
	res += "value_" + generateRandomName(10) + ";";
	res += " Max-Ages=34560000;";
	res += "\r\n";
	return (res);
}