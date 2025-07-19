/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:51:54 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:54:53 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stddef.h>

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*last_node;

	last_node = lst;
	if (lst != NULL)
	{
		while (last_node->next != NULL)
			last_node = last_node->next;
		return (last_node);
	}
	return (NULL);
}
/*
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	int a = 10;
	int b = 20;
	int c = 30;

	t_list *list = ft_lstnew(&a);
	t_list *second = ft_lstnew(&b);
	t_list *last = ft_lstnew(&c);
	list->next = second;
	second->next = last;
	last->next = NULL;
	printf("%d\n", *(int *)ft_lstlast(list)->content);

	t_list *temp;
	while (list)
	{
		temp = list;
		list = list->next;
		free(temp);
	}
	return (0);
}*/
