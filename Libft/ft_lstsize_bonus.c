/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:51:54 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:06 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int		count;
	t_list	*node;

	count = 0;
	node = lst;
	while (node != NULL)
	{
		node = node->next;
		count++;
	}
	return (count);
}
