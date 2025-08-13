/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 19:50:47 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/13 01:54:11 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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

int	pipe_error(t_token *cur, t_data *data)
{
	char	*s;

	if (ft_strncmp(cur->value, "|", 1) == 0 && cur->next != NULL
		&&ft_strncmp(cur->next->value, "|", 1) == 0)
		s = ft_strdup("minishell: syntax error near unexpected token '||'\n");
	else
		s = ft_strdup("minishell: syntax error near unexpected token '|'\n");
	write(2, s, ft_strlen(s));
	free(s);
	data->exit_code = 2;
	return (0);
}

int	print_syntax_error(char *value, t_data *data)
{
	char	*msg;

	if (!ft_strncmp(value, "><", 2))
		msg = ft_strjoin_three(
				"minishell: syntax error near unexpected token '",
				value + 1, "'\n");
	else
		msg = ft_strjoin_three(
				"minishell: syntax error near unexpected token '",
				value + 2, "'\n");
	if (msg)
	{
		write(2, msg, ft_strlen(msg));
		free(msg);
	}
	data->exit_code = 2;
	return (0);
}

int	missing_file(t_data *data)
{
	char	*msg;

	msg = ft_strdup(
			"minishell: syntax error near unexpected token 'newline'\n");
	write(2, msg, ft_strlen(msg));
	free(msg);
	data->exit_code = 2;
	return (0);
}

int	is_dir(const char *path)
{
	struct stat	st;

	if (!path || stat(path, &st) == -1)
		return (0);
	return (S_ISDIR(st.st_mode));
}
