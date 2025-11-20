# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/12 16:43:52 by jpiech            #+#    #+#              #
#    Updated: 2025/10/15 16:49:54 by qsomarri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = @c++

INCLUDES_DIR = includes

CFLAGS = -Wall -Werror -Wextra -g3 -std=c++98 -I./$(INCLUDES_DIR)

SRCS_DIR = srcs

SRCS =	$(SRCS_DIR)/main.cpp \
	$(SRCS_DIR)/Webserv.cpp \
	$(SRCS_DIR)/Webserv2.cpp \
	$(SRCS_DIR)/Server.cpp \
	$(SRCS_DIR)/Server2.cpp \
	$(SRCS_DIR)/Location.cpp \
	$(SRCS_DIR)/Client.cpp \
	$(SRCS_DIR)/Request.cpp \
	$(SRCS_DIR)/Request2.cpp \
	$(SRCS_DIR)/RequestParser.cpp \
	$(SRCS_DIR)/Response.cpp \
	$(SRCS_DIR)/Response2.cpp \
	$(SRCS_DIR)/Signals.cpp \
	$(SRCS_DIR)/Utils.cpp \
	$(SRCS_DIR)/CGI.cpp 

OBJ_DIR = obj

OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) -o $(NAME) $(CFLAGS) $(OBJS)
	@echo "\033[0;92m * $(NAME) program file created\033[0m *"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
clean:
	@$(RM) $(OBJ_DIR)
	@echo "\033[0;91m* $(NAME) object files deleted *\033[0m"

fclean: clean
	@$(RM) $(NAME)
	@echo "\033[0;91m* $(NAME) deleted *\033[0m"

re: fclean all

vlg : 
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME) ./conf/1.conf

.PHONY: all clean fclean re