/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:22 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h> 
#include <string.h>
#include <stddef.h>

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*temp1;
	unsigned char	*temp2;

	temp1 = (unsigned char *)dest;
	temp2 = (unsigned char *)src;
	i = 0;
	if (!temp1 && !temp2)
		return (NULL);
	while (i < n)
	{
		temp1[i] = temp2[i];
		i++;
	}
	return (dest);
}
/*
#include <stdio.h>
#include <string.h>

int	main()
{
	char	s1[] = "";
	const char	s2[] = "";

	//printf("%s\n", (char *)memcpy("", "", 7));
	printf("%s\n", (char *)ft_memcpy("", "", 7));
}*/
