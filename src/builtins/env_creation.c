/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_creation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:20:58 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:20:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	normal_split(const char *s, const char *eq, char **key, char **val)
{
	*key = ft_substr(s, 0, (size_t)(eq - s));
	if (!*key)
		return (1);
	*val = ft_strdup(eq + 1);
	if (!*val)
	{
		free(*key);
		*key = NULL;
		return (1);
	}
	return (0);
}

static int	split_kv(const char *s, char **key, char **val)
{
	char	*eq;

	*key = NULL;
	*val = NULL;
	if (!s)
		return (1);
	eq = ft_strchr(s, '=');
	if (eq)
		return (normal_split(s, eq, key, val));
	*key = ft_strdup(s);
	if (!*key)
		return (1);
	*val = ft_strdup("");
	if (!*val)
	{
		free(*key);
		*key = NULL;
		return (1);
	}
	return (0);
}

static t_env	*env_node_new(const char *entry)
{
	t_env	*node;
	char	*k;
	char	*v;

	k = NULL;
	v = NULL;
	if (split_kv(entry, &k, &v))
		return (NULL);
	node = (t_env *)malloc(sizeof(*node));
	if (!node)
	{
		free(k);
		free(v);
		return (NULL);
	}
	node->key = k;
	node->val = v;
	node->next = NULL;
	return (node);
}

void	env_free_list(t_env *head)
{
	t_env	*nxt;

	while (head)
	{
		nxt = head->next;
		free(head->key);
		free(head->val);
		free(head);
		head = nxt;
	}
}

t_env	*env_init(char **src)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (src && src[i])
	{
		node = env_node_new(src[i]);
		if (!node)
			return (env_free_list(head), NULL);
		if (!head)
			head = node;
		else
			tail->next = node;
		tail = node;
		i++;
	}
	return (head);
}
