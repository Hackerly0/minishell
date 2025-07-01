/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 14:40:59 by oalhoora          #+#    #+#             */
/*   Updated: 2025/06/29 15:27:52 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

static int  is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

static t_token_type classify_token(const char *s)
{
    if (strcmp(s, "|") == 0)
        return (T_PIPE);
    if (strcmp(s, "<<") == 0)
        return (T_HEREDOC);
    if (strcmp(s, "<") == 0)
        return (T_IN_REDIR);
    if (strcmp(s, ">>") == 0)
        return (T_DOUT_REDIR);
    if (strcmp(s, ">") == 0)
        return (T_OUT_REDIR);
    return (T_WORD);
}

static t_token  *create_token(char *val, t_token_type type, int in_quotes)
{
    t_token *tok;

    tok = (t_token *)malloc(sizeof(*tok));
    if (!tok)
        return (NULL);
    tok->value = strdup(val);
    if (!(tok->value))
        return (NULL);
    tok->type = type;
    tok->in_quotes = in_quotes;
    tok->next = NULL;
    return (tok);
}

static void append_token(t_token **head, t_token *new)
{
    t_token *cur;

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

static t_token  *extract_operator_token(char *line, int *i)
{
    int     len;
    char    *val;
    t_token *tok;

    len = 1;
    if ((line[*i] == '<' && line[*i + 1] == '<')
        || (line[*i] == '>' && line[*i + 1] == '>'))
        len = 2;
    val = strndup(line + *i, len);
    if (!val)
        return (NULL);
    tok = create_token(val, classify_token(val), 0);
    if (!tok)
        return (NULL);
    free(val);
    *i += len;
    return (tok);
}

static void buffer_maker_else(char *buf, int *i, int *buf_len, char *line)
{
    buf = realloc(buf, (*buf_len) + 2);
    if (!buf)
        return ;
    buf[(*buf_len)++] = line[*i];
    buf[*buf_len] = '\0';
    (*i)++;
}

static void buffer_maker(char *buf, int *i, int *buf_len, char *line)
{
    (*i)++;
    while (line[*i] && line[*i] != '"' && line[*i] != '\'')
    {
        buf = realloc(buf, (*buf_len) + 2);
        if (!buf)
            return ;
        buf[(*buf_len)++] = line[*i];
        buf[*buf_len] = '\0';
        (*i)++;
    }
    if (line[*i] == '"' || line[*i] == '\'')
        (*i)++;
}

static char *word_loop(char *line, int *i, int *quote, int *buf_len)
{
    char    *buf;

    buf = (char *)malloc(1);
    if (!buf)
        return (NULL);
    while (line[*i] && !isspace(line[*i]) && !is_special_char(line[*i]))
    {
        if (line[*i] == '"' || line[*i] == '\'')
        {
            if (line[*i] == '"')
                (*quote) |= 1;
            else
                (*quote) |= 2;
            buffer_maker(buf, i, buf_len, line);
            if (!buf)
                return (NULL);
        }
        else
        {
            buffer_maker_else(buf, i, buf_len, line);
            if (!buf)
                return (NULL);
        }
    }
    return (buf);
}

static t_token  *extract_word_token(char *line, int *i)
{
    int     buf_len;
    int     quote;
    char    *buf;
    char    c;
    t_token *tok;

    buf_len = 0;
    quote = 0;
    buf = word_loop(line, i, &quote, &buf_len);
    if (!buf)
        return (NULL);
    if (buf_len == 0)
    {
        free(buf);
        return (NULL);
    }
    tok = create_token(buf, T_WORD, quote);
    free(buf);
    return (tok);
}

static int  token_cases(char *line, t_token **head, int *i, t_token *tok)
{
    if (isspace((unsigned char)line[*i]))
        (*i)++;
    else if (is_special_char(line[*i]))
    {
        tok = extract_operator_token(line, i);
        if (!tok)
        {
            free_tokens(*head);
            return (0);
        }
        append_token(head, tok);
    }
    else
    {
        tok = extract_word_token(line, i);
        if (!tok)
        {
            free_tokens(*head);
            return (0);
        }
        append_token(head, tok);
    }
    return (1);
}

t_token *tokenize(char *line)
{
    t_token *head;
    int     i;
    t_token *tok;

    head = NULL;
    i = 0;
    while (line[i])
    {
        if (token_cases(line, &head, &i, tok) == 0)
            return (NULL);
    }
    return (head);
}

void    print_tokens(t_token *t)
{
    t_token *temp;

    temp = t;
    while (temp)
    {
        printf("[%s] type=%d quoted=%d\n",
            temp->value, temp->type, temp->in_quotes);
        temp = temp->next;
    }
}

void    free_tokens(t_token *t)
{
    t_token *next;

    while (t)
    {
        next = t->next;
        free(t->value);
        free(t);
        t = next;
    }
}

int main(void)
{
    char    *line;
    t_token *tokens;

    line = readline("minishell$ ");
    if (!line)
        return (0);
    add_history(line);
    tokens = tokenize(line);
    if (!tokens)
        printf("here\n");
    //expand_variables(tokens);
    print_tokens(tokens);
    free_tokens(tokens);
    free(line);
    return (0);
}