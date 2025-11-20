/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response2.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:09:52 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:39:37 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "Response.hpp"

void	Response::generateFileName(struct stat path_stat)
{
	this->_fileName = "newFile";
	if (this->_headers.find("CONTENT-TYPE") != this->_headers.end())
		this->_fileName += getFileExt(this->_headers["CONTENT-TYPE"]);
	if (stat(this->_fileName.c_str(), &path_stat))
	{
		size_t pos = this->_fileName.rfind('.');
		this->_fileName = this->_fileName.substr(0, pos);
		this->_fileName += getTime() + getFileExt(this->_headers["CONTENT-TYPE"]);
	}
	if (this->_path[_path.size() - 1 ] != '/')
		this->_path += "/";
	this->_path += this->_fileName;
}

int	Response::expandPath(struct stat path_stat)
{
	if ((this->_autoIndex == "on" || this->_autoIndex == "ON") && this->_methode == "GET")
		return (this->autoIndex(), 1);
	if (this->_methode == "GET")
	{
		if (this->_config.find("index") == this->_config.end())
			return (setStatus("404"), 0);
		std::string str(this->_path + this->_config["index"]);
		std::ifstream ifs(str.c_str());
		if (ifs.fail())
			return (setStatus("404"), 0);
		this->_path += this->_config["index"];
		this->_fileName = this->_config["index"];
	}
	if (this->_methode == "POST")
		generateFileName(path_stat);
	return (0);
}

int Response::HandlePath()
{
	struct stat path_stat;
	int			status;
	
	std::memset(&path_stat, 0, sizeof(path_stat));
	status = stat(this->_path.c_str(), &path_stat);
	if (status && this->_methode.compare("POST"))
		return (setStatus("404"), 0);
	if ((access(this->_path.c_str(), R_OK | W_OK) && (S_ISDIR(path_stat.st_mode) || S_ISREG(path_stat.st_mode))))
		return (setStatus("403"), 0);
	if (S_ISDIR(path_stat.st_mode))
		return(expandPath(path_stat));
	else if (!status && this->_methode.compare("POST"))
	{
		std::ifstream ifs(this->_path.c_str(), std::ifstream::in);
		if (ifs.fail() || !ifs.is_open())
			return (setStatus("403"), 0);
	}
	else if (!this->_methode.compare("POST"))
	{
		std::ofstream ofs(this->_path.c_str(), std::ofstream::out);
		if (ofs.fail() || !ofs.is_open())
			return (setStatus("403"), 0);
	}
	size_t pos = this->_path.rfind("/");
	if (pos != std::string::npos)
		this->_fileName = this->_path.substr(pos + 1, this->_path.size() - pos - 1);
	return (0);
}

void Response::readFile()
{
	std::ostringstream os;
	std::ifstream file(this->_path.c_str(), std::ios::in | std::ios::binary);

	if (file.fail())
		return (setStatus("500"), setErrorPage());
	os << file.rdbuf();
	this->_responseBody = os.str();;
}

void Response::getMethode()
{
	this->readFile();
	if (this->_responseStatus == "200")
		return (setResponse());
	else
		return (setErrorPage());
}

void Response::postMethode()
{
	std::string status;
	struct stat path_stat;

	std::map<std::string, std::string> headers = Request::getHeaders();
	if (headers.find("CONTENT-TYPE") != headers.end() && !std::strncmp(headers["CONTENT-TYPE"].c_str(), "multipart/form-data", 19))
		return (postMultipart());
	if (this->_body.empty())
		return (setStatus("400"), setErrorPage());
	if (stat(this->_path.c_str(), &path_stat) != 0)
	{
		if (this->_config.find("upload_folder") == this->_config.end())
			return (setStatus("500"), setErrorPage());
		setStatus("201");
		this->_responseBody = "Resource succesfully created";
	}
	else
	{
		setStatus("200");
		this->_responseBody = "Resource succedfully upadated";
	}
	this->_responseBody += CRLF;
	std::ofstream ofs(this->_path.c_str(), std::ios::out | std::ios::binary);
	if (!ofs.is_open() || ofs.fail())
		return (setStatus("500"), setErrorPage());
	ofs.write(this->_body.data(), this->_body.size());
	ofs.close();
	setResponse();
}
 
