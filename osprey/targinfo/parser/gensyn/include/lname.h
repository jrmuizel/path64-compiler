/** \file lname.h
    \brief list management

*/
#ifndef __LNAME_H__
#define __LNAME_H__

#include "strind.h"
#include "gensyn_defs.h"
#include "cstexp.h"
#include "strind.h"

/** InitDefineLists */
extern void InitDefineLists() ;
/** IsClassDefined */
extern int IsClassDefined(char *name) ;
/** AddNewClassInfo */
extern void AddNewClassInfo (char *name) ;
/** PrintClassList */
extern void PrintClassList (FILE*cfd) ;

#define GENERATE_INCLUDE_SEQUENCE(stream,base)                                                          \
   fprintf (stream, "#define  cvt_to_string(a)                       #a\n");                            \
   fprintf (stream, "#define  str_concat(a,b)                        cvt_to_string(a##b)\n\n");         \
   fprintf (stream, "#define  " base "_fileheader_print(extname)   str_concat(extname,_" base ".h)\n"); \
   fprintf (stream, "#define  " base "_fileheader                  " base "_fileheader_print(%s)\n\n",  \
                    utilsOptionGetScalar( & gensyn_option.short_name ));                                \
   fprintf (stream, "#include " base "_fileheader                  /* Here is it! */\n\n");             \
   fprintf (stream, "#undef   " base "_fileheader_print            /* Safer!      */\n");               \
   fprintf (stream, "#undef   " base "_fileheader                  /* Safer!      */\n");               \
   fprintf (stream, "#undef   str_concat                             /* Safer!      */\n");             \
   fprintf (stream, "#undef   cvt_to_string                          /* Safer!      */\n");

#endif /* __LNAME_H__ */
