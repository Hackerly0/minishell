/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:56:07 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 15:56:07 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	printing_loop(char **argv, int *i)
{
	printf("%s", argv[*i]);
	if (argv[*i + 1])
		printf(" ");
	(*i)++;
}

int	builtin_echo(char **argv)
{
	int	newline;
	int	i;
	int	j;

	newline = 1;
	i = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n')
	{
		j = 1;
		while (argv[i][j] == 'n')
			j++;
		if (argv[i][j] != '\0')
			break ;
		newline = 0;
		i++;
	}
	while (argv[i])
		printing_loop(argv, &i);
	if (newline)
		printf("\n");
	return (0);
}
