/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:28:12 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/16 10:28:12 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

std::string	int_to_string(int value);

size_t		findCRLFCRLF(std::string str);

size_t		findCRLF(std::string str);

size_t		find_mem(const std::vector<char>& vect, const std::string &substr);

int			hexStringToInt(std::string str);

void		strCapitalizer(std::string &str);

std::string trim_white_spaces(std::string str);

void		trim_CRLF(std::string &str);

void		removeQuotes(std::string& str);

std::string	getName(const std::string& str, const std::string& key);

std::string	generateRandomName(unsigned int len);

void		printVect(std::vector<char>&);

std::string getTime();

std::string getTimeStr();

#endif