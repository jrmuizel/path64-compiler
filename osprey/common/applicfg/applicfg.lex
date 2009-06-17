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
alpha 		[a-zA-Z_$]*
alphamL		[a-zA-Z$]
alphanumerics 	[a-zA-Z_$0-9@]*

%{
#include "applicfg_common.h"
#include "applicfg_yacc.h"

#define MAX_INCLUDE_DEPTH 20
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr=0;
int saved_line_num;
char *tmp;
int line_num;
char *current_compiled_file;
FILE *current_file;
extern int yyparse();
%}

%x FL_comment
%x FL_string
%x FL_include
%%
[\n]				++line_num;
[ \t\r]			;

"/*"				BEGIN(FL_comment);
<FL_comment>[^*\n]*
<FL_comment>[^*\n]*\n		++line_num;
<FL_comment>"*"+[^*/\n]*
<FL_comment>"*"+[^*/\n]*\n	++line_num;
<FL_comment>"*"+"/"		BEGIN(INITIAL);


"\""			BEGIN(FL_string);
<FL_string>[^"\""]+"\""		{
				char *tmp_name;
				BEGIN(INITIAL);
				tmp_name = (char*) malloc(strlen(yytext));
				strncpy(tmp_name,yytext,strlen(yytext)-1);
				tmp_name[strlen(yytext)-1]='\0';
				yylval.name  = strdup(tmp_name); 
				free(tmp_name);
				return (FL_STRING);
				}


"#include"[ \t]*"\""		BEGIN(FL_include);
<FL_include>[ \t]*
<FL_include>[^ "\""\t\n]+"\""	{ 
				if(include_stack_ptr >= MAX_INCLUDE_DEPTH) {
					printf("Parse error in file %s at line %d, exceed include depth\n",current_compiled_file,line_num);
				}
				include_stack[include_stack_ptr++]=YY_CURRENT_BUFFER;
				tmp=current_compiled_file;
				current_compiled_file= malloc(strlen(yytext));
				strncpy(current_compiled_file,yytext,strlen(yytext)-1);
				current_compiled_file[strlen(yytext)-1]='\0';
				yyin=fopen(current_compiled_file,"r");
				if(yyin==NULL) {
					printf("Parse error in file %s at line %d, Unable to open include file %s\n",tmp,line_num,current_compiled_file);
					exit(1);
				}
				saved_line_num=line_num;
				line_num=1;
				yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
				BEGIN(INITIAL);
				}


<<EOF>>			{ 
				if(--include_stack_ptr < 0 ) yyterminate();
				yy_delete_buffer(YY_CURRENT_BUFFER);
				yy_switch_to_buffer(include_stack[include_stack_ptr]);
				free(current_compiled_file);
				current_compiled_file=tmp;
				line_num=saved_line_num;
				}

"configuration"	 	return (FL_CONF);
"file"	 			return (FL_FILE);
"function"	 		return (FL_FUNC);

"active"[ \t]+"configuration" return (FL_ACTIVE_CONF);
"{"	return (FL_OPEN_C);
"}"	return (FL_CLOSE_C);

"-"[^ \t\n\r]+	{yylval.name = strdup(yytext); return (FL_OPTION); }

%%

static void set_applicfg_input_file(FILE *file_pt) {
	yyin = file_pt;
	current_file = yyin;
	line_num = 1;
}


int appliconfig_parser(char *filename) {
	int status=0;
	FILE *source_file;
	current_compiled_file=filename;
	source_file = fopen(filename, "r");
	if (source_file == NULL) {
		printf("Unable to open file %s\n",current_compiled_file);
		return 0;
	}
	set_applicfg_input_file(source_file);
	//printf("<Loading Configuration %s>\n", current_compiled_file);
	active_configuration=NULL;
	appli_configurations=NULL;
	status = yyparse ();
	fclose (source_file);
	//printf("<parsing successfully completed>\n");
	return status;
}


void yyerror (char *s) {
	extern int yychar;
	printf ("\nsyntax error: %s while parsing file : %s\n",s,current_compiled_file);
	printf("yychar = %d, yytext = %s\n", yychar, yytext);
	printf ("yylineno = %d\n", line_num);
}

int yywrap () {	return (1);}
	 


Pt_string_list add_string_to_list(Pt_string_list l1, char *opt) {
	Pt_string_list new_list = (Pt_string_list) malloc(sizeof(string_list));
	new_list->next=NULL;
	new_list->name=strdup(opt);
	if (l1 != NULL) {
		Pt_string_list tmp_pt=l1;
		while (tmp_pt->next) tmp_pt=tmp_pt->next;
		tmp_pt->next=new_list;
		return l1;
	} else return new_list;
}


Pt_func_list generate_func_conf(char *name, Pt_string_list l1) {
	Pt_func_list new_list = (Pt_func_list) malloc(sizeof(func_list));
	new_list->next=NULL;
	new_list->name=strdup(name);
	new_list->options=l1;
	return new_list;
}

Pt_file_list generate_file_conf(char *name, Pt_file_list l1) {
	l1->name=strdup(name);
	return l1;
}

