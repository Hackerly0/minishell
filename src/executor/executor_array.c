/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: <login> <login@student.42.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 12:00:00 by <login>           #+#    #+#             */
/*   Updated: 2025/07/01 12:00:00 by <login>          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int	g_last_exit_code;

typedef struct s_exec_ctx
{
	t_command	*cmds;
	int			num_cmds;
	char		**envp;
	int		pipes[2][2];
}	t_exec_ctx;

static void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static void	close_pipes(int pipes[2][2])
{
	int	j;

	j = 0;
	while (j < 2)
	{
		if (pipes[j][0] >= 0)
			close(pipes[j][0]);
		if (pipes[j][1] >= 0)
			close(pipes[j][1]);
		pipes[j][0] = -1;
		pipes[j][1] = -1;
		j++;
	}
}

static void	spawn_child(t_exec_ctx *ctx, int idx)
{
	int	in_fd;
	int	out_fd;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	in_fd = STDIN_FILENO;
	if (idx > 0)
		in_fd = ctx->pipes[(idx - 1) % 2][0];
	if (dup2(in_fd, STDIN_FILENO) < 0)
		error_exit("dup2 stdin");
	out_fd = STDOUT_FILENO;
	if (idx < ctx->num_cmds - 1)
		out_fd = ctx->pipes[idx % 2][1];
	if (dup2(out_fd, STDOUT_FILENO) < 0)
		error_exit("dup2 stdout");
	close_pipes(ctx->pipes);
	execve("/bin/sh",
		(char *[4]){"sh", "-c", ctx->cmds[idx].argv[0], NULL},
		ctx->envp);
	error_exit("execve");
}

static void	launch_children(t_exec_ctx *ctx)
{
	int		i;
	int		prev;
	pid_t	pid;

	i = 0;
	while (i < ctx->num_cmds)
	{
		if (i < ctx->num_cmds - 1
			&& pipe(ctx->pipes[i % 2]) < 0)
			error_exit("pipe");
		pid = fork();
		if (pid < 0)
			error_exit("fork");
		if (pid == 0)
			spawn_child(ctx, i);
		if (i > 0)
		{
			prev = (i - 1) % 2;
			close(ctx->pipes[prev][0]);
			close(ctx->pipes[prev][1]);
		}
		i++;
	}
}

static void	wait_children(int num_cmds)
{
	int	i;
	int	status;

	i = 0;
	while (i < num_cmds)
	{
		if (wait(&status) < 0)
			error_exit("wait");
		if (WIFEXITED(status))
			g_last_exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			g_last_exit_code = 128 + WTERMSIG(status);
		i++;
	}
}

void	executor(t_command *cmds, int num_cmds, char **envp)
{
	t_exec_ctx	ctx;

	ctx.cmds = cmds;
	ctx.num_cmds = num_cmds;
	ctx.envp = envp;
	ctx.pipes[0][0] = -1;
	ctx.pipes[0][1] = -1;
	ctx.pipes[1][0] = -1;
	ctx.pipes[1][1] = -1;
	launch_children(&ctx);
	wait_children(num_cmds);
}