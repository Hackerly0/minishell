/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:15 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		i;
	int		j;
	char	*s;

	i = 0;
	j = 0;
	s = (char *)malloc(ft_strlen((char *)s1) + ft_strlen((char *)s2) + 1);
	if (s == 0)
		return (0);
	while (s1[i] != '\0')
	{
		s[i] = s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		s[i + j] = s2[j];
		j++;
	}
	s[i + j] = '\0';
	return (s);
}
/*

#include <stdio.h>

int	main()
{
	char const	s1[] = "Obada Mun";
	char const	s2[] = "ir Alhoora";
	printf("%s\n", ft_strjoin(s1, s2));
}*/