Pt_file_list generate_one_file_conf(Pt_file_list l1, Pt_func_list l2, Pt_string_list l3) {
	if (l1 == NULL) {
		l1 = (Pt_file_list) malloc(sizeof(file_list));
		l1->functions=l2;
		l1->options=l3;
		l1->next=NULL;
		return l1;
	}
	if(l2 != NULL) {
		if (l1->functions != NULL) {
			Pt_func_list tmp_pt=l1->functions;
			while (tmp_pt->next) tmp_pt=tmp_pt->next;
			tmp_pt->next=l2;
		} else l1->functions = l2;
	}
	
	if(l3 != NULL) {
		if (l1->options != NULL) {
			Pt_string_list tmp_pt=l1->options;
			while (tmp_pt->next) tmp_pt=tmp_pt->next;
			tmp_pt->next=l3;
		} else l1->options = l3;
	}
	return l1;
}

Pt_cfg_struct generate_full_conf(char *name, Pt_cfg_struct l1) {
	l1->name=strdup(name);
	return l1;
}

Pt_cfg_struct generate_one_full_conf(Pt_cfg_struct l1, Pt_file_list l2, Pt_string_list l3) {
	if (l1 == NULL) {
		l1 = (Pt_cfg_struct) malloc(sizeof(cfg_struct));
		l1->files=l2;
		l1->options=l3;
		l1->next=NULL;
		return l1;
	}
	
	if(l2 != NULL) {
		if (l1->files != NULL) {
			Pt_file_list tmp_pt=l1->files;
			while (tmp_pt->next) tmp_pt=tmp_pt->next;
			tmp_pt->next=l2;
		} else l1->files = l2;
	}
	
	if(l3 != NULL) {
		if (l1->options != NULL) {
			Pt_string_list tmp_pt=l1->options;
			while (tmp_pt->next) tmp_pt=tmp_pt->next;
			tmp_pt->next=l3;
		} else l1->options = l3;
	}
	return l1;
}

void add_configuration(Pt_cfg_struct cfg) {
	if (appli_configurations == NULL) {
		appli_configurations = cfg;
	} else {
		Pt_cfg_struct tmp_pt;
		tmp_pt=appli_configurations;
		while (tmp_pt->next) tmp_pt=tmp_pt->next;
		tmp_pt->next=cfg;
	}
}

void set_active_cfg() {
	active_configuration=NULL;
	if(!active_configuration_name) return;
	if(!appli_configurations) return;
	active_configuration=appli_configurations;
	while (active_configuration) {
		if (!strcmp(active_configuration->name,active_configuration_name)) {
			return;
		}
		active_configuration=active_configuration->next;
	}
	active_configuration=NULL;
	return;
}

Pt_string_list get_file_options(char *file) {
	Pt_file_list tmp_list;
	if(!active_configuration) return NULL;
	tmp_list=active_configuration->files;
	while(tmp_list) {
		if (!strcmp(tmp_list->name,file)) return  tmp_list->options;
		tmp_list=tmp_list->next;
	}
	return NULL;
}

static Pt_file_list get_file_options_pt(char *file) {
	Pt_file_list tmp_list;
	if(!active_configuration) return NULL;
	tmp_list=active_configuration->files;
	while(tmp_list) {
		if (!strcmp(tmp_list->name,file)) return  tmp_list;
		tmp_list=tmp_list->next;
	}
	return NULL;
}

Pt_string_list get_func_options(char *file, char *func) {
	Pt_func_list tmp_list;
	Pt_file_list file_info=get_file_options_pt(file);
	if(!file_info) return NULL;
	tmp_list=file_info->functions;
	while(tmp_list) {
		if (!strcmp(tmp_list->name,func)) return  tmp_list->options;
		tmp_list=tmp_list->next;
	}
	return NULL;
}

#ifdef Is_True_On
void dump_string_options(Pt_string_list l, int n) {
    Pt_string_list tmp=l;
    int i;
    while(tmp) {
        for (i=0;i<n;i++) printf("\t");
        printf("%s\n",tmp->name);       
        tmp = tmp->next;
    }
}

void dump_func_spec(Pt_func_list l) {
    Pt_func_list tmp=l;
    while(tmp) {
        printf("\t\t %s\n",tmp->name);
        printf("\t\t Options are :\n"); 
        dump_string_options(tmp->options,3);
        tmp = tmp->next;
    }
}

void dump_file_spec(Pt_file_list l) {
    Pt_file_list tmp=l;
    while(tmp) {
        printf("\t %s\n",tmp->name);
        printf("\t Options are :\n"); 
        dump_string_options(tmp->options,2);
        printf("\t Func specification are :\n"); 
        dump_func_spec(tmp->functions);
        tmp = tmp->next;
    }
}


void dump_cfg() {
    Pt_cfg_struct tmp = appli_configurations;
    printf("Check parsed file:\n");
    printf("Active conf is : %s\n",active_configuration_name);
    while (tmp) {
        printf("Current config name is: %s\n",tmp->name);       
        printf("Options are :\n"); 
        dump_string_options(tmp->options,1);
        printf("File specification are :\n"); 
        dump_file_spec(tmp->files);
        tmp = tmp->next;
    }
    if(active_configuration) {
        printf("Active config is set:\n");
        printf("Options are :\n"); 
        dump_string_options(active_configuration->options,1);
        printf("File specification are :\n"); 
        dump_file_spec(active_configuration->files);
    }
}

#endif