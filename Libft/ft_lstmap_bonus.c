/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:51:54 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:54:58 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new;
	t_list	*node;
	void	*mapped_content;

	node = NULL;
	while (lst != NULL)
	{
		mapped_content = f(lst->content);
		new = ft_lstnew(mapped_content);
		if (new == NULL)
		{
			ft_lstdelone(new, del);
			ft_lstclear(&node, del);
			free(mapped_content);
			return (NULL);
		}
		ft_lstadd_back(&node, new);
		lst = lst->next;
	}
	return (node);
}
