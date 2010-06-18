@echo off
rem ====================================================================
rem ====================================================================
rem Module: gen_x_prop.cmd
rem $Revision$
rem $Date$
rem $Author$
rem $Source$
rem Revision history:
rem   27-Feb-1992 - Original version (gen_x_prop.sh)
rem   18-Jun-2010 - Converted to cmd for Windows
rem
rem Synopsis: gen_x_list.sh -   Create an instantiation of externally linked
rem                              lists onto a base type.  This is something
rem                              you really can't do with the CPP.
rem
rem Usage:      sh gen_x_list TYPE PREFIX [INCLUDE...]
rem
rem             Creates the files type_prop.h and type_prop.cxx that
rem             implement a module for manipulating very simple bit
rem             sets over the univers of objects of the given TYPE.
rem             See util/x_prop.h for documentation of the specific
rem             functions generated.
rem
rem             The arguments are:
rem
rem             TYPE  -     is the complete name of the underlying
rem                         type.  For structure types, you probably
rem                         need to include the final *, as they are
rem                         typically passed by reference.  For types
rem                         not passed by reference, such as indices
rem                         and the rare single word structure, omit
rem                         the final *. TYPE is the actual complete
rem                         declaration of the base type parameters of
rem                         the new functions.
rem
rem             PREFIX -    A single identifier to be prepended to the
rem                         name of the new type and newly created
rem                         functions.
rem
rem             INCLUDE   - is a file name to be included in the generated
rem                         .cxx file.  The above expressions all have
rem                         to be parsed and their components defined.
rem                         INCLUDES have to include all the .h files
rem                         necessary for this.  In particular, it
rem                         will probably include xxx.h and memory.h
rem
rem ====================================================================
rem ====================================================================

rem
rem Parse the arguments:
rem
set BASE_TYPE=%1
shift /1
set PREFIX=%1
shift /1

rem
rem Make the derived names:
rem
set rev='%Revision%'
rem We're lucky, Windows file names aren't case sensitive
set L_PREFIX=%PREFIX%
set H_RCS_ID=%L_PREFIX%_list_rcs_id
set TYPE=%PREFIX%_LIST
set FIRST=%PREFIX%_LIST_first
set REST=%PREFIX%_LIST_rest
set PUSH=%PREFIX%_LIST_Push
set DELETE=%PREFIX%_LIST_Delete
set DELETE_IF=%PREFIX%_LIST_Delete_If

rem
rem Make the names of the output files:
rem
set H_FILE="%L_PREFIX%_list.h"
set C_FILE="%L_PREFIX%_list.cxx"

rem
rem Generate the .h file:
rem
echo /* Constructed by gen_x_list %rev%> %H_FILE%
echo  */>> %H_FILE%
echo #ifndef %L_PREFIX%_list_included>> %H_FILE%
echo #define %L_PREFIX%_list_included>> %H_FILE%

rem
rem Generate the .cxx file:
rem
echo /* Constructed by gen_x_list %rev%> %C_FILE%
echo  */>> %C_FILE%

rem
rem Add the #include lines
rem
:for
  if "%1" == "" goto :done
  echo #include "%1">> %H_FILE%
  echo #include "%1">> %C_FILE%
  shift /1
  goto :for
:done

echo #define _X_LIST_TYPE_ %TYPE%>> %H_FILE%
echo #define _X_LIST_TAG_ %TYPE%>> %H_FILE%
echo #define _X_BASE_TYPE_ %BASE_TYPE%>> %H_FILE%
echo #define _X_RCS_ID_ %H_RCS_ID%>> %H_FILE%
echo #define _X_PUSH_ %PUSH%>> %H_FILE%
echo #define _X_DELETE_ %DELETE%>> %H_FILE%
echo #define _X_DELETE_IF_ %DELETE_IF%>> %H_FILE%
echo #define %FIRST%(x) ((x)-^>first)>> %H_FILE%
echo #define %REST%(x)  ((x)-^>rest)>> %H_FILE%
echo #define _X_LIST_LOCAL_BASE_TYPE_ %PREFIX%_LIST_LOCAL_BASE_TYPE_>> %H_FILE%
echo #include "x_list.h">> %H_FILE%
echo #undef _X_LIST_TYPE_>> %H_FILE%
echo #undef _X_LIST_TAG_>> %H_FILE%
echo #undef _X_BASE_TYPE_>> %H_FILE%
echo #undef _X_RCS_ID_>> %H_FILE%
echo #undef _X_PUSH_>> %H_FILE%
echo #undef _X_DELETE_>> %H_FILE%
echo #undef _X_DELETE_IF_>> %H_FILE%
echo #undef _X_LIST_LOCAL_BASE_TYPE_>> %H_FILE%
echo #endif>> %H_FILE%

echo #define _X_LIST_TYPE_ %TYPE%>> %C_FILE%
echo #define _X_BASE_TYPE_ %BASE_TYPE%>> %C_FILE%
echo #define _X_RCS_ID_ %H_RCS_ID%>> %C_FILE%
echo #define _X_PUSH_ %PUSH%>> %C_FILE%
echo #define _X_DELETE_ %DELETE%>> %C_FILE%
echo #define _X_DELETE_IF_ %DELETE_IF%>> %C_FILE%
echo #define _X_LIST_LOCAL_BASE_TYPE_ %PREFIX%_LIST_LOCAL_BASE_TYPE_>> %C_FILE%
echo #include "x_list.c">> %C_FILE%

