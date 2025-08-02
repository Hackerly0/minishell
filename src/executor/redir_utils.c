#include "../../includes/minishell.h"

int	setup_input_redirection(t_cmd *cmd)
{
	int	fd;

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
