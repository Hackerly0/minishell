/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 02:42:45 by marvin            #+#    #+#             */
/*   Updated: 2025/08/13 02:42:45 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	free_data(t_data *data)
{
	if (data)
	{
		if (data->cmds)
			free_cmd_list(data->cmds);
		if (data->envp)
			env_free_list(*(data->envp));
		if (data->tokens)
			free_tokens(data->tokens);
	}
}

int	hd_make_pipe(int pipefd[2])
{
	if (pipe(pipefd) == -1)
		return (-1);
	return (0);
}

int	non_path(char **cmd_path, char **envp)
{
	char	*resolved_path;

	if ((*cmd_path)[0] != '/')
	{
		resolved_path = find_command_path(*cmd_path, envp);
		if (resolved_path)
			*cmd_path = resolved_path;
		else
		{
			write(2, "minishell: ", 11);
			write(2, *cmd_path, ft_strlen(*cmd_path));
			write(2, ": command not found\n", 20);
			free_envp(envp);
			return (127);
		}
	}
	return (0);
}
