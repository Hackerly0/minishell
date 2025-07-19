/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:57 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

static int	found(char c, const char *set)
{
	int	i;

	i = 0;
	while (set[i] != '\0')
	{
		if (c == set[i])
			return (1);
		i++;
	}
	return (0);
}

static int	start_index(const char *s1, const char *set)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' && found(s1[i], set) != 0)
		i++;
	return (i);
}

static int	end_index(const char *s1, const char *set)
{
	int	i;

	i = ft_strlen((char *)s1) - 1;
	while (i >= 0 && found(s1[i], set) != 0)
		i--;
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*final;
	int		start;
	int		end;

	if (s1 == NULL)
		return (NULL);
	start = start_index(s1, set);
	end = end_index(s1, set);
	if (start > end)
	{
		final = (char *)malloc(1);
		if (final == NULL)
			return (NULL);
		final[0] = '\0';
		return (final);
	}
	final = (char *)malloc(end - start + 2);
	if (final == NULL)
		return (NULL);
	ft_strlcpy(final, (char *)(s1 + start), end - start + 2);
	return (final);
}
