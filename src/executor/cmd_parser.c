#include "../../includes/minishell.h"

// in src/executor/parser.c
#include <stdio.h>
#include <stdlib.h>
t_cmd *parse_error(const char *msg)
{
    fprintf(stderr, "parse error: %s\n", msg);
    return NULL;   // NULL signals “I couldn’t build a t_cmd”
}


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
				if (!cmd_list)
					cmd_list = current;
				else
					last->next = current;
				last = current;
			}
			current = create_cmd();
			argc = 0;
		}
		else if (tokens->type == T_WORD || tokens->type == T_ARGUMENT || tokens->type == T_OPTION
		|| tokens->type == T_COMMAND)
		{
			if (!current)
				current = create_cmd();
			current->argv[argc++] = strdup(tokens->value);
		}
		if (tokens->type == T_IN_REDIR)
		{
			t_token *next = tokens->next;
			if (!next || next->type != T_WORD)
				return parse_error("missing filename after '<'");
			
			if (!current) current = create_cmd();
			current->input_file = strdup(next->value);
			tokens = next;      // consume the filename
			continue;           // skip the bottom `tokens = tokens->next`
		}
		else if (tokens->type == T_OUT_REDIR || tokens->type == T_DOUT_REDIR)
		{
			t_token *next = tokens->next;
			if (!next || next->type != T_WORD) {
				fprintf(stderr, "parse error: missing filename after '%s'\n",
						tokens->type == T_DOUT_REDIR ? ">>" : ">");
				free_cmd_list(cmd_list);
				return NULL;
			}
			if (!current) current = create_cmd();
			current->append_mode = (tokens->type == T_DOUT_REDIR);
			current->output_file = strdup(next->value);

			tokens = next;    // consume the filename
			continue;         // skip the tokens = tokens->next at the bottom
		}
		else if (tokens->type == T_HEREDOC)
		{
		    t_token *next = tokens->next;
		    /* Validate that heredoc is followed by a WORD (the delimiter). */
		    if (!next || next->type != T_WORD) {
		        fprintf(stderr, "parse error: missing delimiter after '<<'\n");
		        free_cmd_list(cmd_list);
		        return NULL;
		    }
		    if (!current)
		        current = create_cmd();
		    /* Record the delimiter, consume exactly one token, then continue. */
		    current->heredoc_delim = strdup(next->value);
		    tokens = next;
		    continue;
		}
		tokens = tokens->next;
	}

	if (current)
	{
		current->argv[argc] = NULL;
		if (!cmd_list)
			cmd_list = current;
		else
			last->next = current;
	}
	return (cmd_list);
}
