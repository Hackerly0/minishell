/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 14:41:23 by oalhoora          #+#    #+#             */
/*   Updated: 2025/06/29 14:52:34 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
// minishell.h
#ifndef MINISHELL_H
# define MINISHELL_H
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
typedef enum e_token_type
{
    T_WORD,
    T_PIPE,
    T_IN_REDIR,
    T_OUT_REDIR,
    T_DOUT_REDIR,
    T_HEREDOC
}   t_token_type;
typedef struct s_token
{
    char            *value;
    t_token_type    type;
    int             in_quotes;
    struct s_token  *next;
}                   t_token;
t_token *tokenize(char *line);
void    expand_variables(t_token *tokens);
void    print_tokens(t_token *tokens);
void    free_tokens(t_token *tokens);
#endif