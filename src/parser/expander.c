/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnisirat <hnisirat@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 19:10:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/06 19:23:48 by hnisirat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	variable_founded(char *s, char **out, int *o_len, int *i)
{
	char	*name;
	char	*val;
	int		n;

	(*i)++;
	n = *i;
	while (ft_isalnum(s[(*i)]) || s[(*i)] == '_')
		(*i)++;
	name = malloc((*i) - n + 1);
	if (!name)
	{
		free(*out);
		write(2, "malloc failure!\n", 17);
		return (0);
	}
	ft_strlcpy(name, s + n, (*i) - n + 1);
	val = getenv(name);
	free(name);
	if (!val)
		val = "";
	*out = ft_realloc(*out, *o_len + ft_strlen(val) + 1);
	ft_memcpy(*out + *o_len, val, ft_strlen(val));
	*o_len += ft_strlen(val);
	(*out)[*o_len] = '\0';
	return (1);
}

char	*expand_string(char *s)
{
	char	*out;
	int		o_len;
	int		i;

	o_len = 0;
	out = malloc(1);
	if (!out)
		return (malloc_fail());
	i = 0;
	while (s[i])
	{
		if (s[i] == '$')
		{
			if (s[i + 1] == '?')
				exit_status(&out, &o_len, &i);
			if (!variable_founded(s, &out, &o_len, &i))
				return (NULL);
		}
		else
			set_out(&out, &o_len, s, &i);
	}
	return (out);
}

void	expand_variables(t_token **tokens)
{
	t_token	*cur;
	char	*newval;

	cur = *tokens;
	while (cur)
	{
		if (cur->type == T_WORD || cur->type == T_ARGUMENT
			|| cur->type == T_FILE || cur->type == T_COMMAND)
		{
			if (cur->in_quotes == 2)
			{
				cur = cur->next;
				continue ;
			}
			newval = expand_string(cur->value);
			free(cur->value);
			if (!newval)
				return ;
			cur->value = newval;
		}
		cur = cur->next;
	}
}
