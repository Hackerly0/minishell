# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/11 00:00:00 by oalhoora          #+#    #+#              #
#    Updated: 2025/07/19 19:51:40 by oalhoora         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g3
LDFLAGS = -lreadline

# Source files
SRCS = src/main.c \
       src/parser/tokenizer.c \
	   src/parser/parser.c \
	   src/parser/expander.c \
       src/executor/executor.c \
       src/builtins/builtins.c \
	   src/builtins/errors.c \
	   src/utils/helper_functions.c

# Object files
OBJS = $(SRCS:.c=.o)

# Libft
LIBFT_DIR = Libft
LIBFT = $(LIBFT_DIR)/libft.a

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
RESET = \033[0m

# Rules
all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@echo "$(BLUE)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(RESET)"

$(LIBFT):
	@echo "$(BLUE)Compiling libft...$(RESET)"
	@make -C $(LIBFT_DIR) --no-print-directory
	@echo "$(GREEN)✓ libft compiled!$(RESET)"

%.o: %.c minishell.h
	@echo "$(BLUE)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -f $(OBJS)
	@make -C $(LIBFT_DIR) clean --no-print-directory
	@echo "$(GREEN)✓ Object files cleaned!$(RESET)"

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean --no-print-directory
	@echo "$(GREEN)✓ $(NAME) cleaned!$(RESET)"

re: fclean all

# Testing rules
test: $(NAME)
	@echo "$(BLUE)Running basic tests...$(RESET)"
	@echo "echo hello world" | ./$(NAME)
	@echo "pwd" | ./$(NAME)
	@echo "ls -la | grep minishell" | ./$(NAME)

# Debugging
debug: CFLAGS += -g3 -fsanitize=address
debug: $(NAME)

# Memory leak check
valgrind: $(NAME)
	@echo "$(BLUE)Running valgrind memory check...$(RESET)"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

# Help
help:
	@echo "$(BLUE)Available targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)     - Build the project"
	@echo "  $(GREEN)clean$(RESET)   - Remove object files"
	@echo "  $(GREEN)fclean$(RESET)  - Remove object files and executable"
	@echo "  $(GREEN)re$(RESET)      - Rebuild the project"
	@echo "  $(GREEN)test$(RESET)    - Run basic tests"
	@echo "  $(GREEN)debug$(RESET)   - Build with debug flags"
	@echo "  $(GREEN)valgrind$(RESET) - Run with valgrind"
	@echo "  $(GREEN)help$(RESET)    - Show this help"

.PHONY: all clean fclean re test debug valgrind help