/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:27:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/14 16:51:09 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::string	int_to_string(int value)
{
	std::ostringstream	oss;
	oss << value;
	return (oss.str());
}

size_t	findCRLFCRLF(std::string str)
{
	size_t	i = -1;
	while(str[++i])
	{
		if (str[i] == '\r' && str[i + 1] && str[i + 1] == '\n'
				&& str[i + 2] && str[i + 2] == '\r' && str[i + 3] && str[i + 3] == '\n')
			return (i);
	}
	return (std::string::npos);
}

size_t	findCRLF(std::string str)
{
	size_t	i = -1;
	while(str[++i])
	{
		if (str[i] == '\r' && str[i + 1] && str[i + 1] == '\n')
			return (i);
	}
	return (std::string::npos);
}

size_t	find_mem(const std::vector<char>& vect, const std::string& str)
{
	size_t vect_len = vect.size();
	size_t str_len = str.size();
	
	if (vect.empty() || str.empty() || vect_len < str_len)
		return (std::string::npos);
	for (size_t i = 0; i <= vect_len - str_len; ++i)
	{
		if (std::memcmp(&vect.data()[i], str.data(), str_len) == 0)
			return (i);
	}
	return (std::string::npos);
}

int	hexStringToInt(std::string str)
{
	int	res;
	std::istringstream(str) >> std::hex >> res;
	return (res);
}

void strCapitalizer(std::string &str)
{
	size_t i = -1;
	while (str[++i])
		str[i] = static_cast<char>(std::toupper(str[i]));
}

std::string trim_white_spaces(std::string str)
{
	std::istringstream	ss(str);
	std::string	res, tmp;

	while (ss >> tmp)
		res += tmp + " ";
	return (res.substr(0, res.size() - 1));
}

void	trim_CRLF(std::string &str)
{
	size_t	end = str.size() - 1;

	while (end - 1 > 0 && str[end] == '\n' && str[end - 1] == '\r')
	{
		str = str.substr(0, str.size() - 2);
		end = str.size() - 1;
	}
}

void removeQuotes(std::string& str)
{
	if (str.size() >= 2 && ((str[0] == '"' && str[str.size() - 1] == '"')
		|| (str[0] == '\'' && str[str.size() - 1] == '\'')))
		str = str.substr(1, str.size() - 2);
}

std::string getName(const std::string& str, const std::string& key)
{
	size_t	pos = str.find(key);

	if (pos == std::string::npos)
		return ("");
	std::string	name = str.substr(pos + key.size());
	size_t	sep = name.find(';');
	if (sep != std::string::npos)
		name = name.substr(0, sep);
	removeQuotes(name);
	return (name);
}

std::string generateRandomName(unsigned int len)
{
	std::string res, chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::srand(time(0));
	for (size_t i = 0; i < len; ++i)
		res += chars[std::rand() % chars.size()];
	return (res);
}

void	printVect(std::vector<char>& v)
{
	for(std::vector<char>::iterator it = v.begin(); it != v.end(); ++it)
		std::cout << *it;
	std::cout << std::endl;
}

std::string	getTime()
{
    std::time_t now = std::time(NULL);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;

	oss << (t->tm_year + 1900)
		<< (t->tm_mon + 1 < 10 ? "0" : "") << (t->tm_mon + 1)
		<< (t->tm_mday < 10 ? "0" : "") << t->tm_mday << "_"
		<< (t->tm_hour < 10 ? "0" : "") << t->tm_hour
		<< (t->tm_min < 10 ? "0" : "") << t->tm_min
		<< (t->tm_sec < 10 ? "0" : "") << t->tm_sec;
    return (oss.str());
}

std::string getTimeStr()
{
	time_t rawtime;

	std::time(&rawtime);
	std::tm *gmt = std::gmtime(&rawtime);
	std::string str = std::asctime(gmt);
	return (str.erase(str.find_last_not_of("\n") + 1) + " GMT");
}