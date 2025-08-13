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
# include <sys/stat.h>

# define Q_DQUOT 1
# define Q_SQUOT 2
# define _XOPEN_SOURCE 700

extern volatile sig_atomic_t	g_signal;

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

typedef struct s_env
{
	char			*key;
	char			*val;
	struct s_env	*next;
}					t_env;

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
	char			**argv;
	char			*input_file;
	char			*output_file;
	char			**heredoc_delims;
	int				heredoc_count;
	int				append_mode;
	struct s_cmd	*next;
	t_token			*token_list;
}					t_cmd;

typedef struct s_exec_context
{
	t_cmd	*cmd_list;
	char	**envp;
	int		last_exit_code;
	int		pipe_count;
}			t_exec_context;

typedef struct s_temp
{
	t_seg_type	**segs;
	t_token		**head;
}				t_temp;

typedef struct s_temp_pipeline
{
	int		**pipes;
	int		*heredoc_fds;
	pid_t	*pids;
	t_cmd	*cur;
	int		i;
	t_cmd	*cmd_list;
	char	**envp;
	int		n;
}			t_temp_pipeline;

typedef struct s_data
{
	int		exit_code;
	t_cmd	*cmds;
	t_env	**envp;
	t_token	**tokens;
}			t_data;

typedef struct s_temp_out
{
	char	**out;
	int		*o_len;
}			t_temp_out;

typedef struct s_temp_child
{
	int	exit_code;
	int	n;
}			t_temp_child;

/* main functions */

int				sig_and_checks(char **line, t_data *data);
int				expand_parse(t_data *data, t_token **tokens, char **line);
void			execute_and_free(t_data *data, t_token **tokens, char **line,
					t_env **env_list);
void			minishell_setup(t_data *data, t_env **env_list);

/* executor functions */

int				execute_command_line(t_token *tokens,
					t_env **envp, t_data *data);
int				init_pipeline_data(t_temp_pipeline *temp,
					t_cmd *cmd_list, int n, t_data *data);
void			error_fd_pipe(t_temp_pipeline *temp, int n);
int				pipeline_cont(t_temp_pipeline *temp, int n);
void			finalize_cmd(t_cmd *cur, t_cmd **head,
					t_cmd **tail, int argc);
void			process_redir(t_cmd **cur, t_token *tok,
					t_token **toks);
int				builtin_case(char **cmd,
					char **cmd_name, char **envp);
t_cmd			*create_cmd(void);
void			free_cmd(t_cmd *cmd);
void			free_cmd_list(t_cmd *cmd_list);
void			free_array(char **arr);
t_cmd			*parse_tokens_to_commands(t_token *tokens);
int				setup_input_redirection(t_cmd *cmd);
void			signal_handler(int sig);
int				execute_single_command(t_cmd *cmd, t_env **envp);
int				is_builtin_cmd(char *cmd_path);
int				create_heredoc_pipe(const char *delim,
					int *out_fd, t_data *data);
int				*create_heredoc_fds(t_cmd *cmd_list, int n,
					t_data *data);
int				setup_single_cmd_input(t_cmd *cmd,
					t_data *data);
int				cmd_not_found(t_cmd *cmd, char **cmd_path,
					char **envp);
void			setup_pipeline_input(t_cmd *cur, int i,
					int **pipes, int *heredoc_fds);
void			setup_pipeline_output(t_cmd *cur, int i,
					int **pipes, int n);
int				single_command_handler(t_cmd *cmd_list,
					t_env **envp, t_data *data);
int				setup_output_redirection(t_cmd *cmd);
char			*find_command_path(char *cmd, char **envp);
int				execute_pipeline(t_cmd *cmd_list,
					t_env **envp, t_data *data);
int				wait_pipeline_processes(pid_t *pids, int n);
void			setup_signals(void);
void			setup_hd_signals(void);
int				is_executable_file(const char *path);
int				count_commands(t_cmd *cmd_list);
void			error_exit(const char *msg);
void			execute_execve(char *cmd_path, t_cmd *cmd,
					char **envp);
int				non_path(char **cmd_path, char **envp);
void			child_code_pipeline(t_temp_pipeline *temp,
					t_cmd *cmd_list, t_env **envp, int n);
