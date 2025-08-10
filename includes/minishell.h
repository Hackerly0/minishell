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
# include <sys/wait.h>
# include <fcntl.h>

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
	T_FILE = 8
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
	// for test
	t_token			*token_list;    // List of tokens for this command
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

int				ft_strcmp(const char *s1, const char *s2);
t_token			*tokenize(char *line);
void			free_tokens(t_token **head);
int				parser(t_token **tokens);
int				quotes_validation(char *line);
void			print_tokens(t_token *tokens);
void			expand_variables(t_token **tokens);
t_token_type	classify_token(char *s);
void			free_tokens(t_token **tokens);
t_token			*create_token(const char *val, t_token_type type, int in_quotes);
void			append_token(t_token **head, t_token *new);
int				quotes_validation(char *line);
int				default_behavior(t_seg_type **segs, t_w_tok *w_tok, t_token **head);
int				tokens_gathering(t_seg_type **segs, t_w_tok *w_tok, t_token **head);
void			temp_init(t_temp *temp, t_seg_type **segs, t_token **head);
int				extract_word_tokens(char *line, int *i, t_token **head);
void			has_var_func(t_seg_type **segs, t_w_tok *w_tok);
int				while_core_code(char *line, int *i, t_w_tok *w_tok, t_temp *temp);
int				continue_while(char *line, int *i, t_temp *temp, t_w_tok *w_tok);
int				continue_word(t_seg_type *segs, t_w_tok *w_tok, t_token **head);
int				append_buf_token(t_token **head, char *buf, int len, int quoted);
int				variable_finder(t_seg_type **segs, t_w_tok *w_tok, char *line, int *i);
void			assign_quote_mask(t_w_tok *w_tok, int *i);
int				segs_filler(t_seg_type **segs, t_w_tok *w_tok, char *line, int *i);
void			w_tok_initializer(t_w_tok *w_tok);
void			increment(int *j, int *len, char *line);
t_token			*extract_operator_token(char *line, int *i);
int				token_cases(char *line, t_token **head, int *i);
t_token			*tokenize(char *line);
int				is_special_char(char c);
int				ft_isspace(int c);
char			*ft_strndup(const char *src, int size);
char			*ft_strjoin_three(char *s1, char *s2, char *s3);
int				pipe_error(t_token *cur);
int				print_syntax_error(char *value);
int				missig_file(void);
int				free_seg_type(t_seg_type **segs, int size);
char			*malloc_fail(void);
void			set_out(char **out, int *o_len, char *s, int *i);
void			exit_status(char **out, int *o_len, int *i);
char			*expand_string(char *s);


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

void		error_cmd(char *cmd, char *msg);
void		error_str(char *msg);
void		error_file(char *filename, char *msg);
t_cmd		*create_cmd(void);
void		free_cmd(t_cmd *cmd);
void		free_cmd_list(t_cmd *cmd_list);
void		free_array(char **arr);
t_cmd		*parse_tokens_to_commands(t_token *tokens);
int			setup_input_redirection(t_cmd *cmd);
int			setup_output_redirection(t_cmd *cmd);
void		signal_handler(int sig);
void		setup_signals(void);

/* executor.c - existing functions that need to be declared */
int		execute_single_command(t_cmd *cmd, char **envp);
int		is_builtin_cmd(char *cmd_path);
int		count_commands(t_cmd *cmd_list);

/* heredoc.c */
int		create_heredoc_pipe(const char *delim, int *out_fd);
//int		create_heredoc_pipe(const char *delim, int *out_fd, t_cmd *cmd);
int		*create_heredoc_fds(t_cmd *cmd_list, int n);
int		setup_single_cmd_input(t_cmd *cmd);

/* pipeline_utils.c */
void	setup_pipeline_input(t_cmd *cur, int i, int **pipes, int *heredoc_fds);
void	setup_pipeline_output(t_cmd *cur, int i, int **pipes, int n);
void	close_all_pipes(int **pipes, int n);
int		**create_pipe_array(int n);
void	free_pipe_array(int **pipes, int n);
void    close_heredoc_fds(int *fds, int n);
void    close_other_heredoc_fds(int *fds, int n, int keep);

/* single_cmd.c */
int		single_command_handler(t_cmd *cmd_list, char **envp);

/* redir_utils.c - existing functions */
int		setup_input_redirection(t_cmd *cmd);
int		setup_output_redirection(t_cmd *cmd);

/* builtins.c - existing functions */
int		is_builtin(char *cmd);
int		execute_builtin(char **argv, char **envp);

/* path_utils.c */
char	*find_command_path(char *cmd, char **envp);
void	free_array(char **arr);

/* executor.c */
int		execute_pipeline(t_cmd *cmd_list, char **envp);
int		wait_pipeline_processes(pid_t *pids, int n);
void	error_exit(const char *msg);


void	env_free_all(char ***envp);

void setup_signals(void);
void setup_hd_signals(void);

#endif
