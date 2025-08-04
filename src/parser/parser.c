/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 22:05:58 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 19:51:33 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	pipe_check(t_token **cur)
{
	char	*s;

	if (ft_strncmp((*cur)->value, "|", 1) == 0 && (*cur)->next != NULL
		&&ft_strncmp((*cur)->next->value, "|", 1) == 0)
	{
		s = ft_strdup("minishell: syntax error near unexpected token '||'\n");
		write(2, s, ft_strlen(s));
		free(s);
		g_signal = 2;
		return (0);
	}
	else if ((*cur)->next == NULL)
	{
		s = ft_strdup("minishell: syntax error near unexpected token '|'\n");
		write(2, s, ft_strlen(s));
		free(s);
		g_signal = 2;
		return (0);
	}
	return (1);
}

int	redirections(t_token *cur)
{
	while (cur)
	{
		if (cur->type == T_DOUT_REDIR || cur->type == T_HEREDOC
			|| cur->type == T_IN_REDIR || cur->type == T_OUT_REDIR)
		{
			if (ft_strcmp(cur->value, "<") != 0
				&& ft_strcmp(cur->value, "<<") != 0
				&& ft_strcmp(cur->value, ">") != 0
				&& ft_strcmp(cur->value, ">>") != 0)
				return (print_syntax_error(cur->value));
			if (cur->next == NULL || cur->next->type != T_WORD)
				return (missig_file());
			cur = cur->next;
			cur->type = T_FILE;
		}
		cur = cur->next;
	}
	return (1);
}

int	wrong_cmd(t_token *cur)
{
	char	*s;

	if (ft_strlen(cur->value) == 0)
		return (1);
	if (!ft_strcmp(cur->value, ".") || !ft_strcmp(cur->value, "/")
		|| !ft_strcmp(cur->value, "./")
		|| !ft_strcmp(cur->value + ft_strlen(cur->value) - 1, "/"))
	{
		s = ft_strjoin_three("minishell: ", cur->value, ": Is a directory\n");
		write(2, s, ft_strlen(s));
		free(s);
		g_signal = 2;
		return (0);
	}
	return (1);
}

int	continue_parser(t_token **cur)
{
	int	count;

	count = 0;
	while (*cur)
	{
		if ((*cur)->type == T_PIPE)
		{
			if (!pipe_check(cur))
				return (0);
			count = 0;
		}
		if ((*cur)->type == T_WORD)
		{
			if (count == 0)
				(*cur)->type = T_COMMAND;
			else
				(*cur)->type = T_ARGUMENT;
			count++;
		}
		(*cur) = (*cur)->next;
	}
	g_signal = 0;
	return (1);
}

int	parser(t_token **tokens)
{
	t_token	*cur;

	cur = *tokens;
	if (cur->type == T_PIPE)
	{
		g_signal = 2;
		return (pipe_error(cur));
	}
	if (cur->type == T_WORD)
	{
		if (!wrong_cmd(cur))
		{
			g_signal = 2;
			return (0);
		}
	}
	if (!redirections(cur))
		return (0);
	return (continue_parser(&cur));
}
