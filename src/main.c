/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 03:05:28 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/12 03:05:28 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile sig_atomic_t	g_signal;

static void	handle_signal_in_main(t_data *data)
{
	if (g_signal == SIGINT)
	{
		data->exit_code = 128 + SIGINT;
		g_signal = 0;
	}
}

void	main_while(t_data *data, t_token **tokens, char **line,
				t_env **env_list)
{
	while (1)
	{
		handle_signal_in_main(data);
		*line = readline("minishell$ ");
		if (!*line)
		{
			printf("exit\n");
			break ;
		}
		if (!sig_and_checks(line, data))
			continue ;
		add_history(*line);
		*tokens = tokenize(*line);
		if (!*tokens)
		{
			free(*line);
			continue ;
		}
		if (!expand_parse(data, tokens, line))
			continue ;
		execute_and_free(data, tokens, line, env_list);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_env	*env_list;
	t_data	data;
	t_token	*tokens;
	char	*line;

	(void)argc;
	(void)argv;
	env_list = env_init(envp);
	minishell_setup(&data, &env_list);
	main_while(&data, &tokens, &line, &env_list);
	rl_clear_history();
	rl_cleanup_after_signal();
	env_free_list(env_list);
	return (data.exit_code);
}
