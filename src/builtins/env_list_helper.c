/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:57:20 by marvin            #+#    #+#             */
/*   Updated: 2025/08/12 18:45:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	list_count(t_env *e)
{
	int	n;

	n = 0;
	while (e)
	{
		n++;
		e = e->next;
	}
	return (n);
}

static char	*join_kv(const char *k, const char *v)
{
	size_t	lk;
	size_t	lv;
	char	*s;

	lk = ft_strlen(k);
	if (v)
		lv = ft_strlen(v);
	else
		lv = 0;
	s = (char *)malloc(lk + 1 + lv + 1);
	if (!s)
		return (NULL);
	ft_memcpy(s, k, lk);
	s[lk] = '=';
	if (lv > 0)
		ft_memcpy(s + lk + 1, v, lv);
	s[lk + 1 + lv] = '\0';
	return (s);
}

static void	*free_partial(char **a, int n)
{
	int	i;

	i = 0;
	while (i < n)
		free(a[i++]);
	free(a);
	return (NULL);
}

char	**env_list_to_array(t_env *head)
{
	int		n;
	int		i;
	char	**arr;

	n = list_count(head);
	arr = (char **)malloc(sizeof(char *) * (n + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (head)
	{
		arr[i] = join_kv(head->key, head->val);
		if (!arr[i])
			return (free_partial(arr, i));
		i++;
		head = head->next;
	}
	arr[i] = NULL;
	return (arr);
}

void	free_envp(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return ;
	while (envp[i])
		free(envp[i++]);
	free(envp);
}
