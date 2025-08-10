#include "../../includes/minishell.h"
#include "builtins.h"

static void	export_bad_ident(const char *s, int *ret)
{
	ft_putstr_fd("export: `", 2);
	ft_putstr_fd((char *)s, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	*ret = 1;
}

static int	handle_assign(char ***envp, const char *arg)
{
	const char	*eq;
	char		*name;
	const char	*val;
	int			res;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (1);
	name = ft_substr(arg, 0, (unsigned int)(eq - arg));
	if (!name)
		return (1);
	val = eq + 1;
	res = env_set(envp, name, val, 1);
	free(name);
	return (res);
}

static int	handle_bare(char ***envp, const char *arg)
{
	int	idx;

	idx = env_get_index(*envp, arg);
	if (idx >= 0)
		return (0);
	return (env_set(envp, arg, "", 0));
}

int	bltn_export(char **argv, char ***envp)
{
	int	i;
	int	ret;

	if (!argv || !argv[0])
		return (1);
	if (!argv[1])
		return (print_export_vars(*envp));
	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
			export_bad_ident(argv[i], &ret);
		else if (ft_strchr(argv[i], '='))
		{
			if (handle_assign(envp, argv[i]))
				ret = 1;
		}
		else if (handle_bare(envp, argv[i]))
			ret = 1;
		i++;
	}
	return (ret);
}
