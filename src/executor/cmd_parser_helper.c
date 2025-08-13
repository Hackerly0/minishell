/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser_helper.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:37:12 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 02:42:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	status_to_code(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
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

void	process_redir(t_cmd **cur, t_token *tok, t_token **toks)
{
	t_token	*next;

	if (!*cur)
		*cur = create_cmd();
	next = tok->next;
	if (tok->type == T_IN_REDIR)
		(*cur)->input_file = ft_strdup(next->value);
	else if (tok->type == T_OUT_REDIR
		|| tok->type == T_DOUT_REDIR)
	{
		(*cur)->append_mode
			= (tok->type == T_DOUT_REDIR);
		(*cur)->output_file = ft_strdup(next->value);
	}
	else
		(*cur)->heredoc_delim = ft_strdup(next->value);
	*toks = next->next;
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	if (cmd->argv)
		free_array(cmd->argv);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd->heredoc_delim);
	free(cmd);
}
