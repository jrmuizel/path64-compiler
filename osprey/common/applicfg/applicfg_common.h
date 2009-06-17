/* 

  Copyright (C) 2009 ST Microelectronics, Inc.  All Rights Reserved. 

  This program is free software; you can redistribute it and/or modify it 
  under the terms of version 2 of the GNU General Public License as 
  published by the Free Software Foundation. 
  This program is distributed in the hope that it would be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

  Further, this software is distributed without any warranty that it is 
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever. 
  You should have received a copy of the GNU General Public License along 
  with this program; if not, write the Free Software Foundation, Inc., 59 
  Temple Place - Suite 330, Boston MA 02111-1307, USA. 

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/

#include <string.h>
#include <stdio.h>


typedef struct string_list *Pt_string_list;
typedef struct func_list *Pt_func_list;
typedef struct file_list *Pt_file_list;
typedef struct cfg_struct *Pt_cfg_struct;
typedef struct applicfg_struct *Pt_applicfg_struct;


typedef struct string_list{
	char *name;
	Pt_string_list next;
} string_list;

typedef struct func_list{
	char *name;
	Pt_string_list options;
	Pt_func_list next;
} func_list;


typedef struct file_list{
	char *name;
	Pt_string_list options;
	Pt_func_list functions;
	Pt_file_list next;
} file_list;


typedef struct cfg_struct {
	char *name;
	Pt_string_list options;
	Pt_file_list files;
	Pt_cfg_struct next;
} cfg_struct;


extern char *active_configuration_name;
extern Pt_cfg_struct appli_configurations;
extern Pt_cfg_struct active_configuration;


/********************************************************************************/
/* initializations procedures                                                   */
/********************************************************************************/
int appliconfig_parser(char *filename);
void add_configuration(Pt_cfg_struct cfg);

Pt_string_list add_string_to_list(Pt_string_list l1, char *opt);
Pt_func_list generate_func_conf(char *name, Pt_string_list l1);
Pt_file_list generate_file_conf(char *name, Pt_file_list l1);
Pt_file_list generate_one_file_conf(Pt_file_list l1, Pt_func_list l2, Pt_string_list l3);
Pt_cfg_struct generate_full_conf(char *name, Pt_cfg_struct l1);
Pt_cfg_struct generate_one_full_conf(Pt_cfg_struct l1, Pt_file_list l2, Pt_string_list l3);

/********************************************************************************/
/* Access functions                                                             */
/********************************************************************************/

void set_active_cfg();
Pt_string_list get_file_options(char *file);
Pt_string_list get_func_options(char *file, char *func);

#ifdef Is_True_On
void dump_cfg();
#endif
