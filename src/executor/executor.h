#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../parser/minishell.h"  /* define t_token and T_* enums */
# include <unistd.h>

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

t_cmd   *parse_commands(t_token *tokens, int *out_ncmds);
void     exec_commands(t_cmd *cmds, int ncmds, char **envp);
void     free_commands(t_cmd *cmds, int ncmds);

#endif