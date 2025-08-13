/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 00:00:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	execute_single_command(t_cmd *cmd, t_env **env_list)
{
	char	*cmd_path;
	char	*cmd_name;
	char	**envp;

	if (!cmd->argv[0])
		return (0);
	envp = env_list_to_array(*env_list);
	if (is_builtin_cmd(cmd->argv[0]))
	{
		if (builtin_case(&(cmd->argv[0]), &cmd_name, envp))
			return (1);
		free_envp(envp);
		free(cmd_name);
		return (execute_builtin(cmd->argv, env_list, cmd));
	}
	cmd_path = cmd->argv[0];
	if (non_path(&cmd_path, envp) == 127)
		return (127);
	if (cmd->argv[0][0] == '\0')
		return (cmd_not_found(cmd, &cmd_path, envp));
	if (!is_executable_file(cmd_path))
		return (permission_denied(cmd_path, envp, cmd->argv));
	execute_execve(cmd_path, cmd, envp);
	free_envp(envp);
	return (127);
}

int	execute_command_line(t_token *tokens, t_env **envp, t_data *data)
{
	t_cmd	*cmds;
	int		code;

	if (!tokens)
		return (0);
	cmds = parse_tokens_to_commands(tokens);
	data->cmds = cmds;
	if (!cmds)
		return (0);
	cmds->token_list = tokens;
	code = execute_pipeline(cmds, envp, data);
	free_cmd_list(cmds);
	return (code);
}

static void	fork_pipeline_commands(t_cmd *cmd_list, t_env **envp,
								int n, t_data *data)
{
	t_temp_pipeline	temp;

	if (!init_pipeline_data(&temp, cmd_list, n, data))
	{
		perror("Failed to initialize pipeline data");
		return ;
	}
	while (temp.i < n)
	{
		temp.pids[temp.i] = fork();
		if (temp.pids[temp.i] == 0)
			child_code_pipeline(&temp, cmd_list, envp, n);
		else if (temp.pids[temp.i] < 0)
		{
			error_fd_pipe(&temp, n);
			return ;
		}
		temp.cur = temp.cur->next;
		temp.i++;
	}
	pipeline_cont(&temp, n);
}

int	wait_pipeline_processes(pid_t *pids, int n)
{
	int		i;
	int		status;
	int		last;
	pid_t	last_pid;
	pid_t	done;

	if (!pids || n <= 0)
		return (0);
	last = 0;
	last_pid = pids[n - 1];
	i = -1;
	while (++i < n)
	{
		done = waitpid(-1, &status, 0);
		if (done == -1)
			continue ;
		if (done == last_pid)
		{
			if (WIFEXITED(status))
				last = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last = WTERMSIG(status) + 128;
		}
	}
	return (last);
}

int	execute_pipeline(t_cmd *cmd_list, t_env **envp, t_data *data)
{
	int	n;

	n = count_commands(cmd_list);
	if (n == 1)
		return (single_command_handler(cmd_list, envp, data));
	fork_pipeline_commands(cmd_list, envp, n, data);
	return (0);
}
