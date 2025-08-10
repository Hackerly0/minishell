#ifndef BULTINS_H
#define BULTINS_H

/* env_utils.c */
int		bltn_export(char **argv, char ***envp);
int		is_valid_identifier(const char *s);
int		env_count(char **env);
int		env_get_index(char **env, const char *name);
int		env_set(char ***envp, const char *name, const char *value, int ow);
int		print_export_vars(char **env);

#endif