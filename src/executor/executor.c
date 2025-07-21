/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 20:00:02 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Error helper
static void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

// Free a NULL-terminated array of strings
static void	free_array(char **arr)
{
	if (!arr) return;
	for (int i = 0; arr[i]; i++)
		free(arr[i]);
	free(arr);
}

// Create and initialize a t_cmd node
static t_cmd	*create_cmd(void)
{
	t_cmd *cmd = malloc(sizeof(t_cmd));
	if (!cmd) return NULL;
	cmd->argv = malloc(sizeof(char *) * 256);
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->heredoc_delim = NULL;
	cmd->append_mode = 0;
	cmd->next = NULL;
	return cmd;
}

// Free one t_cmd node
static void	free_cmd(t_cmd *cmd)
{
	if (!cmd) return;
	free_array(cmd->argv);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd->heredoc_delim);
	free(cmd);
}

// Free a whole pipeline
static void	free_cmd_list(t_cmd *cmd_list)
{
	while (cmd_list)
	{
		t_cmd *next = cmd_list->next;
		free_cmd(cmd_list);
		cmd_list = next;
	}
}

// Turn a token list into a linked list of t_cmd
static t_cmd	*parse_tokens_to_commands(t_token *tokens)
{
	t_cmd *cmd_list = NULL, *current = NULL, *last = NULL;
	int argc = 0;

	while (tokens)
	{
		if (tokens->type == T_PIPE)
		{
			if (current)
			{
				current->argv[argc] = NULL;
				if (!cmd_list) cmd_list = current;
				else last->next = current;
				last = current;
			}
			current = create_cmd();
			argc = 0;
		}
		else if (tokens->type == T_COMMAND)
		{
			// Handle T_COMMAND tokens - path is already resolved in value
            printf("DEBUG: Command token value: '%s'\n", tokens->value);
			if (!current) current = create_cmd();
			current->argv[argc++] = strdup(tokens->value);
		}
		else if (tokens->type == T_WORD || tokens->type == T_ARGUMENT || tokens->type == T_OPTION)
		{
			// Handle other command components
			if (!current) current = create_cmd();
			current->argv[argc++] = strdup(tokens->value);
		}
		else if (tokens->type == T_IN_REDIR)
		{
			if (!current) current = create_cmd();
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
				current->input_file = strdup(tokens->value);
		}
		else if (tokens->type == T_OUT_REDIR || tokens->type == T_DOUT_REDIR)
		{
			if (!current) current = create_cmd();
			int append = (tokens->type == T_DOUT_REDIR);
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
			{
				current->output_file = strdup(tokens->value);
				current->append_mode = append;
			}
		}
		else if (tokens->type == T_HEREDOC)
		{
			if (!current) current = create_cmd();
			tokens = tokens->next;
			if (tokens && tokens->type == T_WORD)
				current->heredoc_delim = strdup(tokens->value);
		}
		tokens = tokens->next;
	}

	if (current)
	{
		current->argv[argc] = NULL;
		if (!cmd_list) cmd_list = current;
		else last->next = current;
	}
	return cmd_list;
}

