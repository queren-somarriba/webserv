/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 18:44:14 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void Request::parsRequest()
{
	std::string key, value, line;
	parsRequestLine(this->_recieved);
	resolvePath();
	parsHeaders(this->_recieved);
	checkRequest();
	check_cgi();
}

void Request::parsRequestLine(std::vector<char>& rcv)
{
	rcv.push_back('\0');
	std::string			line, tmp, msg(rcv.data());
	std::istringstream	ss(msg);

	std::getline(ss, line, '\n');
	this->_request_line_len += line.size() + 1;
	ss.str(line);
	ss >> this->_methode >> this->_path >> this->_protocol >> tmp;
	if (this->_methode.empty() || this->_path.empty() || this->_protocol.empty())
		return (setStatus("400"));
	if (this->_protocol.compare("HTTP/1.1"))
		return (setStatus("505"));
	if (this->_path[0] != '/' || (this->_path[1] && this->_path[0] == '/' && this->_path[1] == '/'))
		return (setStatus("400"));
	if (this->_path.size() >= 4000)
		return (setStatus("414"));
	rcv.pop_back();
	rcv.erase(rcv.begin(), rcv.begin() + line.size() + 1);
	if (!tmp.empty())
		return (setStatus("400"));
}

void Request::parsHeaders(std::vector<char>& rcv)
{
	rcv.push_back('\0');
	std::istringstream ss(rcv.data());
	std::string key, value, line;
	std::size_t count = 0, found = std::string::npos;

	line = ss.str();
	std::getline(ss, line, '\n');
	while (!line.empty())
	{
		count += line.size() + 1;
		if (count >= 4000)
			setStatus("431");
		line = trim_white_spaces(line);
		if (line.empty())
			break;
		found = line.find(':');
		if (!line.empty() && found == std::string::npos)
			return (setStatus("400"));
		key = trim_white_spaces(line.substr(0, found));
		strCapitalizer(key);
		value = trim_white_spaces(line.substr(found + 1));
		if (this->_headers.find(key) != this->_headers.end())
			this->_headers[key] += " " + value;
		else
			this->_headers[key] = value;
		std::getline(ss, line, '\n');
	}
	rcv.pop_back();
	if (count + 1 <= ss.str().size() && (this->_headers.find("TRANSFER-ENCODING") == this->_headers.end() || this->_headers["TRANSFER-ENCODING"] != "chunked"))
		rcv.erase(rcv.begin(), rcv.begin() + count);
	this->_headers_len += count;
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
	if (this->_config.find("client_max_body_size") != this->_config.end() && this->_config["client_max_body_size"] != "0")
		if (this->_body_len > static_cast<size_t>(atoi(this->_config["client_max_body_size"].c_str())))
			return (setStatus("413"));
	parsCookie();
}

void Request::parsBody()
{
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
		if (this->_recieved.back() == '\n')
			this->_recieved.pop_back();
		if (this->_recieved.back() == '\r')
			this->_recieved.pop_back();
		this->_body = this->_recieved;
	}
}

int	Request::parsChunk(std::vector<char>& msg)
{
	size_t	pos, chunk_len;
	char* endpos;

	pos = find_mem(msg, CRLF);
	if (pos == std::string::npos)
		return (1);
	chunk_len = std::strtol(msg.data(), &endpos, 16);
	if (chunk_len > msg.size())
		return (1);
	if (!chunk_len)
	{
		msg.clear();
		return (0);
	}
	if (pos + 2 + chunk_len > msg.size())
		return (1);
	std::vector<char> chunk(msg.begin() + pos + 2, msg.begin() + pos + 2 + chunk_len);
	this->_body.insert(this->_body.end(), chunk.begin(), chunk.end());
	if (msg.size() >= pos + chunk_len + 4)
		msg.erase(msg.begin(), msg.begin() + pos + chunk_len + 4);
	return (1);
}

int	Request::parsChunkedBody()
{
	size_t pos = find_mem(this->_recieved, CRLFCRLF);
	if (pos == std::string::npos)
		return (0);
	this->_recieved.erase(this->_recieved.begin(), this->_recieved.begin() + pos + 4);
	while (1)
	{
		if (!parsChunk(this->_recieved))
			return (0);
	}
	return (0);
}

int	Request::extractPart(std::vector<char>& msg, const std::string &bound, std::vector<char>& part, size_t &sep_pos)
{
	size_t pos = find_mem(msg, bound);
	if (pos == std::string::npos)
		return(setStatus("400"), 1);
	part = std::vector<char>(msg.begin() + pos + bound.size(), msg.end());
	if (part.size() >= 2 && part[0] == '-' && part[1] == '-')
		return (1);
	if (part.size() >= 2 && part.data()[0] == '\r' && part.data()[1] == '\n')
	{
		pos = find_mem(part, CRLF);
		if (pos == std::string::npos)
			return(setStatus("400"), 1);
		part.erase(part.begin(), part.begin() + pos + 2);
	}
	sep_pos = find_mem(part, bound);
	if (sep_pos == std::string::npos)
		return (setStatus("400"), 1);
	part.erase(part.begin() + sep_pos, part.end());
	if (part.size() + bound.size() < msg.size())
		msg.erase(msg.begin(), msg.begin() + part.size() + 1 + bound.size());
	else
		msg.clear();
	return (0);
}

