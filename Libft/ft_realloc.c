/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnisirat <hnisirat@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 16:35:09 by hnisirat          #+#    #+#             */
/*   Updated: 2025/08/06 19:27:28 by hnisirat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_realloc(char *ptr, size_t new_size)
{
	char	*str;
	size_t	old_size;

	if (!ptr)
	{
		if (new_size == 0)
			exit(1);
		return (malloc(new_size));
	}
	old_size = ft_strlen(ptr);
	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (new_size == old_size)
		return (ptr);
	str = malloc(new_size);
	if (!str)
		exit(1);
	if (old_size > new_size)
		old_size = new_size;
	ft_memcpy(str, ptr, old_size);
	free(ptr);
	return (str);
}
