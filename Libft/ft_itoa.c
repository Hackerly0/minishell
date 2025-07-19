/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:38:53 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:54:10 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

static void	rev_str(char *str)
{
	int		i;
	int		j;
	int		len;
	char	temp;

	i = 0;
	len = 0;
	while (str[len] != '\0')
		len++;
	j = len - 1;
	while (i < len / 2)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

static void	after_checking(char *num, int n, int sign)
{
	int	i;

	i = 0;
	while (n != 0)
	{
		num[i] = (n % 10) + '0';
		i++;
		n = n / 10;
	}
	if (sign == -1)
	{
		num[i] = '-';
		i++;
	}
	num[i] = '\0';
	rev_str(num);
}

static int	num_size(int n)
{
	int	i;
	int	temp;

	temp = n;
	i = 1;
	if (n == -2147483648)
		return (12);
	else if (n == 0)
		return (2);
	else
	{
		if (temp < 0)
			temp = temp * -1;
		while (temp > 0)
		{
			i++;
			temp = temp / 10;
		}
		if (n < 0)
			return (i + 1);
		else
			return (i);
	}
	return (0);
}

char	*ft_itoa(int n)
{
	char	*num;

	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	else if (n == 0)
		return (ft_strdup("0"));
	num = (char *)malloc(num_size(n));
	if (num == NULL)
		return (NULL);
	if (n > 0)
		after_checking(num, n, 1);
	else
		after_checking(num, n * -1, -1);
	return (num);
}
/*
#include <stdio.h>

int	main()
{
	int	n;

	n = -62325237;
	printf("%s\n", ft_itoa(n));
}*/