std::map<std::string, std::string>	Request::makeHeadersMap(std::vector<char> part, size_t& sep_pos)
{
	std::map<std::string, std::string> headers_map;
	std::string header_line;

	sep_pos = find_mem(part, CRLFCRLF);
	if (sep_pos == std::string::npos)
		return (setStatus("400"), std::map<std::string, std::string>());
	part.erase(part.begin() + sep_pos, part.end());;
	part.push_back('\0');
	std::istringstream headers_ss(part.data());
	while (std::getline(headers_ss, header_line))
	{
		header_line = trim_white_spaces(header_line);
		if (header_line.empty())
			break;
		size_t colon = header_line.find(':');
		if (colon == std::string::npos)
			return (setStatus("400"), std::map<std::string, std::string>());
		std::string key = trim_white_spaces(header_line.substr(0, colon));
		strCapitalizer(key);
		std::string value = trim_white_spaces(header_line.substr(colon + 1));
		headers_map[key] = value;
	}
	return (headers_map);
}

int	Request::handleContent(std::map<std::string, std::string>& headers_map, std::vector<char>& body_part)
{
	std::string name = getName(headers_map["CONTENT-DISPOSITION"], "name=");
	std::string filename = getName(headers_map["CONTENT-DISPOSITION"], "filename=");
	if (!filename.empty())
	{
		std::string tmp_path = "/tmp/upload_tempfile_" + filename + generateRandomName(10);
		std::ofstream file(tmp_path.c_str(), std::ios::binary);
		if (!file.is_open() || file.fail())
			return (setStatus("500"), 1);
		file.write(body_part.data(), body_part.size());
		file.close();
		this->_files[name] = tmp_path;
	}
	if (name.empty())
		return (setStatus("400"), 1);
	else
	{
		body_part.push_back('\0');
		std::string body_str(body_part.data());
		std::string tmp_path = "/tmp/form_data.csv";
		std::ofstream csv(tmp_path.c_str(), std::ios::app);
		if (!csv.is_open())
			return (setStatus("500"), 1);
		body_str = trim_white_spaces(body_str);
		if (body_str.find(',') != std::string::npos)
			body_str = "\"" + body_str + "\"";
		csv << name << "," << body_str << "\n";
		csv.close();
	}
	return (0);
}

int Request::parsPart(std::vector<char>& msg, std::string& bound)
{
	std::vector<char> part;
	size_t	sep_pos;

	if (extractPart(msg, bound, part, sep_pos))
		return (1);
	std::map<std::string, std::string> headers_map = makeHeadersMap(part, sep_pos);
	if (headers_map.find("CONTENT-DISPOSITION") == headers_map.end())
		return (setStatus("400"), 1);
	sep_pos = find_mem(part, CRLFCRLF);
	if (sep_pos == std::string::npos)
		return(setStatus("400"), 1);
	part.erase(part.begin(), part.begin() + sep_pos + 4);
	if (handleContent(headers_map, part))
		return (1);
	return (0);
}

void	Request::parsMultipart()
{
	std::string	bound;
	size_t pos = this->_headers["CONTENT-TYPE"].find("boundary=");
	bound = this->_headers["CONTENT-TYPE"].substr(pos + 9);
	removeQuotes(bound);
	bound = "--" + bound;
	pos = find_mem(this->_recieved, CRLFCRLF);
	if (pos == std::string::npos)
		return;
	while (pos != std::string::npos)
	{
		if (parsPart(this->_recieved, bound))
			break;
		pos = find_mem(this->_recieved, bound);
	}
	this->_body_len = this->_count - this->_request_line_len - this->_headers_len;
}

void Request::parsCookie()
{
	if (_headers.find("COOKIE") == _headers.end())
		return;
	std::string cookie_header = _headers["COOKIE"];
	std::istringstream ss(cookie_header);
	std::string token, key, value;
	size_t pos;
	while (std::getline(ss, token, ';'))
	{
		pos = token.find('=');
		if (pos == std::string::npos)
			continue;
		key = trim_white_spaces(token.substr(0, pos));
		value = trim_white_spaces(token.substr(pos + 1));
		this->_cookies[key] = value;
	}
	this->_session_id = this->_cookies.begin()->first;
	if (_server_sessions.find(this->_session_id) != _server_sessions.end())
	{
		std::cout << BOLD << CYAN << "Welcome back " << RESET << GREEN << _server_sessions.find(this->_session_id)->first;
		std::cout << BOLD << CYAN << " here is your cookie: " << GREEN << _server_sessions[this->_session_id][this->_cookies.begin()->first] << RESET << std::endl;
	}
	for(std::map<std::string, std::string>::iterator it = _cookies.begin(); it != _cookies.end(); ++it)
		_server_sessions[this->_session_id][it->first] = it->second;
}