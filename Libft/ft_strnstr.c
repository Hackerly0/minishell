/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:48 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strnstr(const char *src, const char *to_find, size_t n)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (to_find[0] == '\0')
		return ((char *)src);
	while (i < n && src[i] != '\0')
	{
		while (src[i] == to_find[j] && to_find[j] && src[i] && i < n)
		{
			i++;
			j++;
		}
		if (!to_find[j] && src)
			return ((char *)&src[i - j]);
		i = i - j + 1;
		j = 0;
	}
	return (0);
}
/*
#include <stdio.h>
#include <string.h>

int	main()
{
	char	s1[] = "Foo aaoaaoaaa";
	char	s2[] = "aaoaaa";
	char *ptr = ft_strnstr(s1, s2, 13);
	printf("%s\n", ptr);
}*/
