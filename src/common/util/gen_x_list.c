
#include <stdio.h>
#include <stdarg.h>

int main(int argc, char * argv[])
{
    char *base_type, *prefix, *l_prefix;
    char filename[260];
    FILE *file;
    int i;

    if (argc < 3)
    {
        fprintf(stderr, "syntax: gen_x_list <type> <prefix> [<include>...]\n");
        return -1;
    }

    base_type = argv[1];
    prefix = argv[2];

    l_prefix = malloc(strlen(prefix) + 1);
    strcpy(l_prefix, prefix);
    strlwr(l_prefix);

    // Generate the .h file
    sprintf(filename, "%s_list.h", prefix);
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", filename);
        return -1;
    }
    fprintf(file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(file, " */\n");
    fprintf(file, "#ifndef %s_list_included\n", l_prefix);
    fprintf(file, "#define %s_list_included\n", l_prefix);
    for (i = 3; i < argc; i++)
        fprintf(file, "#include \"%s\"\n", argv[i]);

    fprintf(file, "#define _X_LIST_TYPE_ %s_LIST\n", prefix);
    fprintf(file, "#define _X_LIST_TAG_ %s_LIST\n", prefix);
    fprintf(file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(file, "#define _X_RCS_ID_ %s_list_rcs_id\n", l_prefix);
    fprintf(file, "#define _X_PUSH_ %s_LIST_Push\n", prefix);
    fprintf(file, "#define _X_DELETE_ %s_LIST_Delete\n", prefix);
    fprintf(file, "#define _X_DELETE_IF_ %s_LIST_Delete_If\n", prefix);
    fprintf(file, "#define %s_LIST_first(x) ((x)->first)\n", prefix);
    fprintf(file, "#define %s_LIST_rest(x) ((x)->rest)\n", prefix);
    fprintf(file, "#define _X_LIST_LOCAL_BASE_TYPE_ %s_LIST_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(file, "#include \"x_list.h\"\n");
    fprintf(file, "#undef _X_LIST_TYPE_\n");
    fprintf(file, "#undef _X_LIST_TAG_\n");
    fprintf(file, "#undef _X_BASE_TYPE_\n");
    fprintf(file, "#undef _X_RCS_ID_\n");
    fprintf(file, "#undef _X_PUSH_\n");
    fprintf(file, "#undef _X_DELETE_\n");
    fprintf(file, "#undef _X_DELETE_IF_\n");
    fprintf(file, "#undef _X_LIST_LOCAL_BASE_TYPE_\n");
    fprintf(file, "#endif\n");
    fclose(file);

    // Generate the .cxx file
    sprintf(filename, "%s_list.cxx", prefix);
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", filename);
        return -1;
    }
    fprintf(file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(file, " */\n");
    for (i = 3; i < argc; i++)
        fprintf(file, "#include \"%s\"\n", argv[i]);
    fprintf(file, "#define _X_LIST_TYPE_ %s_LIST\n", prefix);
    fprintf(file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(file, "#define _X_RCS_ID_ %s_list_rcs_id\n", l_prefix);
    fprintf(file, "#define _X_PUSH_ %s_LIST_Push\n", prefix);
    fprintf(file, "#define _X_DELETE_ %s_LIST_Delete\n", prefix);
    fprintf(file, "#define _X_DELETE_IF_ %s_LIST_Delete_If\n", prefix);
    fprintf(file, "#define _X_LIST_LOCAL_BASE_TYPE_ %s_LIST_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(file, "#include \"x_list.c\"\n");
    fclose(file);

    return 0;
}

