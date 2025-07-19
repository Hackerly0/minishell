/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 12:15:04 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:05 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

char	*ft_strdup(const char *src)
{
	char	*duplicated;
	int		i;

	i = 0;
	while (src[i] != '\0')
		i++;
	duplicated = malloc(i + 1);
	if (duplicated == NULL)
		return (NULL);
	i = 0;
	while (src[i] != '\0')
	{
		duplicated[i] = src[i];
		i++;
	}
	duplicated[i] = '\0';
	return (duplicated);
}
