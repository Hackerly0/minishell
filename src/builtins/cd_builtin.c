/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:55:21 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 15:55:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	oldpwd_set(char **oldpwd, t_env **env_list)
{
	t_env	*node;

	if (*oldpwd)
	{
		node = env_find(*env_list, "OLDPWD");
		if (node)
			env_set_value(node, *oldpwd);
		else
			env_add_front(env_list, "OLDPWD", *oldpwd);
		free(*oldpwd);
		*oldpwd = NULL;
	}
}

void	pwd_set(t_env **env_list)
{
	char	*pwd;
	t_env	*node;

	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		node = env_find(*env_list, "PWD");
		if (node)
			env_set_value(node, pwd);
		else
			env_add_front(env_list, "PWD", pwd);
		free(pwd);
	}
}

int	set_path(char **path, char **oldpwd, t_env **env_list)
{
	*path = search_env("HOME", *env_list);
	if (!*path)
	{
		error_str("cd: HOME not set");
		free(*oldpwd);
		return (0);
	}
	return (1);
}

int	builtin_cd(char **argv, t_env **env_list)
{
	char	*path;
	char	*oldpwd;

	path = argv[1];
	if (argv[2])
	{
		error_str("cd: too many arguments");
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (!path)
		if (!set_path(&path, &oldpwd, env_list))
			return (1);
	if (chdir(path) != 0)
	{
		error_file(path, NULL);
		free(oldpwd);
		return (1);
	}
	oldpwd_set(&oldpwd, env_list);
	pwd_set(env_list);
	return (0);
}
