/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:42:03 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:30 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memset(void *ptr, int val, size_t bytes)
{
	size_t		i;
	char		*temp;

	i = 0;
	temp = (char *)ptr;
	while (i < bytes)
	{
		temp[i] = (char)val;
		i++;
	}
	return (ptr);
}
