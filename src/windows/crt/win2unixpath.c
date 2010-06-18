
char *win2unixpath(char *FileName)
{
	char *s = FileName;
	while (*s) {
		if (*s == '\\')
			*s = '/';
		*s++;
	}
	return FileName;
}

