/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:51:54 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:44:26 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_atoi(const char *str)
{
	int	j;
	int	val;
	int	i;

	i = 0;
	j = 1;
	val = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			j = -1;
		i++;
	}
	while (str[i] <= '9' && str[i] >= '0')
	{
		val = val * 10 + str[i] - '0';
		i++;
	}
	return (val * j);
}
/*
#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%i\n", ft_atoi(argv[1]));
	return (0);
}*/
