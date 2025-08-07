/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassende <hassende@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 21:50:01 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/07 12:13:14 by hassende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	increment(int *j, int *len, char *line)
{
	while (line[(*j)] && (line[(*j)] == '>' || line[(*j)] == '<'))
	{
		(*j)++;
		(*len)++;
	}
}

t_token	*extract_operator_token(char *line, int *i)
{
	int		len;
	char	*val;
	t_token	*tok;
	int		j;

	if (line[*i] == '|')
	{
		val = ft_strndup(line + *i, 1);
		if (!val)
			return (NULL);
		tok = create_token(val, T_PIPE, 0);
		free(val);
		(*i)++;
		return (tok);
	}
	j = *i + 1;
	len = 1;
	increment(&j, &len, line);
	val = ft_strndup(line + *i, len);
	if (!val)
		return (NULL);
	tok = create_token(val, classify_token(val), 0);
	free(val);
	*i += len;
	return (tok);
}

int	token_cases(char *line, t_token **head, int *i)
{
	t_token	*op;

	if (ft_isspace(line[*i]))
	{
		(*i)++;
		return (1);
	}
	if (is_special_char(line[*i]))
	{
		op = extract_operator_token(line, i);
		if (!op)
		{
			free_tokens(head);
			return (0);
		}
		append_token(head, op);
		return (1);
	}
	return (extract_word_tokens(line, i, head));
}

t_token	*tokenize(char *line)
{
	t_token	*head;
	int		i;

	head = NULL;
	i = 0;
	while (line[i])
	{
		if (!token_cases(line, &head, &i))
		{
			if (head)
				free_tokens(&head);
			return (NULL);
		}
	}
	return (head);
}

void	print_tokens(t_token *tokens)
{
	while (tokens)
	{
		printf("[%s] type=%d quoted=%d\n",
			tokens->value, tokens->type, tokens->in_quotes);
		tokens = tokens->next;
	}
}
