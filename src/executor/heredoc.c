/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                         :+:      :+:    :+:    */
/*                                                    +:+ +:+         +:+     */
/*   By: haitham                                      +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 20:00:00 by haitham           #+#    #+#             */
/*   Updated: 2025/08/08 20:00:00 by haitham          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	if_statements(char *in, const char *delim, int wfd, t_data *data)
{
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		return (-1);
	}
	if (!in || ft_strcmp(in, delim) == 0)
	{
		return (0);
	}
	if (hd_write_line(wfd, in, data) == -1)
	{
		return (-1);
	}
	return (1);
}

static int	hd_loop(const char *delim, int wfd, t_data *data)
{
	int		saved;
	char	*in;
	int		ret;

	if (hd_save_stdin(&saved) == -1)
		return (-1);
	setup_hd_signals();
	ret = 0;
	in = NULL;
	while (1)
	{
		in = readline("> ");
		ret = if_statements(in, delim, wfd, data);
		if (ret != 1)
			break ;
		ret = 0;
		free(in);
		in = NULL;
	}
	if (in)
		free(in);
	close(wfd);
	hd_restore_stdin(saved);
	setup_signals();
	return (ret);
}

int	create_heredoc_pipe(const char *delim, int *out_fd, t_data *data)
{
	int	pipefd[2];

	if (!delim || !out_fd)
		return (-1);
	if (hd_make_pipe(pipefd) == -1)
		return (-1);
	if (hd_loop(delim, pipefd[1], data) == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	close(pipefd[1]);
	*out_fd = pipefd[0];
	return (0);
}

static int	hd_assign_fd_for_cmd(t_cmd *c, int *dst, t_data *data)
{
	if (!c || !c->heredoc_delim)
	{
		*dst = -1;
		return (0);
	}
	if (create_heredoc_pipe(c->heredoc_delim, dst, data) == -1)
	{
		*dst = -1;
		return (-1);
	}
	return (0);
}

int	*create_heredoc_fds(t_cmd *cmds, int n, t_data *data)
{
	int		*fds;
	int		i;
	t_cmd	*cur;

	fds = (int *)malloc(sizeof(int) * n);
	if (!fds)
		return (NULL);
	i = 0;
	while (i < n)
		fds[i++] = -1;
	cur = cmds;
	i = -1;
	while (++i < n && cur)
	{
		if (hd_assign_fd_for_cmd(cur, &fds[i], data) == -1)
		{
			close_heredoc_fds(fds, i + 1);
			free(fds);
			return (NULL);
		}
		cur = cur->next;
	}
	return (fds);
}
