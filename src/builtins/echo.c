#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int	builtin_echo(int argc, char **argv)
{
    bool	newline;
    int		i;

	i = 1;
	newline = true;
    if (argc > 1 && strcmp(argv[1], "-n") == 0)
	{
        newline = false;
        i++;
    }
    while (i < argc)
	{
        write(1, argv[i], strlen(argv[i]));
        if (i + 1 < argc)
            write(1, " ", 1);
        i++;
    }
    if (newline)
        write(1, "\n", 1);

    return 0;
}
