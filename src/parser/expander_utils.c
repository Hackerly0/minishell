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
	*out = ft_realloc(*out, *o_len + 2);
	(*out)[(*o_len)++] = s[(*i)++];
	(*out)[*o_len] = '\0';
}

void	exit_status(char **out, int *o_len, int *i)
{
	char	*temp;

	temp = ft_itoa(g_signal);
	*out = ft_realloc(*out, *o_len + ft_strlen(temp)); //
	ft_memcpy(*out + *o_len, temp, ft_strlen(temp));
	*o_len += ft_strlen(temp);
	free(temp);
	(*i)++;
}
