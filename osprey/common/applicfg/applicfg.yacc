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

%token FL_CONF FL_FILE FL_FUNC FL_STRING FL_OPTION FL_OPEN_C FL_CLOSE_C FL_ACTIVE_CONF

%{
#include "applicfg_common.h"
#include "applicfg_yacc.h"

extern char *yytext;
extern char *lex_ident;
extern int yylineno;
extern int yylex();
extern void yyerror();
char *active_configuration_name;
Pt_cfg_struct active_configuration;
Pt_cfg_struct appli_configurations;

#ifdef APPCFG_DEBUG
 #define DBG_printf(...) printf(...)
#else
 #define DBG_printf(...)
#endif

%}

%union {
    int ival;
	char *name;
	Pt_string_list 	str_list;
	Pt_func_list 	func_list;
	Pt_file_list	file_list;
	Pt_cfg_struct   cfg;
	Pt_applicfg_struct	appcfg;
    }

%type <name>
	FL_OPTION
	FL_STRING	
	active_configuration

%type <str_list>
	file_option
	one_func_conf
	global_option

%type <func_list>
	func_conf

%type <file_list>
	one_file_conf
	file_conf

%type <cfg>
	configuration
	one_configuration

%%


configuration_file:
	configuration_file configuration 			{ add_configuration($2);}
	| configuration_file active_configuration	
	| 
;	

active_configuration:
	FL_ACTIVE_CONF FL_STRING 
		{ 
			if(active_configuration_name != NULL)  { 
				fprintf(stderr,"Active configuration already set to %s \n",active_configuration_name);
				fprintf(stderr,"Last value taken : %s\n",$2);
			}
			active_configuration_name = strdup($2); 
			DBG_printf("Active configuration is %s\n",$2);
		}
;

configuration:
	FL_CONF FL_STRING FL_OPEN_C  one_configuration  FL_CLOSE_C 		
		{ $$ = generate_full_conf($2,$4); DBG_printf("END Parse configuration %s\n",$2);}
;

one_configuration:
	one_configuration file_conf 			{ $$ = generate_one_full_conf($1,$2,NULL); }
	| one_configuration global_option 		{ $$ = generate_one_full_conf($1,NULL,$2); }
	| file_conf								{ $$ = generate_one_full_conf(NULL,$1,NULL); }
	| global_option							{ $$ = generate_one_full_conf(NULL,NULL,$1); }
;


global_option:
	FL_OPTION 						{ $$ = add_string_to_list(NULL,$1); DBG_printf("Found Global Option %s\n",$1);}
;

file_conf:
	FL_FILE FL_STRING FL_OPEN_C one_file_conf  FL_CLOSE_C 		
		{ $$ = generate_file_conf($2,$4); DBG_printf("END Parse file conf %s\n",$2);}
;

one_file_conf:
	 one_file_conf func_conf		{ $$ = generate_one_file_conf($1,$2,NULL); }
	| one_file_conf file_option		{ $$ = generate_one_file_conf($1,NULL,$2); }
	| func_conf						{ $$ = generate_one_file_conf(NULL,$1,NULL); }
	| file_option					{ $$ = generate_one_file_conf(NULL,NULL,$1); }
;

file_option:
	FL_OPTION 					{ $$ = add_string_to_list(NULL,$1); DBG_printf("Found File Option %s\n",$1);}

;

func_conf:
	FL_FUNC FL_STRING FL_OPEN_C one_func_conf  FL_CLOSE_C 		
		{ $$ = generate_func_conf($2,$4); DBG_printf("END Parse file conf %s\n",$2);}
;

one_func_conf:
	one_func_conf FL_OPTION	{ $$ = add_string_to_list($1,$2); DBG_printf("Found func Option %s\n",$2);}
	|
	{$$ = NULL;}
;