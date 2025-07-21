/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 14:41:23 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:58:44 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../Libft/libft.h"
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <signal.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# define Q_DQUOT 1
# define Q_SQUOT 2

extern int g_signal;

typedef enum e_token_type
{
	T_WORD = 0,
	T_PIPE = 1,
	T_HEREDOC = 2,
	T_IN_REDIR = 3,
	T_DOUT_REDIR = 4,
	T_OUT_REDIR = 5,
	T_COMMAND = 6,
	T_ARGUMENT = 7,
	T_OPTION = 8
}	t_token_type;

typedef struct s_seg_type
{
	char	*txt;
	int		len;
	int		q;
}			t_seg_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	int				in_quotes;
	struct s_token	*next;
}					t_token;

typedef struct s_w_tok
{
	int		seg_cnt;
	int		in_quote;
	int		mask;
	int		has_assign;
	char	c;
	int		start;
	int		len;
	int		q;
	int		total_len;
	int		j;
	char	*buf;
	int		buf_len;
	int		x;
	int		cur_q;
	int		has_var;
	int		flag;
}			t_w_tok;

typedef struct s_cmd
{
	char			**argv;         // Command arguments
	char			*input_file;    // Input redirection file
	char			*output_file;   // Output redirection file
	char			*heredoc_delim; // Heredoc delimiter
	int				append_mode;    // Append mode for output
	struct s_cmd	*next;          // Next command in pipeline
}					t_cmd;

// Execution context
typedef struct s_exec_context
{
	t_cmd	*cmd_list;      // List of commands to execute
	char	**envp;         // Environment variables
	int		last_exit_code; // Last command exit code
	int		pipe_count;     // Number of pipes in command
}			t_exec_context;

typedef struct s_temp
{
	t_seg_type	**segs;
	t_token		**head;
}				t_temp;

int		ft_strcmp(char *s1, char *s2);
t_token	*tokenize(char *line);
void	free_tokens(t_token **head);
int		parser(t_token **tokens, char **envp);
int		quotes_validation(char *line);
void	print_tokens(t_token *tokens);
void	expand_variables(t_token *tokens);

// ─── Executor Functions ─────────────────────────────────────────────────────

int		execute_command_line(t_token *tokens, char **envp);
void	setup_signals(void);

// ─── Built-in Commands ──────────────────────────────────────────────────────

int		builtin_echo(char **argv);
int		builtin_cd(char **argv);
int		builtin_pwd(char **argv);
int		builtin_env(char **argv, char **envp);
int		builtin_exit(char **argv);
int		builtin_export(char **argv, char ***envp);
int		builtin_unset(char **argv);
int		execute_builtin(char **argv, char **envp);

// ─── Utility Functions ──────────────────────────────────────────────────────

char	*find_command_path(char *cmd, char **envp);
int		is_builtin(char *cmd);


void	error_cmd(char *cmd, char *msg);
void	error_str(char *msg);
void	error_file(char *filename, char *msg);

#endif