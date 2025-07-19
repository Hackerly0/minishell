/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:00:39 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:46 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

static void	after_checking(int n, int fd)
{
	char	c;

	if (n != 0)
	{
		c = (n % 10) + '0';
		after_checking(n / 10, fd);
		write(fd, &c, 1);
	}
}

void	ft_putnbr_fd(int n, int fd)
{
	if (fd >= 0 && fd <= 1023)
	{
		if (n == -2147483648)
			write(fd, "-2147483648", 11);
		else if (n == 0)
			write(fd, "0", 1);
		else if (n < 0)
		{
			write(fd, "-", 1);
			after_checking(n * -1, fd);
		}
		else
			after_checking(n, fd);
	}
}
/*
int	main()
{
	int	n;

	n = -214748364;
	ft_putnbr_fd(n, 1);
	write(1, "\n", 1);
}*/
