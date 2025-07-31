#include "../../includes/minishell.h"
void	heredoc_child(int write_fd, char *delim)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (!ft_strcmp(line, delim))
		{
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

int	handle_heredoc(char *delim)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
	{
		close(pipefd[0]);
		heredoc_child(pipefd[1], delim);
	}
	close(pipefd[1]);
	waitpid(pid, NULL, 0);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		return (-1);
	close(pipefd[0]);
	return (0);
}

int	setup_input_redirection(t_cmd *cmd)
{
	int	fd;

	if (cmd->heredoc_delim)
		return (handle_heredoc(cmd->heredoc_delim));
	if (cmd->input_file)
	{
		fd = open(cmd->input_file, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->input_file);
			return (-1);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
			return (-1);
		close(fd);
	}
	return (0);
}

int	setup_output_redirection(t_cmd *cmd)
{
	int	flags;
	int	fd;

	fd = 0;
	flags = 0;
	if (cmd->output_file)
	{
		flags = O_WRONLY | O_CREAT;
		if (cmd->append_mode)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		fd = open(cmd->output_file, flags, 0644);
		if (fd == -1)
		{
			perror(cmd->output_file);
			return (-1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (0);
}