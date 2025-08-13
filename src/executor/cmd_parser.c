/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:34:53 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/11 23:34:53 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_cmd	*create_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = malloc(sizeof(char *) * 256);
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->heredoc_delims = NULL;
	cmd->heredoc_count = 0;
	cmd->append_mode = 0;
	cmd->next = NULL;
	cmd->token_list = NULL;
	return (cmd);
}

static void	pipe_case(t_cmd **cur, t_cmd **head, t_cmd **tail, int *argc)
{
	finalize_cmd(*cur, head, tail, *argc);
	*cur = create_cmd();
	*argc = 0;
}

static void	init_data_structures(t_cmd **head, t_cmd **tail, t_cmd **cur,
						int *argc)
{
	*head = NULL;
	*tail = NULL;
	*cur = NULL;
	*argc = 0;
}

static void	arg_case(t_token *tokens, t_cmd **cur, int *argc)
{
	if (!*cur)
		*cur = create_cmd();
	(*cur)->argv[(*argc)++] = ft_strdup(tokens->value);
}

t_cmd	*parse_tokens_to_commands(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*cur;
	int		argc;

	init_data_structures(&head, &tail, &cur, &argc);
	while (tokens)
	{
		if (tokens->type == T_PIPE)
			pipe_case(&cur, &head, &tail, &argc);
		else if (tokens->type == T_IN_REDIR || tokens->type == T_OUT_REDIR
			|| tokens->type == T_DOUT_REDIR || tokens->type == T_HEREDOC)
		{
			process_redir(&cur, tokens, &tokens);
			continue ;
		}
		else if (tokens->type == T_WORD || tokens->type == T_ARGUMENT
			|| tokens->type == T_FILE || tokens->type == T_COMMAND)
			arg_case(tokens, &cur, &argc);
		tokens = tokens->next;
	}
	finalize_cmd(cur, &head, &tail, argc);
	return (head);
}
