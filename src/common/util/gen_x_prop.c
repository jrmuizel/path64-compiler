
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "string_case.h"

int main(int argc, char * argv[])
{
    char *base_type, *prefix, *l_prefix, *id_access;
    char filename[260];
    FILE *file;
    int i;
    
    if (argc < 4)
    {
        fprintf(stderr, "syntax: gen_x_prop <type> <prefix> <id_access> [<include>...]\n");
        return -1;
    }

    base_type = argv[1];
    prefix = argv[2];
    id_access = argv[3];
    
    l_prefix = malloc(strlen(prefix) + 1);
    strcpy(l_prefix, prefix);
    string_tolower(l_prefix);

    // Generate the .h file
    sprintf(filename, "%s_prop.h", l_prefix);
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", filename);
        return -1;
    }
    fprintf(file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(file, " */\n");
    fprintf(file, "#ifndef %s_prop_included\n", l_prefix);
    fprintf(file, "#define %s_prop_included\n", l_prefix);
    fprintf(file, "#define _X_PROP_TYPE_ %s_PROP\n", prefix);
    fprintf(file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(file, "#define _X_RCS_ID_ %s_prop_rcs_id\n", l_prefix);
    fprintf(file, "#define _X_PROP_CREATE_ %s_PROP_Create\n", prefix);
    fprintf(file, "#define _X_PROP_SET_ %s_PROP_Set\n", prefix);
    fprintf(file, "#define _X_PROP_RESET_ %s_PROP_Reset\n", prefix);
    fprintf(file, "#define _X_PROP_GET_ %s_PROP_Get\n", prefix);
    fprintf(file, "#define _X_PROP_UNIOND_ %s_PROP_UnionD\n", prefix);
    fprintf(file, "#define _X_PROP_INTERSECTION_IS_NONEMPTY_ %s_PROP_Intersection_Is_NonEmpty\n", prefix);
    fprintf(file, "#define _X_PROP_LOCAL_BASE_TYPE_ %s_PROP_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(file, "#include \"x_prop.h\"\n");
    fprintf(file, "#undef _X_PROP_TYPE_\n");
    fprintf(file, "#undef _X_BASE_TYPE_\n");
    fprintf(file, "#undef _X_RCS_ID_\n");
    fprintf(file, "#undef _X_PROP_CREATE_\n");
    fprintf(file, "#undef _X_PROP_SET_\n");
    fprintf(file, "#undef _X_PROP_RESET_\n");
    fprintf(file, "#undef _X_PROP_GET_\n");
    fprintf(file, "#undef _X_PROP_UNIOND_\n");
    fprintf(file, "#undef _X_PROP_LOCAL_BASE_TYPE_\n");
    fprintf(file, "#undef _X_PROP_INTERSECTION_IS_NONEMPTY_\n");
    fprintf(file, "#endif\n");
    fclose(file);

    // Generate the .cxx file
    sprintf(filename, "%s_prop.cxx", l_prefix);
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", filename);
        return -1;
    }
    fprintf(file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(file, " */\n");
    for (i = 4; i < argc; i++)
        fprintf(file, "#include \"%s\"\n", argv[i]);

    fprintf(file, "#define _X_PROP_TYPE_ %s_PROP\n", prefix);
    fprintf(file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(file, "#define _X_id_(x) %s\n", id_access);
    fprintf(file, "#define _X_PROP_CREATE_ %s_PROP_Create\n", prefix);
    fprintf(file, "#define _X_PROP_SET_ %s_PROP_Set\n", prefix);
    fprintf(file, "#define _X_PROP_RESET_ %s_PROP_Reset\n", prefix);
    fprintf(file, "#define _X_PROP_GET_ %s_PROP_Get\n", prefix);
    fprintf(file, "#define _X_PROP_UNIOND_ %s_PROP_UnionD\n", prefix);
    fprintf(file, "#define _X_PROP_INTERSECTION_IS_NONEMPTY_ %s_PROP_Intersection_Is_NonEmpty\n", prefix);
    fprintf(file, "#define _X_PROP_LOCAL_BASE_TYPE_ %s_PROP_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(file, "#include \"x_prop.c\"\n");
    fclose(file);

    return 0;
}

