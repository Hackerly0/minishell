
#include "../includes/minishell.h"

int g_signal = 0;

// int	main(int argc, char **argv, char **envp)
// {
// 	char	*line;
// 	t_token	*tokens;
// 	int		flag;

// 	argc = 0;
// 	argv = (char **)argv;
// 	line = readline("minishell$ ");
// 	while (line)
// 	{
// 		if (!quotes_validation(line))
// 		{
// 			free(line);
// 			return (1);
// 		}
// 		if (*line)
// 			add_history(line);
// 		tokens = tokenize(line);
// 		if (!tokens)
// 			fprintf(stderr, "tokenize error\n");
// 		else
// 		{
// 			//print_tokens(tokens);
// 			//printf("\n");
//             expand_variables(tokens);
// 			flag = parser(&tokens, envp);
            
// 			if (flag == 0)
// 				printf("syntax: wrong redirection\n");
// 			else if (flag == 1)
// 				printf("success!\n");
// 			else if (flag == 3)
// 				printf("No file\n");
// 			else if (flag == 4)
// 				printf("syntax: redirection without file\n");
// 			else if (flag == 5)
// 				printf("syntax: pipe in the start of the line\n");
// 			print_tokens(tokens);
// 			free_tokens(&tokens);
// 		}
// 		free(line);
// 		line = readline("minishell$ ");
// 	}
// 	free(line);
// 	return (0);
// }
//Initialize the global signal variable


static char *get_prompt(void)
{
    // return "minishell$ ";
    return "niggashell$ ";
}

static void handle_signal_in_main(void)
{
    if (g_signal == SIGINT)
    {
        g_signal = 0;  // Reset signal
        // Signal was handled in the signal handler
    }
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    
    char *line;
    int exit_code = 0;
    
    // Setup signal handlers
    setup_signals();
    
    // Set up environment for $? expansion
    char exit_status[16];
    snprintf(exit_status, sizeof(exit_status), "%d", exit_code);
    setenv("?", exit_status, 1);
    
    printf("Welcome to minishell!\n");
    printf("Type 'exit' to quit.\n");
    
    while (1)
    {
        // Handle any signals that occurred
        handle_signal_in_main();
        
        // Read input
        line = readline(get_prompt());
        
        // Handle EOF (Ctrl-D)
        if (!line)
        {
            printf("exit\n");
            break;
        }
        
        // Skip empty lines
        if (!*line)
        {
            free(line);
            continue;
        }
        
        // Add to history
        add_history(line);
        
        // Tokenize the input
        t_token *tokens = tokenize(line);
        if (!tokens)
        {
            free(line);
            continue;
        }
        
        // Expand variables
        expand_variables(&tokens);
        
        // Execute the command
        exit_code = execute_command_line(tokens, envp);
        
        // Update $? environment variable
        snprintf(exit_status, sizeof(exit_status), "%d", exit_code);
        setenv("?", exit_status, 1);
        
        // Clean up
        free_tokens(&tokens);
        free(line);
    }
    
    // Clean up readline
    rl_clear_history();
    
    return exit_code;
}