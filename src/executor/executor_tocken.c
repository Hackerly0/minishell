/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/11 00:00:00 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Global variable to store signal information
volatile sig_atomic_t g_signal = 0;

// ─── Data Structures ───────────────────────────────────────────────────────

typedef struct s_cmd
{
    char                **argv;         // Command arguments
    char                *input_file;    // Input redirection file
    char                *output_file;   // Output redirection file
    char                *heredoc_delim; // Heredoc delimiter
    int                 append_mode;    // Append mode for output
    struct s_cmd        *next;          // Next command in pipeline
}                       t_cmd;

typedef struct s_exec_context
{
    t_cmd   *cmd_list;      // List of commands to execute
    char    **envp;         // Environment variables
    int     last_exit_code; // Last command exit code
    int     pipe_count;     // Number of pipes in command
}           t_exec_context;

// ─── Helper Functions ──────────────────────────────────────────────────────

static void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static char **ft_split_simple(const char *s, char delim)
{
    // Simple split implementation - replace with your libft version
    char **result = malloc(sizeof(char *) * 256);
    int count = 0;
    char *tmp = strdup(s);
    char *token = strtok(tmp, &delim);
    
    while (token && count < 255)
    {
        result[count++] = strdup(token);
        token = strtok(NULL, &delim);
    }
    result[count] = NULL;
    free(tmp);
    return result;
}

static void free_array(char **arr)
{
    if (!arr) return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

static char *find_command_path(char *cmd, char **envp)
{
    if (strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return strdup(cmd);
        return NULL;
    }
    
    char *path_env = NULL;
    for (int i = 0; envp[i]; i++)
    {
        if (strncmp(envp[i], "PATH=", 5) == 0)
        {
            path_env = envp[i] + 5;
            break;
        }
    }
    
    if (!path_env) return NULL;
    
    char **paths = ft_split_simple(path_env, ':');
    char *full_path = NULL;
    
    for (int i = 0; paths[i]; i++)
    {
        size_t len = strlen(paths[i]) + strlen(cmd) + 2;
        full_path = malloc(len);
        snprintf(full_path, len, "%s/%s", paths[i], cmd);
        
        if (access(full_path, X_OK) == 0)
        {
            free_array(paths);
            return full_path;
        }
        free(full_path);
    }
    
    free_array(paths);
    return NULL;
}

// ─── Built-in Commands ─────────────────────────────────────────────────────

static int builtin_echo(char **argv)
{
    int newline = 1;
    int i = 1;
    
    if (argv[1] && strcmp(argv[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }
    
    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
        i++;
    }
    
    if (newline)
        printf("\n");
    
    return 0;
}

static int builtin_cd(char **argv)
{
    char *path = argv[1];
    
    if (!path)
        path = getenv("HOME");
    
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }
    
    return 0;
}

static int builtin_pwd(char **argv)
{
    (void)argv;
    char *cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("pwd");
        return 1;
    }
    
    printf("%s\n", cwd);
    free(cwd);
    return 0;
}

static int builtin_env(char **argv, char **envp)
{
    (void)argv;
    for (int i = 0; envp[i]; i++)
        printf("%s\n", envp[i]);
    return 0;
}

static int builtin_exit(char **argv)
{
    int exit_code = 0;
    
    if (argv[1])
        exit_code = atoi(argv[1]);
    
    printf("exit\n");
    exit(exit_code);
}

static int is_builtin(char *cmd)
{
    return (strcmp(cmd, "echo") == 0 || strcmp(cmd, "cd") == 0 ||
            strcmp(cmd, "pwd") == 0 || strcmp(cmd, "env") == 0 ||
            strcmp(cmd, "exit") == 0 || strcmp(cmd, "export") == 0 ||
            strcmp(cmd, "unset") == 0);
}

static int execute_builtin(char **argv, char **envp)
{
    if (strcmp(argv[0], "echo") == 0)
        return builtin_echo(argv);
    else if (strcmp(argv[0], "cd") == 0)
        return builtin_cd(argv);
    else if (strcmp(argv[0], "pwd") == 0)
        return builtin_pwd(argv);
    else if (strcmp(argv[0], "env") == 0)
        return builtin_env(argv, envp);
    else if (strcmp(argv[0], "exit") == 0)
        return builtin_exit(argv);
    // TODO: Implement export and unset
    
    return 1;
}

// ─── Command Parsing ───────────────────────────────────────────────────────

static t_cmd *create_cmd(void)
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

static void free_cmd(t_cmd *cmd)
{
    if (!cmd) return;
    
    free_array(cmd->argv);
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd->heredoc_delim);
    free(cmd);
}

static void free_cmd_list(t_cmd *cmd_list)
{
    while (cmd_list)
    {
        t_cmd *next = cmd_list->next;
        free_cmd(cmd_list);
        cmd_list = next;
    }
}

