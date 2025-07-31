#include "../../includes/minishell.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Helper function to get PATH environment variable
// static char *get_path_env(char **envp)
// {
// 	int i = 0;
	
// 	while (envp[i])
// 	{
// 		if (strncmp(envp[i], "PATH=", 5) == 0)
// 			return envp[i] + 5; // Skip "PATH="
// 		i++;
// 	}
// 	return NULL;
// }

// // Check if a file exists and is executable
// static int is_executable(const char *path)
// {
// 	struct stat st;
	
// 	if (stat(path, &st) == 0)
// 	{
// 		// Check if it's a regular file and executable
// 		if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))
// 			return 1;
// 	}
// 	return 0;
// }

// Find the full path of a command by searching through PATH directories
// char *find_command_path(char *cmd, char **envp)
// {
// 	char *path_env;
// 	char *path_copy;
// 	char *dir;
// 	char *full_path;
// 	int cmd_len;
// 	int dir_len;
	
// 	// If command already contains a path separator, don't search PATH
// 	if (strchr(cmd, '/'))
// 	{
// 		if (is_executable(cmd))
// 			return strdup(cmd);
// 		return NULL;
// 	}
	
// 	// Get PATH environment variable
// 	path_env = get_path_env(envp);
// 	if (!path_env)
// 		return NULL;
	
// 	// Make a copy of PATH since strtok modifies the string
// 	path_copy = strdup(path_env);
// 	if (!path_copy)
// 		return NULL;
	
// 	cmd_len = strlen(cmd);
	
// 	// Tokenize PATH and search each directory
// 	dir = strtok(path_copy, ":");
// 	while (dir)
// 	{
// 		dir_len = strlen(dir);
		
// 		// Allocate memory for dir + "/" + cmd + null terminator
// 		full_path = malloc(dir_len + 1 + cmd_len + 1);
// 		if (!full_path)
// 		{
// 			free(path_copy);
// 			return NULL;
// 		}
		
// 		// Construct full path: dir/cmd
// 		strcpy(full_path, dir);
// 		if (dir[dir_len - 1] != '/') // Add slash if not already there
// 			strcat(full_path, "/");
// 		strcat(full_path, cmd);
		
// 		// Check if this path is executable
// 		if (is_executable(full_path))
// 		{
// 			free(path_copy);
// 			return full_path; // Found it!
// 		}
		
// 		free(full_path);
// 		dir = strtok(NULL, ":");
// 	}
	
// 	free(path_copy);
// 	return NULL; // Command not found in PATH
// }