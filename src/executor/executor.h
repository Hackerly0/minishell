// #ifndef EXECUTOR_H
// # define EXECUTOR_H

// # include "../parser/minishell.h"  /* define t_token and T_* enums */
// # include <stdlib.h>
// # include <stdbool.h>
// # include <stddef.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <signal.h>
// #include <errno.h>
// #include <sys/wait.h>
// #include <fcntl.h>

// extern int g_last_exit_code;

// typedef enum e_redir_type
// {
//     RD_INPUT,    // '<'
//     RD_HEREDOC,  // '<<'
//     RD_TRUNC,    // '>'
//     RD_APPEND    // '>>'
// }   t_redir_type;

// typedef struct s_redir
// {
//     t_redir_type   type;
//     char          *file;
// }               t_redir;

// typedef struct s_cmd
// {
//     char    **argv;       // NULL-terminated arglist
//     t_redir *redirs;      // array of redirs
//     int      n_redirs;    // count
// }               t_cmd;

// typedef struct s_command
// {
//     char    **argv;       // NULL-terminated array of strings for execve()
//     int      infile_fd;   // already-opened FD (or STDIN_FILENO)
//     int      outfile_fd;  // already-opened FD (or STDOUT_FILENO)
// }   t_command;

// t_cmd   *parse_commands(t_token *tokens, int *out_ncmds);
// void     exec_commands(t_cmd *cmds, int ncmds, char **envp);
// void     free_commands(t_cmd *cmds, int ncmds);
// // Error handling
// void    error_exit(const char *msg);

// // String utilities (you can use your libft implementations)
// char    **ft_split(char const *s, char c);
// char    *ft_strjoin(char const *s1, char const *s2);

// // Command path resolution
// char    *get_cmd_path(char *cmd, char **envp);

// // Builtin detection and execution
// bool    is_builtin(const char *cmd);
// void    exec_builtin(char **args, char **envp);

// // Executor
// void    executor(char **cmds, char **envp);


// #endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 14:41:23 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/11 00:00:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>

// Global signal variable
extern volatile sig_atomic_t g_signal;

// Token types
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_IN_REDIR,
	T_OUT_REDIR,
	T_DOUT_REDIR,
	T_HEREDOC,
	T_UNKNOWN
}	t_token_type;

// Token structure
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	int				in_quotes;
	struct s_token	*next;
}					t_token;

// Command structure for execution
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

// ─── Tokenizer Functions ───────────────────────────────────────────────────

t_token	*tokenize(const char *line);
void	expand_variables(t_token *tokens);
void	print_tokens(t_token *tokens);
void	free_tokens(t_token *tokens);

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
int		builtin_unset(char **argv, char ***envp);

// ─── Utility Functions ──────────────────────────────────────────────────────

void	error_str(char *msg);
char	*find_command_path(char *cmd, char **envp);
int		is_builtin(char *cmd);

#endif