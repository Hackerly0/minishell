/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 19:55:25 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/11 18:04:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_token_type	classify_token(char *s)
{
	if (ft_strcmp(s, "|") == 0)
		return (T_PIPE);
	if (ft_strcmp(s, "<<") == 0 || ft_strcmp(s, "<<") == '<'
		|| ft_strcmp(s, "<<") == '>')
		return (T_HEREDOC);
	if (ft_strcmp(s, "<") == 0 || ft_strcmp(s, "<") == '>')
		return (T_IN_REDIR);
	if (ft_strcmp(s, ">>") == 0 || ft_strcmp(s, ">>") == '>'
		|| ft_strcmp(s, ">>") == '<')
		return (T_DOUT_REDIR);
	if (ft_strcmp(s, ">") == 0 || ft_strcmp(s, ">") == '<')
		return (T_OUT_REDIR);
	return (T_WORD);
}

void	free_tokens(t_token **tokens)
{
	t_token	*current;
	t_token	*next;

	current = *tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
	*tokens = NULL;
}

t_token	*create_token(const char *val, t_token_type type, int in_quotes)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = ft_strdup(val);
	if (!tok->value)
	{
		free_tokens(&tok);
		return (NULL);
	}
	tok->type = type;
	tok->in_quotes = in_quotes;
	tok->next = NULL;
	return (tok);
}

void	append_token(t_token **head, t_token *new)
{
	t_token	*cur;

	if (!new)
		return ;
	if (!*head)
		*head = new;
	else
	{
		cur = *head;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
}

int	quotes_validation(char *line)
{
	int		i;
	char	c;

	i = 0;
	while (line[i])
	{
		if (line[i] == '\'' || line[i] == '"')
		{
			c = line[i++];
			while (line[i] != c && line[i])
				i++;
			if (!line[i])
				return (0);
		}
		i++;
	}
	return (1);
}
