/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:59:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/11 23:59:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_path_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*search_paths(char *cmd, char *path_env)
{
	char	**paths;
	char	*full_path;
	int		i;

	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	i = -1;
	while (paths[++i])
	{
		full_path = ft_strjoin_three(paths[i], "/", cmd);
		if (!full_path)
			break ;
		if (!access(full_path, X_OK))
		{
			free_array(paths);
			return (full_path);
		}
		free(full_path);
	}
	free_array(paths);
	return (NULL);
}

char	*find_command_path(char *cmd, char **envp)
{
	char	*path_env;

	if (ft_strchr(cmd, '/'))
	{
		if (!access(cmd, X_OK))
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_path_env(envp);
	return (search_paths(cmd, path_env));
}

void	free_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
