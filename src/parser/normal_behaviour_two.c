/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal_behaviour_two.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:01:25 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/02 20:50:04 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	continue_word(t_seg_type *segs, t_w_tok *w_tok, t_token **head)
{
	while (w_tok->j < w_tok->seg_cnt)
	{
		w_tok->total_len += segs[w_tok->j].len;
		w_tok->j++;
	}
	w_tok->buf = malloc(w_tok->total_len + 1);
	if (!w_tok->buf)
	{
		free_tokens(head);
		return (free_seg_type(&segs, w_tok->seg_cnt));
	}
	w_tok->j = 0;
	while (w_tok->j < w_tok->seg_cnt)
	{
		ft_memcpy(w_tok->buf + w_tok->buf_len,
			segs[w_tok->j].txt, segs[w_tok->j].len);
		w_tok->buf_len += segs[w_tok->j].len;
		free(segs[w_tok->j].txt);
		w_tok->j++;
	}
	(w_tok->buf)[w_tok->buf_len] = '\0';
	append_buf_token(head, w_tok->buf, w_tok->buf_len, 0);
	free(w_tok->buf);
	free_seg_type(&segs, 0);
	return (1);
}

int	continue_while(char *line, int *i, t_temp *temp, t_w_tok *w_tok)
{
	if (w_tok->c == '=' && w_tok->in_quote == 0)
		w_tok->has_assign = 1;
	if (w_tok->c == '$' && w_tok->in_quote != '\'')
	{
		if (!variable_finder(temp->segs, w_tok, line, i))
			return (0);
		return (1);
	}
	if ((w_tok->c == '"' && w_tok->in_quote != '\'')
		|| (w_tok->c == '\'' && w_tok->in_quote != '"'))
	{
		assign_quote_mask(w_tok, i);
		return (1);
	}
	if (!segs_filler(temp->segs, w_tok, line, i))
	{
		free_tokens(temp->head);
		return (0);
	}
	return (2);
}

int	while_core_code(char *line, int *i, t_w_tok *w_tok, t_temp *temp)
{
	int	flag;

	while (line[*i] && (w_tok->in_quote
			|| (!ft_isspace(line[*i]) && !is_special_char(line[*i]))))
	{
		w_tok->c = line[*i];
		flag = continue_while(line, i, temp, w_tok);
		if (flag == 0)
			return (0);
	}
	return (1);
}

void	has_var_func(t_seg_type **segs, t_w_tok *w_tok)
{
	while (w_tok->j < w_tok->seg_cnt)
	{
		if ((*segs)[w_tok->j].len > 0 && (*segs)[w_tok->j].txt[0] == '$')
		{
			w_tok->has_var = 1;
			break ;
		}
		w_tok->j++;
	}
}

int	free_seg_type(t_seg_type **segs, int size)
{
	int	i;

	if (segs == NULL || *segs == NULL)
		return (0);
	if (size != 0)
	{
		i = 0;
		while (i < size)
		{
			if ((*segs)[i].txt)
				free((*segs)[i].txt);
			i++;
		}
	}
	free(*segs);
	*segs = NULL;
	return (0);
}