// Set up input redirection or heredoc in a child
static int	setup_input_redirection(t_cmd *cmd)
{
	if (cmd->heredoc_delim)
	{
		int pipefd[2];
		if (pipe(pipefd) == -1)
			return -1;
		if (fork() == 0)
		{
			close(pipefd[0]);
			char *line;
			while ((line = readline("> ")))
			{
				if (strcmp(line, cmd->heredoc_delim) == 0)
				{
					free(line);
					break;
				}
				write(pipefd[1], line, strlen(line));
				write(pipefd[1], "\n", 1);
				free(line);
			}
			close(pipefd[1]);
			exit(0);
		}
		close(pipefd[1]);
		wait(NULL);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
	}
	else if (cmd->input_file)
	{
		int fd = open(cmd->input_file, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->input_file);
			return -1;
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	return 0;
}

// Set up output redirection
static int	setup_output_redirection(t_cmd *cmd)
{
	if (cmd->output_file)
	{
		int flags = O_WRONLY | O_CREAT | (cmd->append_mode ? O_APPEND : O_TRUNC);
		int fd = open(cmd->output_file, flags, 0644);
		if (fd == -1)
		{
			perror(cmd->output_file);
			return -1;
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return 0;
}

// Count how many commands are in the pipeline
static int	count_commands(t_cmd *cmd_list)
{
	int count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return count;
}

// Check if a command is a builtin by checking the original command name
static int	is_builtin_cmd(char *cmd_path)
{
	char *cmd_name;
	
	// Extract command name from full path
	cmd_name = strrchr(cmd_path, '/');
	if (cmd_name)
		cmd_name++; // Skip the '/'
	else
		cmd_name = cmd_path; // No path, just the command name
	
	return is_builtin(cmd_name);
}

//Execute a single command - with fallback PATH resolution
static int	execute_single_command(t_cmd *cmd, char **envp)
{
	if (!cmd->argv[0]) return 0;
	
	// Check if it's a builtin using the command name (not full path)
	if (is_builtin_cmd(cmd->argv[0]))
	{
		// Extract command name for builtin execution
		char *cmd_name = strrchr(cmd->argv[0], '/');
		if (cmd_name)
			cmd_name++; // Skip the '/'
		else
			cmd_name = cmd->argv[0]; // No path, just the command name
		
		// Temporarily replace argv[0] with just the command name for builtin
		char *original_argv0 = cmd->argv[0];
		cmd->argv[0] = cmd_name;
		int result = execute_builtin(cmd->argv, envp);
		cmd->argv[0] = original_argv0; // Restore original
		return result;
	}

	// Try to execute with current path first
	char *cmd_path = cmd->argv[0];
	
	// If it's not an absolute path and execve might fail, try PATH resolution
	if (cmd_path[0] != '/')
	{
		char *resolved_path = find_command_path(cmd_path, envp);
		if (resolved_path)
			cmd_path = resolved_path;
	}
	
	execve(cmd_path, cmd->argv, envp);
	
	// If execve returns, it failed
	char *cmd_name = strrchr(cmd->argv[0], '/');
	if (cmd_name)
		cmd_name++; // Skip the '/'
	else
		cmd_name = cmd->argv[0];
		
	if (errno == ENOENT)
		fprintf(stderr, "minishell: %s: command not found\n", cmd_name);
	else
		perror("execve");
	
	// Free resolved path if it was allocated
	if (cmd_path != cmd->argv[0])
		free(cmd_path);
		
	return 127;
}

// Simple execute_single_command function
// static int execute_single_command(t_cmd *cmd, char **envp)
// {
//     // Check if we have a command
//     if (!cmd->argv[0])
//         return 0;
    
//     // Extract command name (in case it's a full path)
//     char *cmd_name = strrchr(cmd->argv[0], '/');
//     if (cmd_name)
//         cmd_name++; // Skip the '/'
//     else
//         cmd_name = cmd->argv[0]; // No path, just the command name
    
//     // Check if it's a builtin command
//     if (is_builtin(cmd_name))
//     {
//         // For builtins, use just the command name
//         char *original_argv0 = cmd->argv[0];
//         cmd->argv[0] = cmd_name;
//         int result = execute_builtin(cmd->argv, envp);
//         cmd->argv[0] = original_argv0; // Restore original
//         return result;
//     }
    
//     // For external commands, use the full path from tokenizer
//     execve(cmd->argv[0], cmd->argv, envp);
    
//     // If we get here, execve failed
//     if (errno == ENOENT)
//         fprintf(stderr, "minishell: %s: command not found\n", cmd_name);
//     else
//         fprintf(stderr, "minishell: %s: %s\n", cmd_name, strerror(errno));
    
//     return 127;
// }

static int single_command_handler(t_cmd *cmd_list, char **envp)
{
	// Handle builtins in parent
	if (cmd_list->argv[0] && is_builtin_cmd(cmd_list->argv[0]))
	{
		int stdin_backup = dup(STDIN_FILENO);
		int stdout_backup = dup(STDOUT_FILENO);

		if (setup_input_redirection(cmd_list) == -1 ||
			setup_output_redirection(cmd_list) == -1)
		{
			dup2(stdin_backup, STDIN_FILENO);
			dup2(stdout_backup, STDOUT_FILENO);
			close(stdin_backup);
			close(stdout_backup);
			return 1;
		}

		// Extract command name for builtin execution
		char *cmd_name = strrchr(cmd_list->argv[0], '/');
		if (cmd_name)
			cmd_name++; // Skip the '/'
		else
			cmd_name = cmd_list->argv[0]; // No path, just the command name
		
		// Temporarily replace argv[0] with just the command name for builtin
		char *original_argv0 = cmd_list->argv[0];
		cmd_list->argv[0] = cmd_name;
		int exit_code = execute_builtin(cmd_list->argv, envp);
		cmd_list->argv[0] = original_argv0; // Restore original

		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);

		return exit_code;
	}

	// External command in child
	pid_t pid = fork();
	if (pid == 0)
	{
		setup_input_redirection(cmd_list);
		setup_output_redirection(cmd_list);
		exit(execute_single_command(cmd_list, envp));
	}
	else
	{
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}
}

// Execute one or more commands connected by pipes
static int	execute_pipeline(t_cmd *cmd_list, char **envp)
{
	int n = count_commands(cmd_list);

	// Single command (builtin in parent or fork+exec)
	if (n == 1)
		return single_command_handler(cmd_list, envp);

	// Multiple commands -> pipelines
	int **pipes = malloc(sizeof(int *) * (n - 1));
	for (int i = 0; i < n - 1; i++)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (pipe(pipes[i]) == -1)
			error_exit("pipe");
	}
	pid_t *pids = malloc(sizeof(pid_t) * n);
	t_cmd *cur = cmd_list;

	for (int i = 0; i < n; i++, cur = cur->next)
	{
		if ((pids[i] = fork()) == 0)
		{
			// Input
			if (i > 0)
				dup2(pipes[i - 1][0], STDIN_FILENO);
			else
				setup_input_redirection(cur);

			// Output
			if (i < n - 1)
				dup2(pipes[i][1], STDOUT_FILENO);
			else
				setup_output_redirection(cur);

			// Close all pipes
			for (int j = 0; j < n - 1; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			exit(execute_single_command(cur, envp));
		}
	}

	// Parent closes pipes, waits
	for (int i = 0; i < n - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
	}
	free(pipes);

	int status, last = 0;
	for (int i = 0; i < n; i++)
	{
		waitpid(pids[i], &status, 0);
		if (i == n - 1) last = WEXITSTATUS(status);
	}
	free(pids);
	return last;
}

// Top-level executor
int	execute_command_line(t_token *tokens, char **envp)
{
	if (!tokens) return 0;
	t_cmd *cmds = parse_tokens_to_commands(tokens);
	if (!cmds) return 0;
	int code = execute_pipeline(cmds, envp);
	free_cmd_list(cmds);
	return code;
}

// Signal handling for readline
static void	signal_handler(int sig)
{
	g_signal = sig;
	if (sig == SIGINT)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

// Set up signal handlers
void	setup_signals(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}