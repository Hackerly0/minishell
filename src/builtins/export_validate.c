/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_validate.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:20:42 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:47:39 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_id_char(char c, int first)
{
	if (first)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}

int	is_valid_export_id(const char *s)
{
	int	i;

	if (!s || !*s || !is_valid_id_char(s[0], 1))
		return (0);
	i = 1;
	while (s[i] && s[i] != '=' && !(s[i] == '+' && s[i + 1] == '='))
	{
		if (!is_valid_id_char(s[i], 0))
			return (0);
		i++;
	}
	return (1);
}

int	ret_free(char **key)
{
	free(*key);
	*key = NULL;
	return (-1);
}

int	parse_export_arg(const char *arg, char **key, char **val)
{
	int	i;

	*key = NULL;
	*val = NULL;
	if (!arg || !*arg)
		return (-1);
	if (!is_valid_export_id(arg))
		return (-1);
	i = 0;
	while (arg[i] && arg[i] != '=')
		i++;
	if (arg[i] == '\0')
		return (-2);
	*key = ft_substr(arg, 0, i);
	if (!*key)
		return (-1);
	if (arg[i] == '=')
	{
		*val = ft_strdup(arg + i + 1);
		if (!*val)
			return (ret_free(key));
	}
	return (0);
}

void	print_export_id_error(const char *arg)
{
	write(2, "minishell: export: `", 20);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 26);
}
