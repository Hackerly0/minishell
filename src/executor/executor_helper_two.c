/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_helper_two.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:46:38 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 01:07:37 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	child_cleanup_and_exit(t_cmd *cmd_list, int **pipes,
								t_temp_child child, t_env **envp)
{
	free_pipe_array(pipes, child.n);
	(void)cmd_list;
	(void)envp;
	free_tokens(&(cmd_list->token_list));
	free_cmd_list(cmd_list);
	env_free_list(*envp);
	exit(child.exit_code);
}

int	init_pipeline_data(t_temp_pipeline *temp, t_cmd *cmd_list,
					int n, t_data *data)
{
	temp->heredoc_fds = create_heredoc_fds(cmd_list, n, data);
	if (!temp->heredoc_fds)
		return (0);
	temp->pipes = create_pipe_array(n);
	if (!temp->pipes)
		return (0);
	temp->pids = malloc(sizeof(pid_t) * n);
	if (!temp->pids)
		return (0);
	temp->cur = cmd_list;
	temp->i = 0;
	return (1);
}

void	child_code_pipeline(t_temp_pipeline *temp, t_cmd *cmd_list,
								t_env **envp, int n)
{
	t_temp_child	child;

	child.n = n;
	if (temp->heredoc_fds && temp->heredoc_fds[temp->i] >= 0)
	{
		if (dup2(temp->heredoc_fds[temp->i], STDIN_FILENO) == -1)
		{
			child.exit_code = execute_single_command(temp->cur, envp);
			child_cleanup_and_exit(cmd_list, temp->pipes, child, envp);
		}
		close(temp->heredoc_fds[temp->i]);
	}
	close_other_heredoc_fds(temp->heredoc_fds, n, temp->i);
	setup_pipeline_input(temp->cur, temp->i, temp->pipes, temp->heredoc_fds);
	setup_pipeline_output(temp->cur, temp->i, temp->pipes, n);
	close_all_pipes(temp->pipes, n);
	free(temp->pids);
	free(temp->heredoc_fds);
	child.exit_code = execute_single_command(temp->cur, envp);
	child_cleanup_and_exit(cmd_list, temp->pipes, child, envp);
}

void	error_fd_pipe(t_temp_pipeline *temp, int n)
{
	perror("fork");
	close_heredoc_fds(temp->heredoc_fds, n);
	free(temp->heredoc_fds);
	free_pipe_array(temp->pipes, n);
	free(temp->pids);
}

int	pipeline_cont(t_temp_pipeline *temp, int n)
{
	int status;

	free_pipe_array(temp->pipes, n);
	status = wait_pipeline_processes(temp->pids, n);
	close_heredoc_fds(temp->heredoc_fds, n);
	free(temp->heredoc_fds);
	temp->heredoc_fds = NULL;
	free(temp->pids);
	temp->pids = NULL;
	return (status);
}
