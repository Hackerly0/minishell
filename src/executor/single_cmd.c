/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 00:00:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_cmd_name(char *cmd_path)
{
	char	*cmd_name;

	cmd_name = ft_strrchr(cmd_path, '/');
	if (cmd_name)
		cmd_name++;
	else
		cmd_name = cmd_path;
	return (cmd_name);
}

static int	exec_builtin_single(t_cmd *cmd, t_env **envp)
{
	char	*original_argv0;
	char	*cmd_name;
	int		exit_code;

	cmd_name = get_cmd_name(cmd->argv[0]);
	original_argv0 = cmd->argv[0];
	cmd->argv[0] = cmd_name;
	exit_code = execute_builtin(cmd->argv, envp, cmd);
	cmd->argv[0] = original_argv0;
	return (exit_code);
}

static int	handle_builtin_single(t_cmd *cmd_list, t_env **envp, t_data *data)
{
	int	stdin_backup;
	int	stdout_backup;
	int	exit_code;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (setup_single_cmd_input(cmd_list, data) == -1
		|| setup_output_redirection(cmd_list) == -1)
	{
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		return (1);
	}
	exit_code = exec_builtin_single(cmd_list, envp);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (exit_code);
}

int	single_command_handler(t_cmd *cmd_list, t_env **env_list, t_data *data)
{
	pid_t	pid;
	int		status;

	if (cmd_list->argv && cmd_list->argv[0]
		&& is_builtin(cmd_list->argv[0]))
		return (handle_builtin_single(cmd_list, env_list, data));
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		restore_default_signals();
		if (setup_single_cmd_input(cmd_list, data) == -1
			|| setup_output_redirection(cmd_list) == -1)
		{
			free_data(data);
			exit(1);
		}
		status = execute_single_command(cmd_list, env_list);
		free_data(data);
		exit(status);
	}
	if (waitpid(pid, &status, 0) == -1)
		return (perror("waitpid"), 1);
	return (status_to_code(status));
}