int				permission_denied(char *cmd_path, char **envp, char **argv);
int				hd_save_stdin(int *saved);
int				hd_abort(int saved, int wfd, char *in);
int				hd_write_line(int wfd, char *s, t_data *data);
int				hd_finish(int saved, int wfd, char *in);
void			close_all_pipes(int **pipes, int n);
int				**create_pipe_array(int n);
void			free_pipe_array(int **pipes, int n);
void			close_heredoc_fds(int *fds, int n);
void			close_other_heredoc_fds(int *fds,
					int n, int keep);
void			setup_child_signals(void);
void			free_data(t_data *data);
int				hd_restore_stdin(int saved);
int				hd_make_pipe(int pipefd[2]);
void			restore_default_signals(void);
int				status_to_code(int status);
void			setup_child_signals(void);
/* parser functions */

t_token			*tokenize(char *line);
int				ft_strcmp(const char *s1, const char *s2);
int				parser(t_data *data);
void			expand_variables(t_data *data);
t_token_type	classify_token(char *s);
void			free_tokens(t_token **tokens);
t_token			*create_token(const char *val,
					t_token_type type, int in_quotes);
void			append_token(t_token **head, t_token *new);
int				quotes_validation(char *line);
int				extract_word_tokens(char *line, int *i,
					t_token **head);
void			has_var_func(t_seg_type **segs,
					t_w_tok *w_tok);
int				while_core_code(char *line, int *i,
					t_w_tok *w_tok, t_temp *temp);
int				continue_word(t_seg_type *segs,
					t_w_tok *w_tok, t_token **head);
int				append_buf_token(t_token **head,
					char *buf, int len, int quoted);
int				variable_finder(t_seg_type **segs,
					t_w_tok *w_tok, char *line, int *i);
void			assign_quote_mask(t_w_tok *w_tok, int *i);
int				segs_filler(t_seg_type **segs,
					t_w_tok *w_tok, char *line, int *i);
void			w_tok_initializer(t_w_tok *w_tok);
t_token			*tokenize(char *line);
int				is_special_char(char c);
int				ft_isspace(int c);
char			*ft_strndup(const char *src, int size);
char			*ft_strjoin_three(char *s1, char *s2, char *s3);
int				pipe_error(t_token *cur, t_data *data);
int				print_syntax_error(char *value, t_data *data);
int				missing_file(t_data *data);
int				free_seg_type(t_seg_type **segs, int size);
void			set_out(char **out, int *o_len, char *s, int *i);
void			exit_status(char **out, int *o_len, int *i,
					t_data *data);
char			*expand_string(char *s, t_data *data);
void			assign_char(char **out, int *o_len, char c);
int				update_value(char **value, t_data *data);
char			*search_env(char *name, t_env *env_list);
int				out_realloc(char **out, int *o_len, char *val);
char			*malloc_fail(void);
int				is_dir(const char *path);

/* builtins functions */

int				builtin_echo(char **argv);
int				builtin_cd(char **arg,
					t_env **env_list);
int				builtin_pwd(char **argv);
int				builtin_env(char **argv,
					t_env **env_list);
int				builtin_exit(char **argv,
					t_env **env_list, t_cmd *cmd);
int				builtin_unset(char **argv,
					t_env **lst);
int				execute_builtin(char **argv,
					t_env **envp, t_cmd *cmd);
int				is_builtin(char *cmd);
void			error_cmd(char *cmd, char *msg);
void			error_str(char *msg);
void			error_file(char *filename, char *msg);
int				is_valid_export_id(const char *s);
int				parse_export_arg(const char *arg, char **key,
					char **val);
void			print_export_id_error(const char *arg);
t_env			*env_find(t_env *head, const char *key);
int				export_set_list(t_env **env, const char *k,
					const char *v);
int				builtin_export(char **argv, t_env **env);
char			**env_list_to_array(t_env *head);
void			free_envp(char **envp);
int				is_valid_identifier(const char *s);
int				env_set_value(t_env *n, const char *v);
t_env			*env_add_front(t_env **head, const char *k,
					const char *v);
int				is_builtin(char *cmd);
void			env_free_list(t_env *head);
t_env			*env_init(char **src);

#endif
