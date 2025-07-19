/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 08:39:07 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:56:01 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char			*temp;
	unsigned char	a;

	a = (unsigned char)c;
	temp = (char *)s;
	if (a == '\0')
	{
		return (temp + ft_strlen(temp));
	}
	while (*temp != '\0')
	{
		if (*temp == a)
			return (temp);
		temp++;
	}
	return (NULL);
}
/*
#include <stdio.h>

int main() 
{
   const char *str = "Welcome to Tutorialspoint";
   char ch = 'n';
   // Find the first occurrence of 'u' in the string
   char *p = ft_strchr(str, ch);

   if (p != 0) 
   {
       printf("String starting from '%c' is: %s\n", ch, p);
   } 
   else 
   {
       printf("Character '%c' not found in the string.\n", ch);
   }
   return 0;
}*/
