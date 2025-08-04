/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 19:56:46 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 19:57:01 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isspace(int c)
{
	if (c == 32)
		return (1);
	return (0);
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			break ;
		i++;
	}
	return (s1[i] - s2[i]);
}

char	*ft_strndup(const char *src, int size)
{
	char	*duplicated;
	int		i;

	i = 0;
	while (i < size && src[i])
		i++;
	duplicated = malloc(i + 1);
	if (duplicated == NULL)
		return (NULL);
	ft_memcpy(duplicated, src, i);
	duplicated[i] = '\0';
	return (duplicated);
}
