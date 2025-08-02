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

void	setup_pipeline_input(t_cmd *cur, int i, int **pipes, int *heredoc_fds)
{
	if (heredoc_fds[i] >= 0)
	{
		dup2(heredoc_fds[i], STDIN_FILENO);
		close(heredoc_fds[i]);
	}
	else if (cur->input_file)
		setup_input_redirection(cur);
	else if (i > 0)
		dup2(pipes[i - 1][0], STDIN_FILENO);
}

void	setup_pipeline_output(t_cmd *cur, int i, int **pipes, int n)
{
	if (cur->output_file)
		setup_output_redirection(cur);
	else if (i < n - 1)
		dup2(pipes[i][1], STDOUT_FILENO);
}

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
