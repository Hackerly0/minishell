/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 00:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:43:02 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// ─── Echo Command ──────────────────────────────────────────────────────────

int builtin_echo(char **argv)
{
    int newline = 1;
    int i = 1;

    // ——— Consume all “-n”, “-nn”, “-nnnnn”… flags ———
    while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n')
    {
        // check that every character after the dash is 'n'
        int j = 1;
        while (argv[i][j] == 'n')
            j++;
        // if we hit something that isn’t 'n', stop flag parsing
        if (argv[i][j] != '\0')
            break;

        newline = 0;
        i++;
    }

    // ——— Print the rest of the arguments ———
    for (; argv[i]; i++)
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
    }

    // ——— Trailing newline if no -n flag seen ———
    if (newline)
        printf("\n");

    return 0;
}


// ─── Change Directory Command ───────────────────────────────────────────────

int builtin_cd(char **argv)
{
	char *path = argv[1];
	char *oldpwd;
	char *pwd;
	
	// Get current directory for OLDPWD
	oldpwd = getcwd(NULL, 0);
	
	// If no argument, go to HOME
	if (!path)
	{
		path = getenv("HOME");
		if (!path)
		{
			error_str("cd: HOME not set");
			free(oldpwd);
			return 1;
		}
	}
	
	// Change directory
	if (chdir(path) != 0)
	{
		error_file(path, NULL);
		free(oldpwd);
		return 1;
	}
	
	// Update PWD and OLDPWD
	if (oldpwd)
	{
		setenv("OLDPWD", oldpwd, 1);
		free(oldpwd);
	}
	
	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		setenv("PWD", pwd, 1);
		free(pwd);
	}
	
	return 0;
}

// ─── Print Working Directory Command ────────────────────────────────────────

int builtin_pwd(char **argv)
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

// ─── Environment Command ───────────────────────────────────────────────────

int builtin_env(char **argv, char **envp)
{
	(void)argv;
	
	if (!envp)
		return 1;
	
	for (int i = 0; envp[i]; i++)
		printf("%s\n", envp[i]);
	
	return 0;
}

// ─── Export Command ─────────────────────────────────────────────────────────

static int is_valid_identifier(const char *str)
{
	if (!str || !*str)
		return 0;
	
	// First character must be letter or underscore
	if (!isalpha((unsigned char)*str) && *str != '_')
		return 0;
	
	// Rest must be alphanumeric or underscore
	for (int i = 1; str[i]; i++)
	{
		if (!isalnum((unsigned char)str[i]) && str[i] != '_')
			return 0;
	}
	
	return 1;
}

static void print_export_vars(char **envp)
{
	if (!envp)
		return;
	
	// Simple implementation - just print all env vars with "declare -x"
	for (int i = 0; envp[i]; i++)
	{
		printf("declare -x %s\n", envp[i]);
	}
}

int builtin_export(char **argv, char ***envp)
{
	int ret = 0;
	
	// If no arguments, print all exported variables
	if (!argv[1])
	{
		print_export_vars(*envp);
		return 0;
	}
	
	// Process each argument
	for (int i = 1; argv[i]; i++)
	{
		char *eq = strchr(argv[i], '=');
		
		if (eq)
		{
			// Variable assignment: VAR=value
			*eq = '\0';
			char *name = argv[i];
			char *value = eq + 1;
			
			if (!is_valid_identifier(name))
			{
				error_cmd("export", "not a valid identifier");
				ret = 1;
				*eq = '=';  // Restore for next iteration
				continue;
			}
			
			setenv(name, value, 1);
			*eq = '=';  // Restore
		}
		else
		{
			// Just export existing variable
			if (!is_valid_identifier(argv[i]))
			{
				error_cmd("export", "not a valid identifier");
				ret = 1;
				continue;
			}
			
			char *value = getenv(argv[i]);
			if (value)
				setenv(argv[i], value, 1);
			else
				setenv(argv[i], "", 1);
		}
	}
	
	return ret;
}

// ─── Unset Command ──────────────────────────────────────────────────────────

int builtin_unset(char **argv)
{
	int ret = 0;
	
	if (!argv[1])
		return 0;
	
	// Process each argument
	for (int i = 1; argv[i]; i++)
	{
		if (!is_valid_identifier(argv[i]))
		{
			error_cmd("unset", "not a valid identifier");
			ret = 1;
			continue;
		}
		
		unsetenv(argv[i]);
	}
	
	return ret;
}

// ─── Exit Command ───────────────────────────────────────────────────────────

int builtin_exit(char **argv)
{
	int exit_code = 0;
	
	printf("exit\n");
	
	if (argv[1])
	{
		// Check if argument is numeric
		char *endptr;
		long val = strtol(argv[1], &endptr, 10);
		
		if (*endptr != '\0')
		{
			error_cmd("exit", "numeric argument required");
			exit_code = 255;
		}
		else
		{
			exit_code = (int)(val % 256);
		}
		
		// Check for too many arguments
		if (argv[2])
		{
			error_cmd("exit", "too many arguments");
			return 1;  // Don't exit, just return error
		}
	}
	
	exit(exit_code);
}

// ─── Built-in Command Checker ──────────────────────────────────────────────

int is_builtin(char *cmd)
{
	if (!cmd)
		return 0;
	
	return (strcmp(cmd, "echo") == 0 || 
			strcmp(cmd, "cd") == 0 ||
			strcmp(cmd, "pwd") == 0 || 
			strcmp(cmd, "env") == 0 ||
			strcmp(cmd, "exit") == 0 || 
			strcmp(cmd, "export") == 0 ||
			strcmp(cmd, "unset") == 0);
}

// ─── Built-in Command Executor ─────────────────────────────────────────────

int execute_builtin(char **argv, char **envp)
{
	if (!argv || !argv[0])
		return 1;
	
	if (strcmp(argv[0], "echo") == 0)
		return builtin_echo(argv);
	else if (strcmp(argv[0], "cd") == 0)
		return builtin_cd(argv);
	else if (strcmp(argv[0], "pwd") == 0)
		return builtin_pwd(argv);
	else if (strcmp(argv[0], "env") == 0)
		return builtin_env(argv, envp);
	else if (strcmp(argv[0], "export") == 0)
		return builtin_export(argv, &envp);
	else if (strcmp(argv[0], "unset") == 0)
		return builtin_unset(argv);
	else if (strcmp(argv[0], "exit") == 0)
		return builtin_exit(argv);
	
	return 1;
}