static t_cmd *parse_tokens_to_commands(t_token *tokens)
{
    t_cmd *cmd_list = NULL;
    t_cmd *current_cmd = NULL;
    t_cmd *last_cmd = NULL;
    int argc = 0;
    
    while (tokens)
    {
        if (tokens->type == T_PIPE)
        {
            if (current_cmd)
            {
                current_cmd->argv[argc] = NULL;
                if (!cmd_list)
                    cmd_list = current_cmd;
                else
                    last_cmd->next = current_cmd;
                last_cmd = current_cmd;
            }
            current_cmd = create_cmd();
            argc = 0;
        }
        else if (tokens->type == T_WORD)
        {
            if (!current_cmd)
                current_cmd = create_cmd();
            current_cmd->argv[argc++] = strdup(tokens->value);
        }
        else if (tokens->type == T_IN_REDIR)
        {
            if (!current_cmd)
                current_cmd = create_cmd();
            tokens = tokens->next;
            if (tokens && tokens->type == T_WORD)
                current_cmd->input_file = strdup(tokens->value);
        }
        else if (tokens->type == T_OUT_REDIR)
        {
            if (!current_cmd)
                current_cmd = create_cmd();
            tokens = tokens->next;
            if (tokens && tokens->type == T_WORD)
            {
                current_cmd->output_file = strdup(tokens->value);
                current_cmd->append_mode = 0;
            }
        }
        else if (tokens->type == T_DOUT_REDIR)
        {
            if (!current_cmd)
                current_cmd = create_cmd();
            tokens = tokens->next;
            if (tokens && tokens->type == T_WORD)
            {
                current_cmd->output_file = strdup(tokens->value);
                current_cmd->append_mode = 1;
            }
        }
        else if (tokens->type == T_HEREDOC)
        {
            if (!current_cmd)
                current_cmd = create_cmd();
            tokens = tokens->next;
            if (tokens && tokens->type == T_WORD)
                current_cmd->heredoc_delim = strdup(tokens->value);
        }
        
        tokens = tokens->next;
    }
    
    // Add the last command
    if (current_cmd)
    {
        current_cmd->argv[argc] = NULL;
        if (!cmd_list)
            cmd_list = current_cmd;
        else
            last_cmd->next = current_cmd;
    }
    
    return cmd_list;
}

// ─── File Redirection ──────────────────────────────────────────────────────

static int setup_input_redirection(t_cmd *cmd)
{
    if (cmd->heredoc_delim)
    {
        // Handle heredoc
        int pipefd[2];
        if (pipe(pipefd) == -1)
            return -1;
        
        pid_t pid = fork();
        if (pid == 0)
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
        else
        {
            close(pipefd[1]);
            waitpid(pid, NULL, 0);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }
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

static int setup_output_redirection(t_cmd *cmd)
{
    if (cmd->output_file)
    {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->append_mode)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
        
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

// ─── Pipeline Execution ────────────────────────────────────────────────────

static int count_commands(t_cmd *cmd_list)
{
    int count = 0;
    while (cmd_list)
    {
        count++;
        cmd_list = cmd_list->next;
    }
    return count;
}

static int execute_single_command(t_cmd *cmd, char **envp)
{
    if (!cmd->argv[0])
        return 0;
    
    if (is_builtin(cmd->argv[0]))
        return execute_builtin(cmd->argv, envp);
    
    char *path = find_command_path(cmd->argv[0], envp);
    if (!path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
        return 127;
    }
    
    execve(path, cmd->argv, envp);
    perror("execve");
    free(path);
    return 1;
}

static int execute_pipeline(t_cmd *cmd_list, char **envp)
{
    int cmd_count = count_commands(cmd_list);
    
    if (cmd_count == 1)
    {
        // Single command - handle built-ins in parent process
        if (cmd_list->argv[0] && is_builtin(cmd_list->argv[0]))
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
            
            int exit_code = execute_builtin(cmd_list->argv, envp);
            
            dup2(stdin_backup, STDIN_FILENO);
            dup2(stdout_backup, STDOUT_FILENO);
            close(stdin_backup);
            close(stdout_backup);
            
            return exit_code;
        }
        else
        {
            // Single external command
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
    }
    
    // Pipeline execution
    int **pipes = malloc(sizeof(int *) * (cmd_count - 1));
    for (int i = 0; i < cmd_count - 1; i++)
    {
        pipes[i] = malloc(sizeof(int) * 2);
        if (pipe(pipes[i]) == -1)
            error_exit("pipe");
    }
    
    pid_t *pids = malloc(sizeof(pid_t) * cmd_count);
    t_cmd *current = cmd_list;
    
    for (int i = 0; i < cmd_count; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            // Child process
            
            // Setup input
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            else
            {
                setup_input_redirection(current);
            }
            
            // Setup output
            if (i < cmd_count - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            else
            {
                setup_output_redirection(current);
            }
            
            // Close all pipe fds
            for (int j = 0; j < cmd_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            exit(execute_single_command(current, envp));
        }
        current = current->next;
    }
    
    // Close all pipes in parent
    for (int i = 0; i < cmd_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all children
    int last_exit_code = 0;
    for (int i = 0; i < cmd_count; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == cmd_count - 1)  // Last command's exit code
            last_exit_code = WEXITSTATUS(status);
    }
    
    // Cleanup
    for (int i = 0; i < cmd_count - 1; i++)
        free(pipes[i]);
    free(pipes);
    free(pids);
    
    return last_exit_code;
}

// ─── Main Executor Function ────────────────────────────────────────────────

int execute_command_line(t_token *tokens, char **envp)
{
    if (!tokens)
        return 0;
    
    t_cmd *cmd_list = parse_tokens_to_commands(tokens);
    if (!cmd_list)
        return 0;
    
    int exit_code = execute_pipeline(cmd_list, envp);
    
    free_cmd_list(cmd_list);
    return exit_code;
}

// ─── Signal Handling ───────────────────────────────────────────────────────

static void signal_handler(int sig)
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

void setup_signals(void)
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, SIG_IGN);
}