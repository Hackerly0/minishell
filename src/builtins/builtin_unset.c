/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnisirat <hnisirat@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 12:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/12 12:30:43 by hnisirat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	key_match(const char *key, const char *name)
{
	if (!key || !name)
		return (0);
	return (ft_strcmp(key, name) == 0);
}

static void	unlink_free_node(t_env **lst, t_env *prev, t_env *cur)
{
	if (prev)
		prev->next = cur->next;
	else
		*lst = cur->next;
	free(cur->key);
	if (cur->val)
		free(cur->val);
	free(cur);
}

static int	unset_one(char *name, t_env **lst)
{
	t_env	*prev;
	t_env	*cur;

	if (!is_valid_identifier(name))
	{
		error_cmd("unset", "not a valid identifier");
		return (1);
	}
	prev = NULL;
	cur = *lst;
	while (cur && !key_match(cur->key, name))
	{
		prev = cur;
		cur = cur->next;
	}
	if (cur)
		unlink_free_node(lst, prev, cur);
	return (0);
}

int	builtin_unset(char **argv, t_env **lst)
{
	int	i;
	int	ret;

	if (!argv || !argv[1] || !lst)
		return (0);
	i = 1;
	ret = 0;
	while (argv[i])
		ret |= unset_one(argv[i++], lst);
	return (ret);
}
