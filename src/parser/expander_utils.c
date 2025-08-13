/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 19:10:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 22:02:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*malloc_fail(void)
{
	write(2, "malloc failure!\n", 17);
	return (NULL);
}

void	set_out(char **out, int *o_len, char *s, int *i)
{
	char	*temp;

	temp = ft_realloc(*out, *o_len + 2);
	if (!temp)
	{
		free(*out);
		return ;
	}
	*out = temp;
	(*out)[(*o_len)++] = s[(*i)++];
	(*out)[*o_len] = '\0';
}

void	exit_status(char **out, int *o_len, int *i, t_data *data)
{
	char	*temp;

	temp = ft_itoa(data->exit_code);
	*out = ft_realloc(*out, *o_len + ft_strlen(temp));
	ft_memcpy(*out + *o_len, temp, ft_strlen(temp));
	*o_len += ft_strlen(temp);
	free(temp);
	(*i)++;
}

int	update_value(char **value, t_data *data)
{
	char	*newval;

	newval = expand_string(*value, data);
	free(*value);
	if (!newval)
	{
		*value = ft_strdup("");
		if (!*value)
			return (0);
	}
	else
		*value = newval;
	return (1);
}

void	assign_char(char **out, int *o_len, char c)
{
	*out = ft_realloc(*out, *o_len + 2);
	(*out)[*o_len] = c;
	(*out)[*o_len + 1] = '\0';
	(*o_len)++;
}
