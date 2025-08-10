/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haitham                                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 20:00:00 by haitham           #+#    #+#             */
/*   Updated: 2025/08/08 20:00:00 by haitham          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int  hd_save_stdin(int *saved)
{
    *saved = dup(STDIN_FILENO);
    if (*saved == -1)
        return (-1);
    return (0);
}

static int  hd_restore_stdin(int saved)
{
    if (saved >= 0)
    {
        if (dup2(saved, STDIN_FILENO) == -1)
        {
            close(saved);
            return (-1);
        }
        close(saved);
    }
    return (0);
}

static int  hd_abort(int saved, int wfd, char *in)
{
    if (in)
        free(in);
    close(wfd);
    hd_restore_stdin(saved);
    setup_signals();                 /* back to main handlers */
    return (-1);
}

static int  hd_finish(int saved, int wfd, char *in)
{
    (void)wfd;

    if (in)
        free(in);
    close(wfd);
    hd_restore_stdin(saved);
    setup_signals();                 /* back to main handlers */
    return (0);
}

static int  hd_write_line(int wfd, char *s)
{
    ssize_t n;
	char	*str;

    if (!s)
	{
        return (0);
	}
	str = expand_string(s);
    if (!str)
        str = ft_strdup("");
    if (!str)
        return (-1);
    n = write(wfd, str, ft_strlen(str));
    if (n < 0)
	{
        return (-1);
	}
    if (write(wfd, "\n", 1) < 0)
	{
        return (-1);
	}
	free(str);
    return (0);
}

static int hd_loop(const char *delim, int wfd)
{
    int     saved;
    char    *in;

    if (hd_save_stdin(&saved) == -1)
        return (-1);
    setup_hd_signals();              /* <<< heredoc mode */

    while (1)
    {
        in = readline("> ");

        /* if handler closed STDIN, readline returns NULL; abort on SIGINT */
        if (!in && g_signal == SIGINT)
        {
            g_signal = 0;
            return (hd_abort(saved, wfd, NULL));
        }
        if (!in || ft_strcmp(in, delim) == 0)
            break ;

        if (hd_write_line(wfd, in) == -1)
            return (hd_abort(saved, wfd, in));
        free(in);
    }
    return (hd_finish(saved, wfd, in));
}

void    set_cloexec(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        return ;
    fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

static int  hd_make_pipe(int pipefd[2])
{
    if (pipe(pipefd) == -1)
        return (-1);
    set_cloexec(pipefd[0]);
    set_cloexec(pipefd[1]);
    return (0);
}

/* public API **************************************************************** */

int create_heredoc_pipe(const char *delim, int *out_fd)
{
    int pipefd[2];

    if (!delim || !out_fd)
        return (-1);
    if (hd_make_pipe(pipefd) == -1)
        return (-1);
    if (hd_loop(delim, pipefd[1]) == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return (-1);
    }
    close(pipefd[1]);
    *out_fd = pipefd[0];
    return (0);
}

static int  hd_assign_fd_for_cmd(t_cmd *c, int *dst)
{
    if (!c || !c->heredoc_delim)
    {
        *dst = -1;
        return (0);
    }
    if (create_heredoc_pipe(c->heredoc_delim, dst) == -1)
    {
        *dst = -1;
        return (-1);
    }
    return (0);
}

int *create_heredoc_fds(t_cmd *cmds, int n)
{
    int     *fds;
    int     i;
    t_cmd   *cur;

    fds = (int *)malloc(sizeof(int) * n);
    if (!fds)
        return (NULL);
    i = 0;
    while (i < n)
    {
        fds[i++] = -1;
    } 
    cur = cmds;
    i = 0;
    while (i < n && cur)
    {
        if (hd_assign_fd_for_cmd(cur, &fds[i]) == -1)
        {
            close_heredoc_fds(fds, i + 1);
            free(fds);
            return (NULL);
        }
        cur = cur->next;
        i++;
    }
    return (fds);
}

int setup_single_cmd_input(t_cmd *cmd)
{
    int hd_fd;

    if (cmd && cmd->heredoc_delim)
    {
        if (create_heredoc_pipe(cmd->heredoc_delim, &hd_fd) == -1)
            return (-1);
        if (dup2(hd_fd, STDIN_FILENO) == -1)
        {
            close(hd_fd);
            return (-1);
        }
        close(hd_fd);
        return (0);
    }
    return (setup_input_redirection(cmd));
}
