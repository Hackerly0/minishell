/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal_behaviour.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassende <hassende@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 19:58:54 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/07 11:54:41 by hassende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	default_behavior(t_seg_type **segs, t_w_tok *w_tok, t_token **head)
{
	w_tok->total_len = 0;
	w_tok->j = 0;
	while (w_tok->j < w_tok->seg_cnt)
		w_tok->total_len += (*segs)[w_tok->j++].len;
	w_tok->buf = malloc(w_tok->total_len + 1);
	if (!w_tok->buf)
	{
		free_tokens(head);
		return (free_seg_type(segs, w_tok->seg_cnt));
	}
	w_tok->j = 0;
	while (w_tok->j < w_tok->seg_cnt)
	{
		ft_memcpy(w_tok->buf + w_tok->buf_len,
			(*segs)[w_tok->j].txt, (*segs)[w_tok->j].len);
		w_tok->buf_len += (*segs)[w_tok->j].len;
		free((*segs)[w_tok->j].txt);
		w_tok->j++;
	}
	(w_tok->buf)[w_tok->buf_len] = '\0';
	append_buf_token(head, w_tok->buf, w_tok->buf_len, w_tok->mask);
	free(w_tok->buf);
	return (1);
}

int	tokens_gathering(t_seg_type **segs, t_w_tok *w_tok, t_token **head)
{
	w_tok->buf_len = 0;
	w_tok->j = w_tok->x;
	w_tok->cur_q = (*segs)[w_tok->x].q;
	while (w_tok->j < w_tok->seg_cnt && (*segs)[w_tok->j].q == w_tok->cur_q)
		w_tok->buf_len += (*segs)[w_tok->j++].len;
	w_tok->buf = (char *)malloc(w_tok->buf_len + 1);
	if (!w_tok->buf)
	{
		free_tokens(head);
		return (free_seg_type(segs, w_tok->seg_cnt));
	}
	w_tok->buf[0] = '\0';
	w_tok->j = 0;
	while (w_tok->x < w_tok->seg_cnt && (*segs)[w_tok->x].q == w_tok->cur_q)
	{
		ft_memcpy(w_tok->buf + w_tok->j, (*segs)[w_tok->x].txt,
			(*segs)[w_tok->x].len);
		w_tok->j += (*segs)[w_tok->x].len;
		free((*segs)[w_tok->x].txt);
		w_tok->x++;
	}
	w_tok->buf[w_tok->j] = '\0';
	append_buf_token(head, w_tok->buf, w_tok->buf_len, w_tok->cur_q);
	free(w_tok->buf);
	return (1);
}

void	temp_init(t_temp *temp, t_seg_type **segs, t_token **head)
{
	temp->head = head;
	temp->segs = segs;
}

int	extract_word_tokens(char *line, int *i, t_token **head)
{
	t_seg_type	*segs;
	t_w_tok		w_tok;
	t_temp		temp;

	segs = (t_seg_type *)malloc(ft_strlen(line) * sizeof(t_seg_type));
	if (!segs)
		return (0);
	w_tok_initializer(&w_tok);
	temp_init(&temp, &segs, head);
	if (!while_core_code(line, i, &w_tok, &temp))
		return (free_seg_type(&segs, w_tok.seg_cnt));
	if (w_tok.has_assign)
		return (continue_word(segs, &w_tok, head));
	has_var_func(&segs, &w_tok);
	if (!w_tok.has_assign && !w_tok.has_var)
	{
		w_tok.flag = default_behavior(&segs, &w_tok, head);
		free(segs);
		return (w_tok.flag);
	}
	while (w_tok.x < w_tok.seg_cnt)
		if (!tokens_gathering(&segs, &w_tok, head))
			return (0);
	free(segs);
	return (1);
}
