/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_envp.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:01:20 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:45:38 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_env	*env_find(t_env *head, const char *key)
{
	while (head)
	{
		if (!ft_strcmp(head->key, key))
			return (head);
		head = head->next;
	}
	return (NULL);
}

t_env	*env_add_front(t_env **head, const char *k, const char *v)
{
	t_env	*node;

	node = (t_env *)malloc(sizeof(*node));
	if (!node)
		return (NULL);
	node->key = ft_strdup(k);
	if (!node->key)
		return (free(node), NULL);
	if (v)
		node->val = ft_strdup(v);
	else
		node->val = ft_strdup("");
	if (!node->val)
		return (free(node->key), free(node), NULL);
	node->next = *head;
	*head = node;
	return (node);
}

int	env_set_value(t_env *n, const char *v)
{
	char	*dup;

	if (!v)
		v = "";
	dup = ft_strdup(v);
	if (!dup)
		return (1);
	free(n->val);
	n->val = dup;
	return (0);
}

int	export_set_list(t_env **env, const char *k, const char *v)
{
	t_env	*n;

	n = env_find(*env, k);
	if (!n)
	{
		if (env_add_front(env, k, v))
			return (0);
		else
			return (1);
	}
	if (v)
		return (env_set_value(n, v));
	return (0);
}
