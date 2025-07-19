/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:00:39 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:40 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
/*
char	add_index(unsigned int i, char c)
{
	if (i + c > 127)
		return (40);
	else
		return (i + c);
}*/

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*str;
	int		i;
	int		len_s;

	len_s = 0;
	while (s[len_s] != '\0')
		len_s++;
	str = (char *)malloc(len_s + 1);
	if (str == 0)
		return (0);
	i = 0;
	while (i < len_s)
	{
		str[i] = f((unsigned int)i, s[i]);
		i++;
	}
	str[i] = '\0';
	return (str);
}
/*
#include <stdio.h>

int	main()
{
	char	s[] = "1111111abcdz";
	char	*a;

	a = ft_strmapi(s, add_index);
	printf("%s\n", a);
}*/
