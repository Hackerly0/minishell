/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 22:05:58 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:15:24 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

//two redirections without file
// <<< >>>
// <> ><
//
//No file
//end of redirections word
//end of redirections pipe

int	check_conditions(int count, t_token **cur, t_token_type prev)
{
	if (count != 0 && prev != T_WORD && prev != T_PIPE && prev != T_HEREDOC
		&& (*cur)->type != T_WORD && (*cur)->type != T_PIPE)
		return (4);
	if ((*cur)->type == T_HEREDOC || (*cur)->type == T_DOUT_REDIR)
		if (ft_strlen((*cur)->value) > 2)
			return (0);
	if ((*cur)->type == T_IN_REDIR || (*cur)->type == T_OUT_REDIR)
		if (ft_strlen((*cur)->value) > 1)
			return (0);
	if ((*cur)->type != T_WORD && (*cur)->type != T_PIPE && (*cur)->next != NULL
		&& (*cur)->next->type == T_WORD)
		(*cur) = (*cur)->next;
	else if ((*cur)->type != T_WORD && (*cur)->type != T_PIPE
		&& (*cur)->next == NULL)
		return (3);
	else if ((*cur)->type == T_WORD)
		return (1);
	else if ((*cur)->type == T_PIPE && count != 0)
		return (2);
	else if ((*cur)->type == T_PIPE && count == 0)
		return (5);
	return (6);
}

//return (6); end of command line

int	skip_red(t_token **cur)
{
	int				count;
	t_token_type	prev;
	int				flag;

	prev = (*cur)->type;
	count = 0;
	while (*cur)
	{
		flag = check_conditions(count, cur, prev);
		if (flag != 6)
			return (flag);
		prev = (*cur)->type;
		(*cur) = (*cur)->next;
		count++;
	}
	return (6);
}

void	op_arg(t_token **cur)
{
	if ((*cur)->value[0] == '-')
		(*cur)->type = T_OPTION;
	else
		(*cur)->type = T_ARGUMENT;
}

int	parse_word_cmd(t_token **cur)
{
	int				count;
	int				flag;

	flag = 6;
	count = 0;
	(*cur)->type = T_COMMAND;
	(*cur) = (*cur)->next;
	while (*cur)
	{
		if ((*cur)->type != T_WORD && (*cur)->type != T_PIPE)
		{
			flag = skip_red(cur);
			if (!flag || flag == 3 || flag == 4 || flag == 5)
				return (flag);
			if (flag == 6)
				return (1);
		}
		if ((*cur)->type == T_WORD)
			op_arg(cur);
		if ((*cur)->type == T_PIPE)
			return (2);
		(*cur) = (*cur)->next;
		count++;
	}
	return (flag);
}

int	is_here(char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == c)
			return (1);
		i++;
	}
	return (0);
}

void	free_split(char **arr)
{
	int	i;

	i = 0;
	while (arr[i] != NULL)
	{
		free(arr[i]);
		arr[i] = NULL;
		i++;
	}
	free(arr);
}

char	*ft_strjoin_three(char *s1, char *s2, char *s3)
{
	int		i;
	int		j;
	int		k;
	char	*s;

	i = -1;
	j = -1;
	k = -1;
	s = malloc(ft_strlen(s1) + ft_strlen(s2) + ft_strlen(s3) + 1);
	if (s == 0)
		return (0);
	while (s1[++i])
		s[i] = s1[i];
	while (s2[++j])
		s[i + j] = s2[j];
	while (s3[++k])
		s[i + j + k] = s3[k];
	s[i + j + k] = '\0';
	return (s);
}

int	with_path(char *value)
{
	if (access(value, X_OK) == 0)
		return (1);
	else
		return (0);
}

char	*return_all_paths(char **envp)
{
	int		i;
	char	*path_envp;

	i = 0;
	path_envp = NULL;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
		{
			path_envp = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!path_envp)
		return (NULL);
	return (path_envp);
}

int	is_cmd(char **temp_path, char ***split_paths, int *i, t_token **cur)
{
	char	*temp;

	temp = NULL;
	if (!(*temp_path))
	{
		free_split((*split_paths));
		return (2);
	}
	if (access((*temp_path), X_OK) == 0)
	{
		temp = (*cur)->value;
		(*cur)->value = *temp_path;
		free(temp);
		free_split((*split_paths));
		return (1);
	}
	free((*temp_path));
	(*i)++;
	return (0);
}

int	exists(t_token **cur, char **envp)
{
	char	*path_envp;
	int		i;
	char	**split_paths;
	char	*temp_path;
	int		flag;

	path_envp = NULL;
	if (is_here((*cur)->value, '/'))
		return (with_path((*cur)->value));
	path_envp = return_all_paths(envp);
	if (!path_envp)
		return (0);
	split_paths = ft_split(path_envp, ':');
	if (!split_paths)
		return (2);
	i = 0;
	while (split_paths[i] != NULL)
	{
		temp_path = ft_strjoin_three(split_paths[i], "/", (*cur)->value);
		flag = is_cmd(&temp_path, &split_paths, &i, cur);
		if (flag == 1 || flag == 2)
			return (flag);
	}
	free_split(split_paths);
	return (0);
}

int	check_cmd(t_token **tokens, char **envp)
{
	t_token	*cur;
	int		flag;

	cur = *tokens;
	flag = 0;
	while (cur)
	{
		if (cur->type == T_COMMAND)
		{
			if (!ft_strcmp(cur->value, "echo")
				|| !ft_strcmp(cur->value, "cd")
				|| !ft_strcmp(cur->value, "pwd")
				|| !ft_strcmp(cur->value, "export")
				|| !ft_strcmp(cur->value, "unset")
				|| !ft_strcmp(cur->value, "env")
				|| !ft_strcmp(cur->value, "exit"))
				return (1);
			flag = exists(&cur, envp);
			if (!flag || flag == 2)
				return (flag);
		}
		cur = cur->next;
	}
	return (1);
}

int	continue_parser(t_token **cur, t_token **tokens, char **envp)
{
	int	counter;
	int	flag;
	int	cmd_r;

	counter = 0;
	while ((*cur))
	{
		if (counter == 0 && (*cur)->type == T_PIPE)
			parser(cur, envp);
		if ((*cur)->type == T_WORD)
		{
			flag = parse_word_cmd(cur);
			if (!flag || flag == 3 || flag == 4 || flag == 5)
				return (flag);
		}
		if ((*cur))
			(*cur) = (*cur)->next;
		counter++;
	}
	cmd_r = check_cmd(tokens, envp);
	if (!cmd_r)
		printf("Command not found!\n");
	else if (cmd_r == 2)
		printf("malloc failure\n");
	return (1);
}

int	parser(t_token **tokens, char **envp)
{
	t_token	*cur;
	int		flag;

	cur = *tokens;
	flag = skip_red(&cur);
	if (flag == 1 || flag == 2)
		return (continue_parser(&cur, tokens, envp));
	else if (flag == 6)
		return (1);
	return (0);
}

/*while (cur)
		{
			if (counter == 0 && cur->type == T_PIPE)
				parser(&cur, envp);
			if (cur->type == T_WORD)
			{
				flag = parse_word_cmd(&cur);
				if (!flag || flag == 3 || flag == 4 || flag == 5)
					return (flag);
			}
			if (cur)
				cur = cur->next;
			counter++;
		}
		cmd_r = check_cmd(tokens, envp);
		if (!cmd_r)
			printf("Command not found!\n");
		else if (cmd_r == 2)
			printf("malloc failure\n");
		return (1);*/