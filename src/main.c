
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

int	main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;

    char    *line;
    int     exit_code = 0;

    /* Setup signal handlers */
    setup_signals();

    /* Prepare `$?` for expansions */
    char    exit_status[16];
    snprintf(exit_status, sizeof(exit_status), "%d", exit_code);
    setenv("?", exit_status, 1);

    printf("Welcome to minishell!\n");
    printf("Type 'exit' to quit.\n");

    while (1)
    {
        /* Handle any pending signals */
        handle_signal_in_main();

        /* Read a line */
        line = readline(get_prompt());
        if (!line)
        {
            printf("exit\n");
            break;
        }

        /* Skip blank lines */
        if (!*line)
        {
            free(line);
            continue ;
        }

        /* Add to history now that we know it’s syntactically valid */
        add_history(line);

        /* Tokenize */
        t_token *tokens = tokenize(line);
        if (!tokens)
        {
            free(line);
            continue ;
        }

        /* Expand variables like `$?` */
        expand_variables(&tokens);
        if(!parser(&tokens))
        {
            free(line);
            free_tokens(&tokens);
            continue;
        }
        /* Dispatch into your existing executor */
        exit_code = execute_command_line(tokens, envp);
        free_tokens(&tokens);
        printf("hello\n");

        /* Update `$?` */
        snprintf(exit_status, sizeof(exit_status), "%d", exit_code);
        setenv("?", exit_status, 1);

        /* Clean up this iteration */
        free(line);
    }

    /* Teardown readline & clean up `$?` */
    rl_clear_history();
    rl_cleanup_after_signal();
    unsetenv("?");

    return (exit_code);
}
