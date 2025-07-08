#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../parser/minishell.h"  /* define t_token and T_* enums */
# include <stdlib.h>
# include <stdbool.h>
# include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

extern int g_last_exit_code;

typedef enum e_redir_type
{
    RD_INPUT,    // '<'
    RD_HEREDOC,  // '<<'
    RD_TRUNC,    // '>'
    RD_APPEND    // '>>'
}   t_redir_type;

typedef struct s_redir
{
    t_redir_type   type;
    char          *file;
}               t_redir;

typedef struct s_cmd
{
    char    **argv;       // NULL-terminated arglist
    t_redir *redirs;      // array of redirs
    int      n_redirs;    // count
}               t_cmd;

typedef struct s_command
{
    char    **argv;       // NULL-terminated array of strings for execve()
    int      infile_fd;   // already-opened FD (or STDIN_FILENO)
    int      outfile_fd;  // already-opened FD (or STDOUT_FILENO)
}   t_command;

t_cmd   *parse_commands(t_token *tokens, int *out_ncmds);
void     exec_commands(t_cmd *cmds, int ncmds, char **envp);
void     free_commands(t_cmd *cmds, int ncmds);
// Error handling
void    error_exit(const char *msg);

// String utilities (you can use your libft implementations)
char    **ft_split(char const *s, char c);
char    *ft_strjoin(char const *s1, char const *s2);

// Command path resolution
char    *get_cmd_path(char *cmd, char **envp);

// Builtin detection and execution
bool    is_builtin(const char *cmd);
void    exec_builtin(char **args, char **envp);

// Executor
void    executor(char **cmds, char **envp);


#endif