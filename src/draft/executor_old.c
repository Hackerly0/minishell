// executor.c
#include "executor.h"
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>   // for getenv()

int g_last_exit_code = 0;

// Print error via perror and exit
void error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

// Count how many commands
static int count_cmds(char **cmds)
{
	int i = 0;
	while (cmds[i])
		i++;
	return (i);
}

// Close both ends of the two pipe slots
static void close_pipes(int pipes[2][2])
{
	for (int j = 0; j < 2; j++) {
		if (pipes[j][0] >= 0) { close(pipes[j][0]); pipes[j][0] = -1; }
		if (pipes[j][1] >= 0) { close(pipes[j][1]); pipes[j][1] = -1; }
	}
}

// In the child: wire stdin/stdout, close fds, exec
static void spawn_child(t_vars *vars, int idx)
{
	// Restore default signal handling
	signal(SIGINT,  SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	// stdin ← previous pipe read end
	if (idx > 0) {
		if (dup2(vars->pipes[(idx - 1) % 2][0], STDIN_FILENO) < 0)
			error_exit("dup2 stdin");
	}
	// stdout → current pipe write end
	if (idx < vars->num - 1) {
		if (dup2(vars->pipes[idx % 2][1], STDOUT_FILENO) < 0)
			error_exit("dup2 stdout");
	}

	// Close all pipes
	close_pipes(vars->pipes);

	// Determine shell path
	char *shell = getenv("SHELL");
	if (!shell)
		shell = "/bin/sh";

	// Build argv for sh -c "cmd"
	char *base = strrchr(shell, '/');
	char *shname = base ? base + 1 : shell;
	char *args[] = { shname, "-c", vars->cmds[idx], NULL };

	execve(shell, args, vars->envp);
	// if execve returns, it failed
	perror("execve");
	exit(errno);
}

// Parent: create pipes, fork children, close unused fds, then wait
void executor(t_vars *vars)
{
	vars->num = count_cmds(vars->cmds);

	// initialize pipe slots to -1
	for (int j = 0; j < 2; j++)
		vars->pipes[j][0] = vars->pipes[j][1] = -1;

	for (int i = 0; i < vars->num; i++) {
		// create a new pipe if not last command
		if (i < vars->num - 1) {
			if (pipe(vars->pipes[i % 2]) < 0)
				error_exit("pipe");
		}

		pid_t pid = fork();
		if (pid < 0)
			error_exit("fork");

		if (pid == 0) {
			// child
			spawn_child(vars, i);
		} else {
			// parent closes the ends of the *previous* pipe slot
			if (i > 0) {
				int prev = (i - 1) % 2;
				if (vars->pipes[prev][0] >= 0) close(vars->pipes[prev][0]);
				if (vars->pipes[prev][1] >= 0) close(vars->pipes[prev][1]);
			}
		}
	}

	// wait for all children and capture exit code
	for (int i = 0; i < vars->num; i++) {
		int status;
		if (wait(&status) < 0)
			error_exit("wait");

		if (WIFEXITED(status))
			g_last_exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			g_last_exit_code = 128 + WTERMSIG(status);
	}
}

// Sample main to test executor
int main(int argc, char **argv, char **envp)
{
	if (argc < 4) {
		fprintf(stderr, "Usage: %s infile cmd1 [cmd2 ...] outfile\n", argv[0]);
		return (1);
	}

	t_vars vars;
	vars.envp = envp;

	// redirect stdin from infile
	vars.infile = open(argv[1], O_RDONLY);
	if (vars.infile < 0) {
		perror("open infile");
		return (1);
	}
	if (dup2(vars.infile, STDIN_FILENO) < 0) {
		perror("dup2 infile");
		return (1);
	}
	close(vars.infile);

	// redirect stdout to outfile
	vars.outfile = open(argv[argc - 1],
						O_CREAT | O_WRONLY | O_TRUNC,
						0644);
	if (vars.outfile < 0) {
		perror("open outfile");
		return (1);
	}
	if (dup2(vars.outfile, STDOUT_FILENO) < 0) {
		perror("dup2 outfile");
		return (1);
	}
	close(vars.outfile);

	// ignore signals in the parent shell
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	// build the cmds array: argv[2] .. argv[argc-2]
	int cmd_count = argc - 3;
	vars.cmds = malloc((cmd_count + 1) * sizeof(char *));
	if (!vars.cmds)
		error_exit("malloc");

	for (int i = 0; i < cmd_count; i++)
		vars.cmds[i] = argv[i + 2];
	vars.cmds[cmd_count] = NULL;

	executor(&vars);

	free(vars.cmds);
	return (g_last_exit_code);
}
