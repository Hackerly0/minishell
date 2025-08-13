/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:23:52 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:23:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	minishell_setup(t_data *data, t_env **env_list)
{
	setup_signals();
	data->exit_code = 0;
	data->cmds = NULL;
	data->envp = env_list;
	data->tokens = NULL;
	printf("Welcome to minishell!\n");
	printf("Type 'exit' to quit.\n");
}

int	normal_checks(char **line, t_data *data)
{
	if (!*line)
	{
		free(*line);
		return (0);
	}
	if (!quotes_validation(*line))
	{
		write(2, "Syntax error: unmatched quotes\n", 32);
		data->exit_code = 2;
		free(*line);
		return (0);
	}
	return (1);
}

int	expand_parse(t_data *data, t_token **tokens, char **line)
{
	data->tokens = tokens;
	expand_variables(data);
	if (!parser(data))
	{
		free(*line);
		free_tokens(tokens);
		return (0);
	}
	return (1);
}

void	execute_and_free(t_data *data, t_token **tokens, char **line,
					t_env **env_list)
{
	data->exit_code = execute_command_line(*tokens, env_list, data);
	free_tokens(tokens);
	free(*line);
}

int	sig_and_checks(char **line, t_data *data)
{
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		data->exit_code = 128 + SIGINT;
		free(*line);
		return (0);
	}
	if (!normal_checks(line, data))
		return (0);
	return (1);
}
