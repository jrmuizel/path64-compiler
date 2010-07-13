


#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>

int arg_targets;

void usage()
{
    printf("Syntax:\n");
    printf("      symlink [-f] target linkname\n");
    printf("Options\n");
    printf("  -f\n");
    printf("       Remove existing destination files.\n\n");
}

int main(int argc, char * argv[])
{
    int opt_force, file1;

    if (argc < 3)
    {
        usage();
        return -1;
    }

    file1 = 1;
    if (strcmp(argv[1], "-f") == 0)
    {
        opt_force = 1;
        file1++;
    }

    if (file1 + 1 >= argc)
    {
        usage();
        return -1;
    }

    return symlink(argv[file1], argv[file1+1]);
}
