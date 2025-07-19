/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:07:20 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:57:02 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char			*str;
	char			*temp;
	size_t			i;
	unsigned int	s_len;

	i = 0;
	s_len = 0;
	while (s[s_len] != '\0')
		s_len++;
	if (s_len <= start)
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	str = (char *)malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	temp = (char *)s;
	while (i < len && temp[i + start] != '\0')
	{
		str[i] = temp[i + start];
		i++;
	}
	str[i] = '\0';
	return (str);
}
