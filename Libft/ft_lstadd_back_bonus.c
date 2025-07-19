/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:51:54 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:54:15 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stddef.h>

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*node;

	node = *lst;
	if (lst == NULL || new == NULL)
		return ;
	if (*lst != NULL)
	{
		while (node->next != NULL)
		{
			node = node->next;
		}
		node->next = new;
	}
	else
	{
		*lst = new;
	}
}
/*
#include <stdio.h>
#include <stdlib.h>

void	print_list(t_list *lst)
{
	int	i;

	i = 1;
	while (lst)
	{
		printf("Node number %i is : %d\n", i, *(int *)lst->content);
		lst = lst->next;
		i++;
	}
}

int	main()
{
	int	a = 10;
	int	b = 20;
	int	c = 30;

	t_list	*list = ft_lstnew(&a);
	t_list	*second = ft_lstnew(&b);
	t_list  *last = ft_lstnew(&c);
	list->next = NULL;
	ft_lstadd_back(&list, second);
	ft_lstadd_back(&list, last);
	print_list(list);
	t_list *temp;
	while (list)
	{
		temp = list;
		list = list->next;
		free(temp);
	}
}*/
