/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 08:39:07 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:53 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strrchr(const char *s, int c)
{
	int				i;
	unsigned char	*temp;
	unsigned char	a;

	a = (unsigned char)c;
	temp = (unsigned char *)s;
	i = 0;
	while (s[i] != '\0')
		i++;
	while (i >= 0)
	{
		if (*(temp + i) == a)
			return ((char *)(temp + i));
		i--;
	}
	return (NULL);
}
/*
#include <stdio.h>
#include <string.h>

int main () {
   int len;
   const char str[] = "https://www.tutorialspoint.com";
   const char ch = '0';
   char *ret;

   ret = ft_strrchr(str, ch);

   printf("String after |%c| is - |%s|\n", ch, ret);

   return(0);
}*/
