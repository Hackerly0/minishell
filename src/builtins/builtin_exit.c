/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 12:00:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/12 16:56:07 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_number(const char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	ft_atoi_exit(const char *str)
{
	int		i;
	int		sign;
	long	result;

	i = 0;
	sign = 1;
	result = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] && ft_isdigit(str[i]))
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return ((int)((result * sign) % 256));
}

static void	close_all_fds(void)
{
	int	fd;

	fd = 3;
	while (fd < 1024)
	{
		close(fd);
		fd++;
	}
}

static void	cleanup_and_exit(int exit_code, t_env **env_list, t_cmd *cmd)
{
	close_all_fds();
	if (env_list && *env_list)
		env_free_list(*env_list);
	if (cmd)
	{
		if (cmd->token_list)
			free_tokens(&(cmd->token_list));
		free_cmd_list(cmd);
	}
	rl_clear_history();
	exit(exit_code);
}

int	builtin_exit(char **argv, t_env **env_list, t_cmd *cmd)
{
	int	exit_code;

	exit_code = 0;
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (argv[1])
	{
		if (!is_valid_number(argv[1]))
		{
			ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
			ft_putstr_fd(argv[1], STDERR_FILENO);
			ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
			cleanup_and_exit(255, env_list, cmd);
		}
		if (argv[2])
		{
			ft_putstr_fd("minishell: exit: too many", STDERR_FILENO);
			ft_putstr_fd(" arguments\n", STDERR_FILENO);
			return (1);
		}
		exit_code = ft_atoi_exit(argv[1]);
	}
	cleanup_and_exit(exit_code, env_list, cmd);
	return (0);
}
