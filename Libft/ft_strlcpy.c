/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 12:50:44 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:30 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
//#include <stdio.h>
//#include <bsd/string.h>

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	sizesrc;

	i = 0;
	sizesrc = 0;
	while (src[sizesrc] != '\0')
		sizesrc++;
	if (size == 0)
		return (sizesrc);
	if (src == NULL)
		return (0);
	while (i < size && src[i] != '\0')
	{
		if (i == (size - 1))
			break ;
		else
			dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (sizesrc);
}
