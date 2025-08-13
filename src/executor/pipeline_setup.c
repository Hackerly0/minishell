/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_setup.c                                  :+:      :+:    :+:    */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 14:51:27 by oalhoora         #+#    #+#              */
/*   Updated: 2025/08/12 14:51:27 by oalhoora        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	setup_pipeline_input(t_cmd *cur, int i, int **pipes, int *heredoc_fds)
{
	if (heredoc_fds && heredoc_fds[i] >= 0)
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
