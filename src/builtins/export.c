/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:32:03 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:48:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	free_stuff_inc(char **key, char **val, int *i)
{
	free(*key);
	free(*val);
	(*i)++;
}

int	while_cases(char **argv, char **key, char **val, t_env **penv)
{
	int	i;
	int	x;
	int	ret;

	i = 1;
	ret = 0;
	while (argv[i])
	{
		x = parse_export_arg(argv[i], key, val);
		if (x == -1)
		{
			print_export_id_error(argv[i]);
			ret = 1;
		}
		else if (x == -2)
		{
			free_stuff_inc(key, val, &i);
			continue ;
		}
		else if (export_set_list(penv, *key, *val))
			ret = 1;
		free_stuff_inc(key, val, &i);
	}
	return (ret);
}

int	builtin_export(char **argv, t_env **penv)
{
	int		ret;
	char	*key;
	char	*val;

	if (!argv[1])
	{
		builtin_env(argv, penv);
		return (0);
	}
	ret = 0;
	while_cases(argv, &key, &val, penv);
	return (ret);
}
