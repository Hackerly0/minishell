/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal_behaviour_three.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnisirat <hnisirat@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:01:28 by oalhoora          #+#    #+#             */
/*   Updated: 2025/08/06 20:48:47 by hnisirat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	append_buf_token(t_token **head, char *buf, int len, int quoted)
{
	t_token	*tok;

	if (len == 0 && quoted == 0)
		return (1);
	else if (len != 0)
		buf[len] = '\0';
	tok = create_token(buf, T_WORD, quoted);
	if (tok)
	{
		append_token(head, tok);
		return (1);
	}
	free_tokens(&tok);
	return (0);
}

int	variable_finder(t_seg_type **segs, t_w_tok *w_tok, char *line, int *i)
{
	w_tok->start = (*i)++;
	if (line[*i] == '?')
		(*i)++;
	else
		while (ft_isalnum(line[*i]) || line[*i] == '_')
			(*i)++;
	if (line[*i] && !ft_isalnum(line[*i]) && !ft_isspace(line[*i])
		&& !is_special_char(line[*i]) && line[*i] != '"' && line[*i] != '\'')
		(*i)++;
	w_tok->len = *i - w_tok->start;
	if (w_tok->in_quote == '\"')
		w_tok->q = Q_DQUOT;
	else if (w_tok->in_quote == '\'')
		w_tok->q = Q_SQUOT;
	else
		w_tok->q = 0;
	(*segs)[w_tok->seg_cnt].txt = ft_strndup(line + w_tok->start, w_tok->len);
	if (!(*segs)[w_tok->seg_cnt].txt)
		return (free_seg_type(segs, w_tok->seg_cnt));
	(*segs)[w_tok->seg_cnt].len = w_tok->len;
	(*segs)[w_tok->seg_cnt].q = w_tok->q;
	w_tok->seg_cnt++;
	return (1);
}

void	assign_quote_mask(t_w_tok *w_tok, int *i)
{
	if (w_tok->c == '"' && w_tok->in_quote != '\'')
	{
		if (w_tok->in_quote == '"')
			w_tok->in_quote = 0;
		else
			w_tok->in_quote = '"';
		if (w_tok->in_quote)
			w_tok->mask |= Q_DQUOT;
		(*i)++;
	}
	if (w_tok->c == '\'' && w_tok->in_quote != '"')
	{
		if (w_tok->in_quote == '\'')
			w_tok->in_quote = 0;
		else
			w_tok->in_quote = '\'';
		if (w_tok->in_quote)
			w_tok->mask |= Q_SQUOT;
		(*i)++;
	}
}

int	segs_filler(t_seg_type **segs, t_w_tok *w_tok, char *line, int *i)
{
	(*segs)[w_tok->seg_cnt].txt = ft_strndup(line + *i, 1);
	if (!(*segs)[w_tok->seg_cnt].txt)
		return (free_seg_type(segs, w_tok->seg_cnt));
	(*segs)[w_tok->seg_cnt].len = 1;
	if (w_tok->in_quote == '\"')
		(*segs)[w_tok->seg_cnt].q = Q_DQUOT;
	else if (w_tok->in_quote == '\'')
		(*segs)[w_tok->seg_cnt].q = Q_SQUOT;
	else
		(*segs)[w_tok->seg_cnt].q = 0;
	w_tok->seg_cnt++;
	(*i)++;
	return (1);
}

void	w_tok_initializer(t_w_tok *w_tok)
{
	w_tok->buf_len = 0;
	w_tok->c = 0;
	w_tok->seg_cnt = 0;
	w_tok->in_quote = 0;
	w_tok->mask = 0;
	w_tok->has_assign = 0;
	w_tok->cur_q = 0;
	w_tok->has_var = 0;
	w_tok->x = 0;
	w_tok->j = 0;
	w_tok->len = 0;
	w_tok->q = 0;
	w_tok->start = 0;
	w_tok->total_len = 0;
	w_tok->flag = 0;
}
