/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:26 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*cast_dest;
	unsigned char	*cast_src;

	i = 0;
	cast_src = (unsigned char *)src;
	cast_dest = (unsigned char *)dest;
	if (!src && !dest)
		return (NULL);
	if (src < dest)
	{
		while (i < n)
		{
			cast_dest[n - 1 - i] = cast_src[n - 1 - i];
			i++;
		}
		return (dest);
	}
	while (i < n)
	{
		cast_dest[i] = cast_src[i];
		i++;
	}
	return (dest);
}
