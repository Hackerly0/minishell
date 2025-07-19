/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 21:50:01 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:26:52 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isspace(int c)
{
	if (c == 32)
		return (1);
	return (0);
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			break ;
		i++;
	}
	return (s1[i] - s2[i]);
}

char	*ft_strndup(const char *src, int size)
{
	char	*duplicated;
	int		i;

	i = 0;
	while (i < size && src[i])
		i++;
	duplicated = malloc(i + 1);
	if (duplicated == NULL)
		return (NULL);
	ft_memcpy(duplicated, src, i);
	duplicated[i] = '\0';
	return (duplicated);
}

static t_token_type	classify_token(char *s)
{
	if (ft_strcmp(s, "|") == 0)
		return (T_PIPE);
	if (ft_strcmp(s, "<<") == 0 || ft_strcmp(s, "<<") == '<'
		|| ft_strcmp(s, "<<") == '>')
		return (T_HEREDOC);
	if (ft_strcmp(s, "<") == 0 || ft_strcmp(s, "<") == '>')
		return (T_IN_REDIR);
	if (ft_strcmp(s, ">>") == 0 || ft_strcmp(s, ">>") == '>'
		|| ft_strcmp(s, ">>") == '<')
		return (T_DOUT_REDIR);
	if (ft_strcmp(s, ">") == 0 || ft_strcmp(s, ">") == '<')
		return (T_OUT_REDIR);
	return (T_WORD);
}

void	free_tokens(t_token **tokens)
{
	t_token	*next;

	while (*tokens)
	{
		next = (*tokens)->next;
		free((*tokens)->value);
		free(*tokens);
		*tokens = next;
	}
}

t_token	*create_token(const char *val, t_token_type type, int in_quotes)
{
	t_token	*tok;

	tok = malloc(sizeof(*tok));
	if (!tok)
		return (NULL);
	tok->value = ft_strdup(val);
	if (!tok->value)
	{
		free(tok);
		return (NULL);
	}
	tok->type = type;
	tok->in_quotes = in_quotes;
	tok->next = NULL;
	return (tok);
}

void	append_token(t_token **head, t_token *new)
{
	t_token	*cur;

	if (!new)
		return ;
	if (!*head)
		*head = new;
	else
	{
		cur = *head;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
}

static t_token	*extract_operator_token(char *line, int *i)
{
	int		len;
	char	*val;
	t_token	*tok;
	int		j;

	j = *i + 1;
	len = 1;
	while (line[j] && (line[j] == '>' || line[j] == '<'))
	{
		j++;
		len++;
	}
	val = ft_strndup(line + *i, len);
	if (!val)
		return (NULL);
	tok = create_token(val, classify_token(val), 0);
	free(val);
	*i += len;
	return (tok);
}

static int	append_buf_token(t_token **head, char *buf, int len, int quoted)
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
	return (0);
}

static int	free_seg_type(t_seg_type **segs, int size)
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

static int	variable_finder(t_seg_type **segs, t_w_tok *w_tok,
	char *line, int *i)
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

static void	assign_quote_mask(t_w_tok *w_tok, int *i)
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

static int	segs_filler(t_seg_type **segs, t_w_tok *w_tok, char *line, int *i)
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

static void	w_tok_initializer(t_w_tok *w_tok)
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

static int	continue_word(t_seg_type *segs, t_w_tok *w_tok, t_token **head)
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

static int	continue_while(char *line, int *i, t_temp *temp,
	t_w_tok *w_tok)
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

static int	while_core_code(char *line, int *i, t_w_tok *w_tok, t_temp *temp)
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

static void	has_var_func(t_seg_type **segs, t_w_tok *w_tok)
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

static int	default_behavior(t_seg_type **segs, t_w_tok *w_tok, t_token **head)
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

static int	tokens_gathering(t_seg_type **segs, t_w_tok *w_tok, t_token **head)
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

static void	temp_init(t_temp *temp, t_seg_type **segs, t_token **head)
{
	temp->head = head;
	temp->segs = segs;
}

static int	extract_word_tokens(char *line, int *i, t_token **head)
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

static int	token_cases(char *line, t_token **head, int *i)
{
	t_token	*op;

	if (ft_isspace(line[*i]))
	{
		(*i)++;
		return (1);
	}
	if (is_special_char(line[*i]))
	{
		op = extract_operator_token(line, i);
		if (!op)
		{
			free_tokens(head);
			return (0);
		}
		append_token(head, op);
		return (1);
	}
	return (extract_word_tokens(line, i, head));
}

t_token	*tokenize(char *line)
{
	t_token	*head;
	int		i;

	head = NULL;
	i = 0;
	while (line[i])
	{
		if (!token_cases(line, &head, &i))
		{
			if (head)
				free_tokens(&head);
			return (NULL);
		}
	}
	return (head);
}

void	print_tokens(t_token *tokens)
{
	while (tokens)
	{
		printf("[%s] type=%d quoted=%d\n",
			tokens->value, tokens->type, tokens->in_quotes);
		tokens = tokens->next;
	}
}

int	quotes_validation(char *line)
{
	int		i;
	char	c;

	i = 0;
	while (line[i])
	{
		if (line[i] == '\'' || line[i] == '"')
		{
			c = line[i++];
			while (line[i] != c && line[i])
				i++;
			if (!line[i])
				return (0);
		}
		i++;
	}
	return (1);
}
