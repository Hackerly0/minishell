/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:45:42 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 10:53:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_builtin_cmd(char *cmd_path)
{
	char	*cmd_name;

	cmd_name = ft_strrchr(cmd_path, '/');
	if (cmd_name)
		cmd_name++;
	else
		cmd_name = cmd_path;
	return (is_builtin(cmd_name));
}

void	remove_backslash(char **cmd_name)
{
	int	j;

	j = 1;
	if (ft_strlen(*cmd_name) < 2)
		return ;
	while ((*cmd_name)[j])
	{
		(*cmd_name)[j - 1] = (*cmd_name)[j];
		j++;
	}
	(*cmd_name)[j - 1] = '\0';
}

int	builtin_case(char **cmd, char **cmd_name, char **envp)
{
	if (ft_strrchr(*cmd, '/'))
	{
		*cmd_name = ft_strdup(ft_strrchr(*cmd, '/'));
		remove_backslash(cmd_name);
		free(*cmd);
		*cmd = ft_strdup(*cmd_name);
	}
	else
		*cmd_name = ft_strdup(*cmd);
	if (!*cmd)
	{
		free(*cmd_name);
		free_envp(envp);
		perror("malloc failure");
		return (1);
	}
	return (0);
}

int	cmd_not_found(t_cmd *cmd, char **cmd_path, char **envp)
{
	write(2, "minishell: ", 11);
	write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(2, ": command not found\n", 20);
	free(*cmd_path);
	free_envp(envp);
	return (127);
}

void	execute_execve(char *cmd_path, t_cmd *cmd, char **envp)
{
	char	*cmd_name;

	execve(cmd_path, cmd->argv, envp);
	cmd_name = ft_strrchr(cmd->argv[0], '/');
	if (cmd_name)
		cmd_name++;
	else
		cmd_name = cmd->argv[0];
	if (errno == ENOENT)
	{
		write(2, "minishell: ", 11);
		write(2, cmd_name, ft_strlen(cmd_name));
		write(2, ": command not found\n", 21);
	}
	else
		perror("execve");
	if (cmd_path != cmd->argv[0])
		free(cmd_path);
	free_cmd(cmd);
}
