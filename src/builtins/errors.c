/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalhoora <oalhoora@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 15:04:20 by oalhoora          #+#    #+#             */
/*   Updated: 2025/07/19 19:42:07 by oalhoora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void error_str(char *msg)
{
    if (msg)
    {
        write(STDERR_FILENO, "minishell: ", 11);
        write(STDERR_FILENO, msg, strlen(msg));
        write(STDERR_FILENO, "\n", 1);
    }
}

void error_cmd(char *cmd, char *msg)
{
    write(STDERR_FILENO, "minishell: ", 11);
    if (cmd)
    {
        write(STDERR_FILENO, cmd, strlen(cmd));
        write(STDERR_FILENO, ": ", 2);
    }
    if (msg)
        write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
}

void error_file(char *filename, char *msg)
{
    write(STDERR_FILENO, "minishell: ", 11);
    if (filename)
    {
        write(STDERR_FILENO, filename, strlen(filename));
        write(STDERR_FILENO, ": ", 2);
    }
    if (msg)
        write(STDERR_FILENO, msg, strlen(msg));
    else
        write(STDERR_FILENO, strerror(errno), strlen(strerror(errno)));
    write(STDERR_FILENO, "\n", 1);
}
