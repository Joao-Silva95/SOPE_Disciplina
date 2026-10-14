// EXAMPLE
// The following program will print on the standard output the
// names of files in the current directory with a .c suffix.

#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	char	*cmd;
	char	buf[BUFSIZ];
	FILE	*ptr;

	cmd = "/bin/ls *.c";
	if ((ptr = popen(cmd, "r")) != NULL)
	{
		while (fgets(buf, BUFSIZ, ptr) != NULL)
			printf("%s", buf);
		pclose(ptr);
	}
	return (0);
}
