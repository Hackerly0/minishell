/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 08:39:07 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:17 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*temp1;
	unsigned char	*temp2;

	temp1 = (unsigned char *)s1;
	temp2 = (unsigned char *)s2;
	i = 0;
	while (i < n && temp1[i] == temp2[i])
		i++;
	if (n == 0 || i == n)
		return (0);
	return (temp1[i] - temp2[i]);
}
/*
#include <stdio.h>

int	main()
{
	int	s1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	int	s2[] = {1, 2, 5, 4, 5, 6, 7, 8};

	printf("%i       ", ft_memcmp(s1, s2, 9 * sizeof(int)));
}*/
