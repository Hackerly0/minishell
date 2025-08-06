#ifndef HELPER_H
# define HELPER_H
#include "minishell.h"

int     is_arg(int type);
void    finalize_cmd(t_cmd *cur, t_cmd **head, t_cmd **tail, int argc);
void	process_redir(t_cmd **cur, t_token *tok, t_token **toks);

#endif