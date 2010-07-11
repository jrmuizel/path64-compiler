
#include <stdio.h>
#include <stdarg.h>

int asprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    char buf[2];
    int len;

    va_start(args, fmt);
    len = _vsnprintf(0,0,fmt,args);
    *strp = malloc(len+2);
    len = _vsnprintf(*strp,len+1,fmt,args);
    va_end(args);
    (*strp)[len] = '\0';
    return len;
}

int main(int argc, char * argv[])
{
    char *base_type, *prefix, *l_prefix, *id_access;
    char *h_rcs_id, *type, *create, *set, *reset, *get, *uniond, *intersect;
    char *h_file_name, *c_file_name;
    FILE *h_file, *c_file;
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
    strlwr(l_prefix);

    // Make the derived names
    asprintf(&h_rcs_id, "%s_prop_rcs_id", l_prefix);
    asprintf(&type, "%s_PROP", prefix);
    asprintf(&create, "%s_PROP_Create", prefix);
    asprintf(&set, "%s_PROP_Set", prefix);
    asprintf(&reset, "%s_PROP_Reset", prefix);
    asprintf(&get, "%s_PROP_Get", prefix);
    asprintf(&uniond, "%s_PROP_UnionD", prefix);
    asprintf(&intersect, "%s_PROP_Intersection_Is_NonEmpty", prefix);

    // Make the names of the output files
    asprintf(&h_file_name, "%s_prop.h", prefix);
    asprintf(&c_file_name, "%s_prop.cxx", prefix);

    // Generate the .h file
    h_file = fopen(h_file_name, "w");
    if (!h_file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", h_file_name);
        return -1;
    }
    fprintf(h_file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(h_file, " */\n");
    fprintf(h_file, "#ifndef %s_prop_included\n", l_prefix);
    fprintf(h_file, "#define %s_prop_included\n", l_prefix);
    fprintf(h_file, "#define _X_PROP_TYPE_ %s\n", type);
    fprintf(h_file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(h_file, "#define _X_RCS_ID_ %s\n", h_rcs_id);
    fprintf(h_file, "#define _X_PROP_CREATE_ %s\n", create);
    fprintf(h_file, "#define _X_PROP_SET_ %s\n", set);
    fprintf(h_file, "#define _X_PROP_RESET_ %s\n", reset);
    fprintf(h_file, "#define _X_PROP_GET_ %s\n", get);
    fprintf(h_file, "#define _X_PROP_UNIOND_ %s\n", uniond);
    fprintf(h_file, "#define _X_PROP_INTERSECTION_IS_NONEMPTY_ %s\n", intersect);
    fprintf(h_file, "#define _X_PROP_LOCAL_BASE_TYPE_ %s_PROP_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(h_file, "#include \"x_prop.h\"\n");
    fprintf(h_file, "#undef _X_PROP_TYPE_\n");
    fprintf(h_file, "#undef _X_BASE_TYPE_\n");
    fprintf(h_file, "#undef _X_RCS_ID_\n");
    fprintf(h_file, "#undef _X_PROP_CREATE_\n");
    fprintf(h_file, "#undef _X_PROP_SET_\n");
    fprintf(h_file, "#undef _X_PROP_RESET_\n");
    fprintf(h_file, "#undef _X_PROP_GET_\n");
    fprintf(h_file, "#undef _X_PROP_UNIOND_\n");
    fprintf(h_file, "#undef _X_PROP_LOCAL_BASE_TYPE_\n");
    fprintf(h_file, "#undef _X_PROP_INTERSECTION_IS_NONEMPTY_\n");
    fprintf(h_file, "#endif\n");
    fclose(h_file);

    // Generate the .cxx file
    c_file = fopen(c_file_name, "w");
    if (!h_file)
    {
        fprintf(stderr, "Error: failed to open '%s'\n", c_file_name);
        return -1;
    }
    fprintf(c_file, "/* Constructed by gen_x_prop $rev\n");
    fprintf(c_file, " */\n");
    for (i = 4; i < argc; i++)
        fprintf(c_file, "#include \"%s\"\n", argv[i]);

    fprintf(c_file, "#define _X_PROP_TYPE_ %s\n", type);
    fprintf(c_file, "#define _X_BASE_TYPE_ %s\n", base_type);
    fprintf(c_file, "#define _X_id_(x) %s\n", id_access);
    fprintf(c_file, "#define _X_PROP_CREATE_ %s\n", create);
    fprintf(c_file, "#define _X_PROP_SET_ %s\n", set);
    fprintf(c_file, "#define _X_PROP_RESET_ %s\n", reset);
    fprintf(c_file, "#define _X_PROP_GET_ %s\n", get);
    fprintf(c_file, "#define _X_PROP_UNIOND_ %s\n", uniond);
    fprintf(c_file, "#define _X_PROP_INTERSECTION_IS_NONEMPTY_ %s\n", intersect);
    fprintf(c_file, "#define _X_PROP_LOCAL_BASE_TYPE_ %s_PROP_LOCAL_BASE_TYPE_\n", prefix);
    fprintf(c_file, "#include \"x_prop.c\"\n");
    fclose(c_file);

    // cleanup
    free(h_rcs_id);
    free(type);
    free(create);
    free(set);
    free(reset);
    free(get);
    free(uniond);
    free(intersect);
    free(h_file_name);
    free(c_file_name);

    return 0;
}

