#include "../../includes/minishell.h"
#include "../../includes/helper.h"

t_cmd	*parse_error(const char *msg)
{
	fprintf(stderr, "parse error: %s\n", msg);
	return (NULL);
}

t_cmd	*create_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = malloc(sizeof(char *) * 256);
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->heredoc_delim = NULL;
	cmd->append_mode = 0;
	cmd->next = NULL;
	return (cmd);
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_array(cmd->argv);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd->heredoc_delim);
	free(cmd);
}

void	free_cmd_list(t_cmd *cmd_list)
{
	t_cmd	*next;

	while (cmd_list)
	{
		next = cmd_list->next;
		free_cmd(cmd_list);
		cmd_list = next;
	}
}

t_cmd	*parse_tokens_to_commands(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*cur;
	int		argc;

	head = NULL;
	tail = NULL;
	cur = NULL;
	argc = 0;
	while (tokens)
	{
		if (tokens->type == T_PIPE)
		{
			finalize_cmd(cur, &head, &tail, argc);
			cur = create_cmd();
			argc = 0;
		}
		else if (tokens->type == T_IN_REDIR
			|| tokens->type == T_OUT_REDIR
			|| tokens->type == T_DOUT_REDIR
			|| tokens->type == T_HEREDOC)
		{
			process_redir(&cur, tokens, &tokens);
			continue ;
		}
		else if (is_arg(tokens->type))
		{
			if (!cur)
				cur = create_cmd();
			cur->argv[argc++] = strdup(tokens->value);
		}
		tokens = tokens->next;
	}
	finalize_cmd(cur, &head, &tail, argc);
	return (head);
}
