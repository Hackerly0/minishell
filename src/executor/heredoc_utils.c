/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 23:57:33 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/12 23:54:12 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	hd_save_stdin(int *saved)
{
	if (!saved)
		return (-1);
	*saved = dup(STDIN_FILENO);
	if (*saved == -1)
		return (-1);
	return (0);
}

int	hd_restore_stdin(int saved)
{
	int	ret;

	ret = 0;
	if (saved >= 0)
	{
		if (dup2(saved, STDIN_FILENO) == -1)
			ret = -1;
		if (close(saved) == -1)
			ret = -1;
		close(saved);
	}
	return (ret);
}

int	hd_abort(int saved, int wfd, char *in)
{
	if (in)
		free(in);
	close(wfd);
	hd_restore_stdin(saved);
	g_signal = 0;
	setup_signals();
	return (-1);
}

int	hd_finish(int saved, int wfd, char *in)
{
	if (in)
		free(in);
	close(wfd);
	hd_restore_stdin(saved);
	setup_signals();
	g_signal = 0;
	return (0);
}

int	hd_write_line(int wfd, char *s, t_data *data)
{
	ssize_t	n;
	char	*str;

	if (!s)
		return (0);
	str = expand_string(s, data);
	if (!str)
		str = ft_strdup("");
	if (!str)
		return (-1);
	n = write(wfd, str, ft_strlen(str));
	if (n < 0)
		return (-1);
	if (write(wfd, "\n", 1) < 0)
		return (-1);
	free(str);
	return (0);
}
