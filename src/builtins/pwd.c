#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int builtin_pwd(int argc, char **argv)
{
	char	*cwd;

    (void)argc;
    (void)argv;
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("pwd");
        return 1;
    }
    write(1, cwd, strlen(cwd));
    write(1, "\n", 1);
    free(cwd);
    return 0;
}