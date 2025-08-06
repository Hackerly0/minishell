#include "../../includes/helper.h"
/* Helper to detect argument tokens */  
int is_arg(int type)
{  
	return (type == T_WORD  
		|| type == T_ARGUMENT  
		|| type == T_FILE  
		|| type == T_COMMAND);  
}

/* Finalize current cmd and link into list */  
void	finalize_cmd(t_cmd *cur, t_cmd **head, t_cmd **tail, int argc)
{  
	if (!cur)  
		return ;  
	cur->argv[argc] = NULL;  
	if (!*head)  
		*head = cur;  
	else  
		(*tail)->next = cur;  
	*tail = cur;  
}

/* Handle all redirections, advance tokens ptr */  
void	process_redir(t_cmd **cur, t_token *tok, t_token **toks)
{  
	t_token	*next;  
  
	if (!*cur)  
		*cur = create_cmd();  
	next = tok->next;  
	if (tok->type == T_IN_REDIR)  
		(*cur)->input_file = strdup(next->value);  
	else if (tok->type == T_OUT_REDIR  
		|| tok->type == T_DOUT_REDIR)  
	{  
		(*cur)->append_mode  
			= (tok->type == T_DOUT_REDIR);  
		(*cur)->output_file = strdup(next->value);  
	}  
	else  
		(*cur)->heredoc_delim = strdup(next->value);  
	*toks = next->next;  
}
