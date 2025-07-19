/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:40:44 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:54:26 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include "libft.h"
/*
void	delete_content(void *content)
{
	free(content);
}*/

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*temp;

	while (*lst != NULL)
	{
		temp = *lst;
		*lst = (*lst)->next;
		del(temp->content);
		free(temp);
	}
}
/*

#include <stdio.h>

int	main()
{
	t_list	*list;
	t_list	*node;
	char	*a;
	char	*b;
	char	*c;

	a = (char *)malloc(9);
	b = (char *)malloc(9);
	c = (char *)malloc(9);
	ft_strlcpy(a,"Number 1", 9);
	ft_strlcpy(b, "Number 2", 9);
	ft_strlcpy(c, "Number 3", 9);
	list = ft_lstnew(a);
	ft_lstadd_back(&list, ft_lstnew(b));
	ft_lstadd_back(&list, ft_lstnew(c));
	node = list;
	while (node != NULL)
	{
		printf("%s\n", (char *)node->content);
		node = node->next;
	}
	ft_lstclear(&list, delete_content);
	node = list;
	if (node == NULL)
		printf("The list is cleared successfully!");
	else
		printf("Error!");
	return (0);

}*/
