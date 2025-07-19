/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 19:10:00 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:15:23 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*expand_string(const char *s)
{
	char	*out;
	int		o_len;

	out = malloc(1);
	out[0] = '\0';

	for (int i = 0; s[i]; )
	{
		if (s[i] == '$')
		{
			i++;
			char name[128];
			int  n = 0;

			if (s[i] == '{')
			{
				i++;
				while (s[i] && s[i] != '}' && n < 126)
					name[n++] = s[i++];
				if (s[i] == '}') i++;
			}
			else
			{
				while (isalnum((unsigned char)s[i]) || s[i] == '_')
					name[n++] = s[i++];
			}
			name[n] = '\0';
			char *val = getenv(name);
			if (!val) val = "";
			int vlen = strlen(val);
			out = realloc(out, o_len + vlen + 1);
			memcpy(out + o_len, val, vlen);
			o_len += vlen;
			out[o_len] = '\0';
		}
		else
		{
			out = realloc(out, o_len + 2);
			out[o_len++] = s[i++];
			out[o_len]   = '\0';
		}
	}

	return out;
}

void expand_variables(t_token *tokens)
{
	for (t_token *t = tokens; t; t = t->next)
	{
		if (t->type == T_WORD)
		{
			// skip expansion for purely single-quoted tokens
			if (t->in_quotes == 2)
				continue;
			char *newval = expand_string(t->value);
			free(t->value);
			t->value = newval;
		}
	}
}