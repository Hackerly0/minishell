#include "../../includes/minishell.h"

t_cmd	*create_cmd(void)
{
	t_cmd *cmd = malloc(sizeof(t_cmd));
	if (!cmd) return NULL;
	cmd->argv = malloc(sizeof(char *) * 256);
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->heredoc_delim = NULL;
	cmd->append_mode = 0;
	cmd->next = NULL;
	return cmd;
}

// Free one t_cmd node
void	free_cmd(t_cmd *cmd)
{
	if (!cmd) return;
	free_array(cmd->argv);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd->heredoc_delim);
	free(cmd);
}

// Free a whole pipeline
void	free_cmd_list(t_cmd *cmd_list)
{
	while (cmd_list)
	{
		t_cmd *next = cmd_list->next;
		free_cmd(cmd_list);
		cmd_list = next;
	}
}

// Turn a token list into a linked list of t_cmd
t_cmd	*parse_tokens_to_commands(t_token *tokens)
{
	t_cmd *cmd_list = NULL, *current = NULL, *last = NULL;
	int argc = 0;

	while (tokens)
	{
		if (tokens->type == T_PIPE)
		{
			if (current)
			{
				current->argv[argc] = NULL;
				if (!cmd_list) cmd_list = current;
				else last->next = current;
				last = current;
			}
			current = create_cmd();
			argc = 0;
		}
		else if (tokens->type == T_COMMAND)
		{
			// Handle T_COMMAND tokens - path is already resolved in value
            printf("DEBUG: Command token value: '%s'\n", tokens->value);
			if (!current) current = create_cmd();
			current->argv[argc++] = strdup(tokens->value);
		}
		else if (tokens->type == T_WORD || tokens->type == T_ARGUMENT || tokens->type == T_OPTION)
		{
			// Handle other command components
			if (!current) current = create_cmd();
			current->argv[argc++] = strdup(tokens->value);
		}
		else if (tokens->type == T_IN_REDIR)
		{
			if (!current) current = create_cmd();
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
				current->input_file = strdup(tokens->value);
		}
		else if (tokens->type == T_OUT_REDIR || tokens->type == T_DOUT_REDIR)
		{
			if (!current) current = create_cmd();
			int append = (tokens->type == T_DOUT_REDIR);
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
			{
				current->output_file = strdup(tokens->value);
				current->append_mode = append;
			}
		}
		else if (tokens->type == T_HEREDOC)
		{
			if (!current) current = create_cmd();
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
				current->heredoc_delim = strdup(tokens->value);
		}
		tokens = tokens->next;
	}

	if (current)
	{
		current->argv[argc] = NULL;
		if (!cmd_list) cmd_list = current;
		else last->next = current;
	}
	return cmd_list;
}