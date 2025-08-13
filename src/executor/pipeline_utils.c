/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 00:00:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	close_all_pipes(int **pipes, int n)
{
	int	j;

	j = 0;
	while (j < n - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
}

int	**create_pipe_array(int n)
{
	int	**pipes;
	int	i;

	pipes = malloc(sizeof(int *) * (n - 1));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < n - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (pipe(pipes[i]) == -1)
			error_exit("pipe");
		i++;
	}
	return (pipes);
}

void	free_pipe_array(int **pipes, int n)
{
	int	i;

	i = 0;
	while (i < n - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

void	close_heredoc_fds(int *fds, int n)
{
	int	i;

	if (!fds)
		return ;
	i = 0;
	while (i < n)
	{
		if (fds[i] >= 0)
			close(fds[i]);
		i++;
	}
}

void	close_other_heredoc_fds(int *fds, int n, int keep)
{
	int	i;

	if (!fds)
		return ;
	i = 0;
	while (i < n)
	{
		if (i != keep && fds[i] >= 0)
			close(fds[i]);
		i++;
	}
}
