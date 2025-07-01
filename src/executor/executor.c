#include "executor.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

/*
 * 1) parse_commands:
 *    - Walk the token list
 *    - Split on T_PIPE into separate t_cmd
 *    - For each cmd: collect T_WORD into argv[], T_*_REDIR + next token as filename
 */
static char **dup_argv(t_token **cur)
{
    // Count words
    int   cnt = 0;
    t_token *t = *cur;
    while (t && t->type == T_WORD)
    {
        cnt++;
        t = t->next;
    }
    // Build argv
    char **argv = malloc((cnt + 1) * sizeof(char*));
    for (int i = 0; i < cnt; i++)
    {
        argv[i] = strdup((*cur)->value);
        *cur = (*cur)->next;
    }
    argv[cnt] = NULL;
    return argv;
}

t_cmd *parse_commands(t_token *tokens, int *out_ncmds)
{
    // First pass: count pipes -> ncmds = pipes+1
    int ncmds = 1;
    for (t_token *t = tokens; t; t = t->next)
        if (t->type == T_PIPE) ncmds++;
    t_cmd *cmds = calloc(ncmds, sizeof(t_cmd));

    t_token *cur = tokens;
    for (int ci = 0; ci < ncmds; ci++)
    {
        cmds[ci].n_redirs = 0;
        cmds[ci].redirs   = NULL;
        // allocate worst-case redirs = token count
        cmds[ci].redirs = malloc(sizeof(t_redir) * 100);

        // gather argv until PIPE or end
        if (cur->type == T_WORD)
            cmds[ci].argv = dup_argv(&cur);
        else
            cmds[ci].argv = calloc(1, sizeof(char*)); // empty argv

        // now handle any redirs in this segment
        while (cur && cur->type != T_PIPE)
        {
            if (cur->type == T_IN_REDIR
             || cur->type == T_HEREDOC
             || cur->type == T_OUT_REDIR
             || cur->type == T_DOUT_REDIR)
            {
                t_redir_type rt;
                if (cur->type == T_IN_REDIR)   rt = RD_INPUT;
                if (cur->type == T_HEREDOC)    rt = RD_HEREDOC;
                if (cur->type == T_OUT_REDIR)  rt = RD_TRUNC;
                if (cur->type == T_DOUT_REDIR) rt = RD_APPEND;
                cur = cur->next; // skip operator
                if (!cur) break; // syntax error
                cmds[ci].redirs[cmds[ci].n_redirs++] = (t_redir){ rt, strdup(cur->value) };
            }
            cur = cur->next;
        }
        if (cur && cur->type == T_PIPE)
            cur = cur->next; // skip the pipe token
    }
    *out_ncmds = ncmds;
    return cmds;
}

/*
 * 2) exec_commands:
 *    - If ncmds>1, create ncmds-1 pipes
 *    - Fork per command, dup2() stdin/stdout from pipes or redirs
 *    - execve or perror
 *    - Parent closes fds and waits
 */
void exec_commands(t_cmd *cmds, int ncmds, char **envp)
{
    int pipes[ ncmds -1 ][2];
    for (int i = 0; i < ncmds-1; i++)
        pipe(pipes[i]);

    for (int i = 0; i < ncmds; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // stdin from previous pipe?
            if (i > 0)
                dup2(pipes[i-1][0], STDIN_FILENO);
            // stdout to next pipe?
            if (i < ncmds-1)
                dup2(pipes[i][1], STDOUT_FILENO);
            // close all pipe fds
            for (int j = 0; j < ncmds-1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            // apply redirections
            for (int r = 0; r < cmds[i].n_redirs; r++)
            {
                t_redir *rd = &cmds[i].redirs[r];
                int fd;
                if (rd->type == RD_INPUT)
                    fd = open(rd->file, O_RDONLY);
                else if (rd->type == RD_TRUNC)
                    fd = open(rd->file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
                else if (rd->type == RD_APPEND)
                    fd = open(rd->file, O_WRONLY|O_CREAT|O_APPEND, 0644);
                else
                    continue; // skip heredoc for now
                if (fd < 0) { perror("open"); exit(1); }
                if (rd->type == RD_INPUT)   dup2(fd, STDIN_FILENO);
                else                         dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            // execute
            execve(cmds[i].argv[0], cmds[i].argv, envp);
            perror("execve");
            exit(1);
        }
    }
    // parent: close pipes + wait
    for (int i = 0; i < ncmds-1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < ncmds; i++)
        wait(NULL);
}

void free_commands(t_cmd *cmds, int ncmds)
{
    for (int i = 0; i < ncmds; i++)
    {
        for (int j = 0; cmds[i].argv[j]; j++)
            free(cmds[i].argv[j]);
        free(cmds[i].argv);
        for (int r = 0; r < cmds[i].n_redirs; r++)
            free(cmds[i].redirs[r].file);
        free(cmds[i].redirs);
    }
    free(cmds);
}