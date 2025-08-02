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

static int	exec_builtin_single(t_cmd *cmd, char **envp)
{
	char	*original_argv0;
	char	*cmd_name;
	int		exit_code;

	cmd_name = get_cmd_name(cmd->argv[0]);
	original_argv0 = cmd->argv[0];
	cmd->argv[0] = cmd_name;
	exit_code = execute_builtin(cmd->argv, envp);
	cmd->argv[0] = original_argv0;
	return (exit_code);
}

static int	handle_builtin_single(t_cmd *cmd_list, char **envp)
{
	int	stdin_backup;
	int	stdout_backup;
	int	exit_code;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (setup_single_cmd_input(cmd_list) == -1
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

int	single_command_handler(t_cmd *cmd_list, char **envp)
{
	pid_t	pid;
	int		status;
	char	*cmd_name;

	if (cmd_list->argv[0])
	{
		cmd_name = get_cmd_name(cmd_list->argv[0]);
		if (is_builtin(cmd_name))
			return (handle_builtin_single(cmd_list, envp));
	}
	pid = fork();
	if (pid == 0)
	{
		setup_single_cmd_input(cmd_list);
		setup_output_redirection(cmd_list);
		exit(execute_single_command(cmd_list, envp));
	}
	waitpid(pid, &status, 0);
	return (WEXITSTATUS(status));
}
