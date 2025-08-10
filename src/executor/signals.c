// signals.c
#include "../../includes/minishell.h"
#include <readline/readline.h>
#include <unistd.h>

//extern volatile sig_atomic_t g_signal;

/* main prompt handler you already have */
void    signal_handler(int sig)
{
    g_signal = sig;
    if (sig == SIGINT)
    {
        write(1, "\n", 1);
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_done = 1;
    }
}

/* NEW: heredoc-specific SIGINT */
static void hd_sigint(int sig)
{
    (void)sig;
    g_signal = SIGINT;
    write(1, "\n", 1);
    /* make readline("> ") return immediately */
    rl_replace_line("", 0);
    rl_on_new_line();
    close(STDIN_FILENO);
}

/* expose two small helpers to switch modes */
void    setup_signals(void)
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, SIG_IGN);
}

void    setup_hd_signals(void)
{
    signal(SIGINT, hd_sigint);
    signal(SIGQUIT, SIG_IGN);
}
