/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 08:39:07 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:11 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memchr(const void *s, int c, size_t n)
{
	char		*temp;
	size_t		i;

	temp = (char *)s;
	i = 0;
	while (i < n)
	{
		if (*temp == (char)c)
			return ((void *)temp);
		temp++;
		i++;
	}
	return (0);
}
/*
#include <stdio.h>
#include <string.h>
int main () 
{
   const char str[] = "Tutorialspoint";
   const char ch = 't';
   char *ret;
   ret = ft_memchr(str, ch, strlen(str));
   printf("String after |%c| is - |%s|\n", ch, ret);
   return(0);
}*/
