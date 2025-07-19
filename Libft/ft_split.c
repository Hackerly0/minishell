/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 15:00:39 by oalhoora          #+#    #+#             */
/*   Updated: 2024/09/12 14:55:56 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	freeing(char **words, int count)
{
	int		i;

	i = 0;
	while (i < count)
	{
		free(words[i]);
		i++;
	}
	free(words);
	return (0);
}

static int	count(const char *s, char c)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (s[i] == c)
		i++;
	while (s[i] != '\0')
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			j++;
		i++;
	}
	return (j);
}

static int	memory(const char *s, char ***words, char c)
{
	int	i;
	int	j;
	int	k;

	i = 0;
	j = 0;
	k = 0;
	*words = (char **)malloc((count(s, c) + 1) * sizeof(char *));
	if (*words == NULL)
		return (0);
	while (s[i] != '\0')
	{
		if (s[i] == c)
			i++;
		else
		{
			j = i;
			while (s[i] != c && s[i] != '\0')
				i++;
			(*words)[k] = (char *)malloc(i - j + 1);
			if ((*words)[k++] == NULL)
				return (freeing(*words, k));
		}
	}
	return (1);
}

static void	real_splitter(char const *s, char **words, char c)
{
	int	i;
	int	j;
	int	k;

	i = 0;
	j = 0;
	k = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			i++;
		else
		{
			while (s[i] != c && s[i] != '\0')
			{
				words[j][k] = s[i];
				i++;
				k++;
			}
			words[j][k] = '\0';
			j++;
			k = 0;
		}
	}
	words[j] = NULL;
}

char	**ft_split(char const *s, char c)
{
	char	**words;

	if (memory(s, &words, c) == 0)
		return (NULL);
	real_splitter(s, words, c);
	return (words);
}
/*
#include <stdio.h>
#include <stdlib.h>

void print_split_result(char **result) {
    if (!result) {
        printf("Result is NULL\n");
        return;
    }

    int i = 0;
    while (result[i]) {
        printf("result[%d]: \"%s\"\n", i, result[i]);
        i++;
    }
}

int main() {
    // Test cases with different scenarios
    char **result;

    printf("Test 1: Normal case with no consecutive delimiters\n");
    result = ft_split("hello,world,test", ',');
    print_split_result(result);

    printf("\nTest 2: String with consecutive delimiters\n");
    result = ft_split("hello,,world", ',');
    print_split_result(result);

    printf("\nTest 3: String starts and ends with delimiter\n");
    result = ft_split(",hello,", ',');
    print_split_result(result);

    printf("\nTest 4: String with only delimiters\n");
    result = ft_split(",,,", ',');
    printf("%s", result[5]);

    printf("\nTest 5: Empty string input\n");
    result = ft_split("", ',');
    print_split_result(result);

    return 0;
}*/
