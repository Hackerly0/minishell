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
void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

// Free a NULL-terminated array of strings

// Create and initialize a t_cmd node


// Set up input redirection or heredoc in a child

// Count how many commands are in the pipeline
int	count_commands(t_cmd *cmd_list)
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
int	is_builtin_cmd(char *cmd_path)
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
int	execute_single_command(t_cmd *cmd, char **envp)
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
// static int	execute_pipeline(t_cmd *cmd_list, char **envp)
// {
// 	int n = count_commands(cmd_list);

// 	// Single command (builtin in parent or fork+exec)
// 	if (n == 1)
// 		return single_command_handler(cmd_list, envp);

// 	// Multiple commands -> pipelines
// 	int **pipes = malloc(sizeof(int *) * (n - 1));
// 	for (int i = 0; i < n - 1; i++)
// 	{
// 		pipes[i] = malloc(sizeof(int) * 2);
// 		if (pipe(pipes[i]) == -1)
// 			error_exit("pipe");
// 	}
// 	pid_t *pids = malloc(sizeof(pid_t) * n);
// 	t_cmd *cur = cmd_list;

// 	for (int i = 0; i < n; i++, cur = cur->next)
// 	{
// 		if ((pids[i] = fork()) == 0)
// 		{
// 			// Input
// 			if (i > 0)
// 				dup2(pipes[i - 1][0], STDIN_FILENO);
// 			else
// 				setup_input_redirection(cur);

// 			// Output
// 			if (i < n - 1)
// 				dup2(pipes[i][1], STDOUT_FILENO);
// 			else
// 				setup_output_redirection(cur);

// 			// Close all pipes
// 			for (int j = 0; j < n - 1; j++)
// 			{
// 				close(pipes[j][0]);
// 				close(pipes[j][1]);
// 			}
// 			exit(execute_single_command(cur, envp));
// 		}
// 	}

// 	// Parent closes pipes, waits
// 	for (int i = 0; i < n - 1; i++)
// 	{
// 		close(pipes[i][0]);
// 		close(pipes[i][1]);
// 		free(pipes[i]);
// 	}
// 	free(pipes);

// 	int status, last = 0;
// 	for (int i = 0; i < n; i++)
// 	{
// 		waitpid(pids[i], &status, 0);
// 		if (i == n - 1) last = WEXITSTATUS(status);
// 	}
// 	free(pids);
// 	return last;
// }

#include "../../includes/minishell.h"

static int create_heredoc_pipe(const char *delim, int *out_fd)
{
    int  pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1)
        return -1;
    pid = fork();
    if (pid < 0)
        return -1;
    if (pid == 0)
    {
        close(pipefd[0]);
        while (1)
        {
            char *line = readline("> ");
            if (!line || !strcmp(line, delim))
            {
                free(line);
                break;
            }
            write(pipefd[1], line, strlen(line));
            write(pipefd[1], "\n", 1);
            free(line);
        }
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
    *out_fd = pipefd[0];
    return 0;
}

static int execute_pipeline(t_cmd *cmd_list, char **envp)
{
    int   n = count_commands(cmd_list);
    int  *heredoc_fds = malloc(sizeof(int) * n);
    t_cmd *c = cmd_list;

    // 1) Pre-read here-docs into pipes
    for (int i = 0; i < n; i++, c = c->next)
    {
        if (c->heredoc_delim)
        {
            if (create_heredoc_pipe(c->heredoc_delim, &heredoc_fds[i]) < 0)
                heredoc_fds[i] = -1;
        }
        else
            heredoc_fds[i] = -1;
    }

    // Single command?
    if (n == 1)
    {
        int status = single_command_handler(cmd_list, envp);
        free(heredoc_fds);
        return status;
    }

    // 2) Create pipes
    int **pipes = malloc(sizeof(int *) * (n - 1));
    for (int i = 0; i < n - 1; i++)
    {
        pipes[i] = malloc(sizeof(int) * 2);
        if (pipe(pipes[i]) == -1)
            error_exit("pipe");
    }

    // 3) Fork each stage
    pid_t *pids = malloc(sizeof(pid_t) * n);
    t_cmd *cur  = cmd_list;
    for (int i = 0; i < n; i++, cur = cur->next)
    {
        if ((pids[i] = fork()) == 0)
        {
            // stdin: heredoc?
            if (heredoc_fds[i] >= 0)
            {
                dup2(heredoc_fds[i], STDIN_FILENO);
                close(heredoc_fds[i]);
            }
            else if (cur->input_file)
            {
                if (setup_input_redirection(cur) < 0)
                    exit(1);
            }
            else if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // stdout
            if (cur->output_file)
            {
                if (setup_output_redirection(cur) < 0)
                    exit(1);
            }
            else if (i < n - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // close all fds
            for (int j = 0; j < n - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            exit(execute_single_command(cur, envp));
        }
    }

    // 4) Parent closes pipes & frees
    for (int i = 0; i < n - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
        free(pipes[i]);
    }
    free(pipes);

    // 5) Wait and collect last exit status
    int status, last = 0;
    for (int i = 0; i < n; i++)
    {
        waitpid(pids[i], &status, 0);
        if (i == n - 1)
            last = WEXITSTATUS(status);
        if (heredoc_fds[i] >= 0)
            close(heredoc_fds[i]);  // cleanup any unused heredoc FDs
    }
    free(pids);
    free(heredoc_fds);
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
