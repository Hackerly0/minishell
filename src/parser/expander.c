/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 19:10:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 02:53:22 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	variable_founded(char *s, t_temp_out *temp, int *i, t_data *data)
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
		free(*(temp->out));
		write(2, "malloc failure!\n", 17);
		return (0);
	}
	ft_strlcpy(name, s + n, (*i) - n + 1);
	val = search_env(name, *(data->envp));
	free(name);
	if (!val)
		val = "";
	if (!out_realloc(temp->out, temp->o_len, val))
		return (0);
	return (1);
}

void	init_temp_out(t_temp_out *temp, char **out, int *o_len)
{
	temp->out = out;
	temp->o_len = o_len;
}

int	expander_loop(char **out, int *o_len, char *s, t_data *data)
{
	int			i;
	t_temp_out	temp;

	i = 0;
	init_temp_out(&temp, out, o_len);
	while (s[i])
	{
		if (s[i] == '$')
		{
			if (s[i + 1] == '?')
				exit_status(out, o_len, &i, data);
			else if (!ft_isalnum(s[i + 1]) && s[i + 1] != '_')
			{
				assign_char(out, o_len, s[i]);
				i++;
				return (1);
			}
			if (!variable_founded(s, &temp, &i, data))
				return (0);
		}
		else
			set_out(out, o_len, s, &i);
	}
	return (1);
}

char	*expand_string(char *s, t_data *data)
{
	char	*out;
	int		o_len;

	o_len = 0;
	out = malloc(2);
	if (!out)
		return (malloc_fail());
	out[0] = '\0';
	if (!expander_loop(&out, &o_len, s, data))
		return (NULL);
	if (o_len == 0)
	{
		free(out);
		return (NULL);
	}
	return (out);
}

void	expand_variables(t_data *data)
{
	t_token	*cur;

	cur = *(data->tokens);
	while (cur)
	{
		if (cur->type == T_WORD || cur->type == T_ARGUMENT
			|| cur->type == T_FILE || cur->type == T_COMMAND)
		{
			if (cur->in_quotes == 2 || cur->value[0] == '\0')
			{
				cur = cur->next;
				continue ;
			}
			if (!update_value(&cur->value, data))
				return ;
		}
		cur = cur->next;
	}
}
