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
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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

int	execute_single_command(t_cmd *cmd, char **envp)
{
	char	*cmd_path;
	char	*cmd_name;

	if (!cmd->argv[0])
		return (0);
	if (is_builtin_cmd(cmd->argv[0]))
	{
		cmd_name = ft_strrchr(cmd->argv[0], '/');
		if (cmd_name)
			cmd_name++;
		else
			cmd_name = cmd->argv[0];
		cmd->argv[0] = cmd_name;
		return (execute_builtin(cmd->argv, envp));
	}
	cmd_path = cmd->argv[0];
	if (cmd_path[0] != '/')
	{
		char *resolved_path = find_command_path(cmd_path, envp);
		if (resolved_path)
			cmd_path = resolved_path;
		else
		{
			free(cmd->token_list->value);
			free(cmd->token_list);
			fprintf(stderr, "minishell: %s: command not found\n", resolved_path);
			//free_cmd(cmd);
			return (127);
		}
	}
	execve(cmd_path, cmd->argv, envp);
	cmd_name = ft_strrchr(cmd->argv[0], '/');
	if (cmd_name)
		cmd_name++;
	else
		cmd_name = cmd->argv[0];
	if (errno == ENOENT)
		fprintf(stderr, "minishell: %s: command not found\n", cmd_name);
	else
		perror("execve");
	if (cmd_path != cmd->argv[0])
		free(cmd_path);
	free_cmd(cmd);
	return (127);
}

int	execute_command_line(t_token *tokens, char **envp)
{
	t_cmd	*cmds;
	int		code;

	if (!tokens)
		return (0);
	cmds = parse_tokens_to_commands(tokens);
	if (!cmds)
		return (0);
	cmds->token_list = tokens;
	code = execute_pipeline(cmds, envp);
	free_cmd_list(cmds);
	return (code);
}

static void	child_cleanup_and_exit(t_cmd *cmd_list, int **pipes,
								int n, int exit_code)
{
	free_cmd_list(cmd_list);
	free_pipe_array(pipes, n);
	exit(exit_code);
}

static void	fork_pipeline_commands(t_cmd *cmd_list, char **envp, int n)
{
	int		**pipes;
	int		*heredoc_fds;
	pid_t	*pids;
	t_cmd	*cur;
	int		i;

	heredoc_fds = create_heredoc_fds(cmd_list, n);
	pipes = create_pipe_array(n);
	pids = malloc(sizeof(pid_t) * n);
	cur = cmd_list;
	i = 0;
	while (i < n)
	{
		pids[i] = fork();
		if (pids[i] == 0)
		{
			 setup_pipeline_input(cur, i, pipes, heredoc_fds);
			 setup_pipeline_output(cur, i, pipes, n);
			 free(heredoc_fds);
			 free(pids);
			 close_all_pipes(pipes, n);
			 child_cleanup_and_exit(cmd_list, pipes, n,
			     execute_single_command(cur, envp));
		}
		else if (pids[i] < 0)
		{
			perror("fork");
			free_pipe_array(pipes, n);
			free(heredoc_fds);
			free(pids);
			return ;
		}
		cur = cur->next;
		i++;
	}
	free_pipe_array(pipes, n);
	wait_pipeline_processes(pids, n, heredoc_fds);
	free(pids);
	free(heredoc_fds);
}

int	wait_pipeline_processes(pid_t *pids, int n, int *heredoc_fds)
{
	int	status;
	int	last;
	int	i;

	last = 0;
	i = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		if (i == n - 1)
			last = WEXITSTATUS(status);
		if (heredoc_fds[i] >= 0)
			close(heredoc_fds[i]);
		i++;
	}
	return (last);
}

int	execute_pipeline(t_cmd *cmd_list, char **envp)
{
	int	n;

	n = count_commands(cmd_list);
	if (n == 1)
		return (single_command_handler(cmd_list, envp));
	fork_pipeline_commands(cmd_list, envp, n);
	return (0);
}
