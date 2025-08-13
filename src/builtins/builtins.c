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

int	builtin_pwd(char **argv)
{
	char	*cwd;

	(void)argv;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}

int	builtin_env(char **argv, t_env **envp_list)
{
	int		i;
	char	**envp;

	(void)argv;
	envp = env_list_to_array(*envp_list);
	if (!envp)
		return (1);
	i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	free_envp(envp);
	return (0);
}

int	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!ft_isalpha((unsigned char)*str) && *str != '_')
		return (0);
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum((unsigned char)str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0);
}

int	execute_builtin(char **argv, t_env **env_list, t_cmd *cmd)
{
	int		ret;

	if (!argv || !argv[0])
		return (1);
	if (ft_strcmp(argv[0], "echo") == 0)
		ret = builtin_echo(argv);
	else if (ft_strcmp(argv[0], "cd") == 0)
		ret = builtin_cd(argv, env_list);
	else if (ft_strcmp(argv[0], "pwd") == 0)
		ret = builtin_pwd(argv);
	else if (ft_strcmp(argv[0], "env") == 0)
		ret = builtin_env(argv, env_list);
	else if (ft_strcmp(argv[0], "export") == 0)
		ret = builtin_export(argv, env_list);
	else if (ft_strcmp(argv[0], "unset") == 0)
		ret = builtin_unset(argv, env_list);
	else if (ft_strcmp(argv[0], "exit") == 0)
		ret = builtin_exit(argv, env_list, cmd);
	else
		return (1);
	return (ret);
}
