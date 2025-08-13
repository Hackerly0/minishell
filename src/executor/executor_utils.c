/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:46:22 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 02:42:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	permission_denied(char *cmd_path, char **envp, char **argv)
{
	write(2, "minishell: ", 11);
	write(2, cmd_path, ft_strlen(cmd_path));
	write(2, ": Permission denied or not executable\n", 39);
	free_envp(envp);
	if (cmd_path != argv[0])
		free(cmd_path);
	return (126);
}

int	setup_single_cmd_input(t_cmd *cmd, t_data *data)
{
	int	hd_fd;
	int	i;

	if (cmd && cmd->heredoc_count > 0)
	{
		i = 0;
		while (i < cmd->heredoc_count)
		{
			if (create_heredoc_pipe(cmd->heredoc_delims[i], &hd_fd, data) == -1)
				return (-1);
			if (i == cmd->heredoc_count - 1)
			{
				if (dup2(hd_fd, STDIN_FILENO) == -1)
				{
					close(hd_fd);
					return (-1);
				}
			}
			close(hd_fd);
			i++;
		}
		return (0);
	}
	return (setup_input_redirection(cmd));
}

void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int	count_commands(t_cmd *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}

int	is_executable_file(const char *path)
{
	struct stat	st;

	if (stat(path, &st) == -1)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	if (access(path, X_OK) == -1)
		return (0);
	return (1);
}