void Response::postMultipart()
{
	std::string	field_name, tmp_path, ext, final_path;
	size_t		pos;

	if (!this->_files.empty())
	{
		std::string upload_dir = _ogRoot + this->_config["upload_folder"];
		if (upload_dir == _ogRoot)
			upload_dir += "/uploads";
		mkdir(upload_dir.c_str(), 0777);
		for (std::map<std::string, std::string>::iterator it = this->_files.begin(); it != this->_files.end(); ++it)
		{
			field_name = it->first;
			tmp_path = it->second;
			pos = it->first.find(".");
			if (pos != std::string::npos)
				ext = it->first.substr(pos);
			final_path = upload_dir + "/" + field_name.substr(0, pos) + "_" + generateRandomName(10) + ext;
			std::ifstream src(tmp_path.c_str(), std::ios::binary);
			std::ofstream dst(final_path.c_str(), std::ios::binary);
			if (!src.is_open() || !dst.is_open())
			{
				this->_responseBody += "Failed to save file: " + field_name + CRLF;
				continue;
			}
			dst << src.rdbuf();
			src.close();
			dst.close();
			this->_responseBody += "File: " + field_name + " saved to " + final_path + CRLF;
		}
	}
	else if (this->_body.empty())
		this->_responseBody += "Form saved to /tmp/form_data.csv\r\n";
	setResponse();
}

void Response::deleteMethode()
{
	if (std::remove(this->_path.c_str()))
		return (setErrorPage());
	this->_responseBody += "File: " + this->_fileName + " deleted" + CRLF;
	setResponse();
}

void Response::autoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::string path, filename, index_page = "<!DOCTYPE html>\n<html>\n<head>\n<title>Page Title</title>\n</head>\n<body>\n\n<h1>Index of ";
	path = this->_path.substr(_ogRoot.size() + 1);
	while (path[path.size() - 1] == '/')
		path = path.substr(0, path.size() - 1);
	while (this->_path[this->_path.size() - 1] == '/')
			this->_path = this->_path.substr(0, this->_path.size() - 1);
	path = "/" + path;
	if (!path.empty() && path[path.size() - 1] != '/')
        path += '/';
    index_page += path + "</h1>";
	if ((dir = opendir(this->_path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] == '.')
				continue;
			filename = ent->d_name;
			index_page += "<a href=\"";
			index_page += path + filename;
			index_page += "\">";
			index_page += filename;
			index_page += "</a><br>\n";

		}
		index_page += "</p>\n</body>\n</html>\n";
		if (closedir(dir) < 0)
			return (setStatus("500"), setErrorPage());
	}
	else
		return (setStatus("403"), setErrorPage());
	this->_fileName = "autoIndex.html";
	this->_responseBody = index_page;
	setResponse();
}


void Response::setRedirect()
{
	this->_response_msg += "HTTP/1.1 " + this->_responseStatus;
	this->_response_msg += " Found ";
	this->_response_msg += "\r\nServer: Webserv\r\n";
	this->_response_msg += "Date: " + getTimeStr() + CRLF;
	this->_response_msg += "Content-length: 0\r\n"; 
	this->_response_msg += "Location: " + this->_config["return"] + CRLFCRLF;
}

void Response::setResponse()
{
	this->_response_msg += "HTTP/1.1 " + this->_responseStatus;
	if (this->_responseStatus == "200")
		this->_response_msg += " OK";
	if (this->_responseStatus == "201")
		this->_response_msg += " Created";
	this->_response_msg += "\r\nServer: Webserv\r\n";
	this->_response_msg += "Date: " + getTimeStr() + CRLF;
	this->_response_msg += "Content-type: " + this->getContent_type() + CRLF;
	this->_response_msg += "Content-Length: " + int_to_string(this->_responseBody.size()) + CRLF;
	if (!this->_headers.empty() && this->_headers.find("COOKIE") == this->_headers.end())
		this->_response_msg += setCookies();
	this->_response_msg += CRLF + this->_responseBody;
}

void Response::callMethode()
{
	std::string methodes[3] = {"GET", "POST", "DELETE"};
	void (Response::*f[])(void) = {&Response::getMethode, &Response::postMethode, &Response::deleteMethode};
	if (this->_responseStatus == "302")
		return (clearTmpFiles(), setRedirect());
	if (this->_responseStatus != "200")
		return (clearTmpFiles(), setErrorPage());
	if (HandlePath())
		return;
	if (this->_responseStatus != "200")
		return (clearTmpFiles(), setErrorPage());
	for (int i = 0; i < 3; ++i)
		if (!methodes[i].compare(this->_methode))
			(void)((this->*f[i])());
	clearTmpFiles();
}

void Response::setErrorPage()
{
	std::ostringstream os;
	std::string target;
	if(this->_error_pages.find(this->_responseStatus) != this->_error_pages.end())
		target = this->_ogRoot + this->_error_pages[_responseStatus];
	else
		target = this->_ogRoot + "/error/" + this->_responseStatus + ".html";
	std::ifstream file(target.c_str());
	if (file.fail())
	{
		this->_responseBody = "<p style=\"text-align: center;\"><strong><span style=\"font-size: 30px;\">500 Internal Server Error</strong></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">___________________________________________________________________________________________</span></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">webserv</span></p>";
		this->_fileName = "500.html";
	}
	else
	{
		os << file.rdbuf();
		this->_responseBody = os.str();
		this->_fileName = target;
	}
	setResponse();
}
