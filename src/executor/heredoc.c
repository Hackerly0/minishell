#include "../../includes/minishell.h"

static void	heredoc_reader(int write_fd, char *delim)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, delim))
		{
			if (line)
				free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
	close(write_fd);
	exit(EXIT_SUCCESS);
}

int	create_heredoc_pipe(const char *delim, int *out_fd)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		heredoc_reader(pipefd[1], (char *)delim);
	}
	close(pipefd[1]);
	waitpid(pid, NULL, 0);
	*out_fd = pipefd[0];
	return (0);
}

int	*create_heredoc_fds(t_cmd *cmd_list, int n)
{
	int		*heredoc_fds;
	t_cmd	*c;
	int		i;

	heredoc_fds = malloc(sizeof(int) * n);
	if (!heredoc_fds)
		return (NULL);
	c = cmd_list;
	i = 0;
	while (i < n)
	{
		if (c->heredoc_delim)
		{
			if (create_heredoc_pipe(c->heredoc_delim, &heredoc_fds[i]) < 0)
				heredoc_fds[i] = -1;
		}
		else
			heredoc_fds[i] = -1;
		c = c->next;
		i++;
	}
	return (heredoc_fds);
}

int	setup_single_cmd_input(t_cmd *cmd)
{
	int	heredoc_fd;

	if (cmd->heredoc_delim)
	{
		if (create_heredoc_pipe(cmd->heredoc_delim, &heredoc_fd) < 0)
			return (-1);
		dup2(heredoc_fd, STDIN_FILENO);
		close(heredoc_fd);
	}
	else if (setup_input_redirection(cmd) == -1)
		return (-1);
	return (0);
}
