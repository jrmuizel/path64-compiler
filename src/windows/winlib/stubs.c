/*
 * Copyright (C) 2010. PathScale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmplrs/rcodes.h>

const char * Anl_File_Path()
{
    fprintf(stderr, "Anl_File_Path() is unimplemented\n");
    return 0;
}

struct rusage;
int getrusage(int who, struct rusage *r_usage)
{
    fprintf(stderr, "getrusage() is unimplemented\n");
    return 0;
}

char *sbrk(int increment)
{
    fprintf(stderr, "sbrk() is unimplemented\n");
    return 0;
}

__int64 
Get_Next_Construct_Id()
{
    fprintf(stderr, "Get_Next_Construct_Id() is unimplemented\n");
    return 0;
}

__int64 
New_Construct_Id()
{
    fprintf(stderr, "New_Construct_Id() is unimplemented\n");
    return 0;
}

int kill(pid_t pid, int sig)
{
    fprintf(stderr, "kill() is unimplemented\n");
    exit(RC_UNIMPLEMENTED_ERROR);
    return 0;
}

int fork()
{
    fprintf(stderr, "fork() is unimplemented\n");
    return -1;
}

int getdomainname(char *name, size_t len)
{
    fprintf(stderr, "getdomainname() is unimplemented\n");
    snprintf(name, len, "fakedomain");
    return 0;
}

void Anl_Process_Command_Line(int phase_argc, char *phase_argv[], int argc, char *argv[])
{
    fprintf(stderr, "Anl_Process_Command_Line() is unimplemented\n");
}

int Anl_Needs_Whirl2c(void)
{
    return 0;
}

int Anl_Needs_Whirl2f(void)
{
    return 0;
}

void Anl_Init(void)
{
    fprintf(stderr, "Anl_Init() is unimplemented\n");
}

int Anl_Init_Map(int *id_map_pool)
{
    fprintf(stderr, "Anl_Init_Map() is unimplemented\n");
    return 0;
}

void Anl_Fini(void)
{
    fprintf(stderr, "Anl_Fini() is unimplemented\n");
}

void Anl_Static_Analysis(int *pu, int id_map)
{
    fprintf(stderr, "Anl_Fini() is unimplemented\n");
}

#if 0
int* Pre_Optimizer (int a1, int* a2, int* a3, int* a4)
{
    fprintf(stderr, "Pre_Optimizer() is unimplemented\n");
    return 0;
}

char *cplus_demangle(const char * a1, int a2)
{
    fprintf(stderr, "cplus_demangle() is unimplemented\n");
    return 0;
}
#endif

