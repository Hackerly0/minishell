/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 14:56:11 by oalhoora         #+#    #+#              */
/*   Updated: 2025/08/12 14:56:11 by oalhoora        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	out_realloc(char **out, int *o_len, char *val)
{
	char	*temp;

	temp = ft_realloc(*out, *o_len + ft_strlen(val) + 1);
	if (!temp)
	{
		free(*out);
		return (0);
	}
	*out = temp;
	ft_memcpy(*out + *o_len, val, ft_strlen(val));
	*o_len += ft_strlen(val);
	(*out)[*o_len] = '\0';
	return (1);
}

char	*search_env(char *name, t_env *env_list)
{
	t_env	*env;

	env = env_list;
	while (env)
	{
		if (ft_strcmp(env->key, name) == 0)
			return (env->val);
		env = env->next;
	}
	return (NULL);
}
