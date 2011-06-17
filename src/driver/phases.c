/*
   Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/
/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007, 2008, 2009. PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */


/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/

#ifdef __linux
#define _GNU_SOURCE /* For *asprintf */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#ifdef _WIN32
#include <alloca.h>
#endif
#include "driver_defs.h"

#include "phases.h"
#include "options.h"
#include "lang_defs.h"
#include "option_seen.h"
#include "option_names.h"
#include "string_utils.h"
#include "errors.h"
#include "file_names.h"
#include "file_utils.h"
#include "run.h"
#include "objects.h"
#include "opt_actions.h"
#include "profile_type.h"    /* for PROFILE_TYPE */
#include "get_options.h"
#include "targets.h"

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "license.h"

char *outfile = NULL;		/* from -o <outfile> */
char *prof_file = NULL;	/* executable file for prof to work upon */
char *fb_file = NULL;		/* from -fb_create <feedback-file> */
char *internal_fb_file = NULL;	/* from -fb <feedback-file> */
char *opt_file = NULL;		/* from -fb_opt <feedback-file> */
char *fb_xdir = NULL;		/* dir where pixie emits dso's */
char *fb_cdir = NULL; 		/* dir where pixie emits count files */
char *command_line = NULL;	/* original command line */
char *fb_phase = NULL;         /* from -fb_phase=<phase> */
char *fb_type = NULL;          /* from -fb_type=<type> */
char *source_file = NULL;
char *coco_setfile = NULL;	/* -fcoco[=setfile]	bug 9058 */

#ifdef TARG_X8664
int mregparm = 0;
boolean msseregparm = FALSE;
#endif

boolean multiple_source_files = FALSE;

boolean keep_mp = FALSE;
boolean keep_list = FALSE;
boolean keep_cif = FALSE;
boolean keep_listing = FALSE;
boolean auto_parallelize = FALSE;
boolean Gen_feedback = FALSE;
boolean Use_feedback = FALSE;
boolean Disable_open_mp = FALSE;
boolean Disable_old_mp = FALSE;
boolean O3_flag = FALSE;
boolean use_cpp = FALSE;
boolean expand_ftpp_macros = TRUE;	// bug 2258
int     fortran_line_length = 72; /* Fortran line length */
char roundoff=0;
boolean nocpp_flag = FALSE;

char *ld_library_path = NULL;
char *ld_libraryn32_path = NULL;
char *orig_program_name = NULL;
char *old_ld_library_path = NULL;

extern void turn_down_opt_level (int new_olevel, char *msg);

static string_list_t *ipl_cmds = 0; /* record the user options that needed
				       to be passed to ipl */
static boolean string_md = FALSE;
static boolean string_mmd = FALSE;
extern string_list_t *feedback_files;

extern boolean is_replacement_combo (int);
static void convert_saved_command_line_into_string(void);
static char *make_ii_file_name(char *objname);
static char *make_rii_file_name(char *objname);
static int update_instantiation_info_file(char* ii_file_name, char *sourcefile);
static void write_command_string_into_file(FILE *cmdfile,
					   char *sourcefile,
					   int for_ii_file);
static void add_command_line_arg(string_list_t *args, char *sourcefile);

static void do_f90_common_args(string_list_t *args) ;
static void set_f90_source_form(string_list_t *args,boolean set_line_length) ;
static void set_stack_size();

static phases_t
post_fe_phase (void);

static boolean
previous_mf_exists (string_list_t *list)
{
	string_item_t * p;
	
	for (p = list->head; p != NULL; p = p->next) {
		if (strcmp (p->name, "-MF") == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

static void
remove_previous_mf (string_list_t *list)
{
	string_item_t * p;
	
	for (p = list->head; p != NULL; p = p->next) {
		if (strcmp (p->name, "-MF") == 0) {
			replace_string (list, p->name, "");
			replace_string (list, p->next->name, "");
		}
	}
}

static void
add_implied_string (string_list_t *list, int iflag, int flag, phases_t phase)
{
	/* assume inside a FOREACH_IMPLIED_OPTION iteration */
	char *iname = get_current_implied_name();

	if (strncmp (iname, "-MF", 3) == 0) {
		remove_previous_mf (list);
	}

#ifdef PATH64_ENABLE_PSCRUNTIME
	//we intercept -Wp,<option> and pass <option>
        //this should be done if we call cc142 explicitly
        //common case is like -Wp,-MD,scripts/basic/.fixdep.d
	if (!strncmp(iname, "-Wp,", 4)) {
	    // cut away -Wp, prefix and split the rest in pieces at each ","
	    int i, prev;
	    char *s;
	    iname[3] = '\0';
	    iname+=3;
	    prev = 1;
	    s = iname;
	    for (i = 1; s[i]; i++) {
		if (s[i] == ',') {
		    s[i] = '\0';
		    add_string(list, s + prev);
		    prev = i+1;
                    break;
		}
	    }
	    add_string(list, s + prev);
	}
#endif //PSCRUNTIME

	// Coco Fortran preprocessor.
	if (phase == P_f_coco) {
	  // Skip implied -D options.
	  char *name = get_option_name(flag);
	  if (!strncmp(iname, "-D", 2) &&
	      strncmp(name, "-D", 2))
	    return;

	  // Skip -O options.
	  if (!strncmp(iname, "-O", 2))
	    return;
	}

	if (option_has_blank(iflag))
		/* add a string for each blank-separated sub-string. */
		/* for -MT, only recognize the first blank (bug 982) */
		add_multi_strings(list, iname, strncmp(iname, "-MT", 3) == 0);
	else {
	        /* Fix for BUG 249 */
		if ( strcmp(iname, "-MD") ==0 ) {
			if ( string_md == FALSE ) {
				add_string(list, "-M");
				if (!previous_mf_exists (list)) {
				  add_string(list, "-MF");
				  // bug 3342: if -o was specified, use it
				  // including the entire pathname and pass
				  // to -MF so that the .d file is created in
				  // the out dir. Also use -MQ to change the
				  // target of the dependency rule to use
				  // outfile.
				  if (outfile)
				  {
				    add_string(list, change_suffix(outfile, "d"));
				    add_string(list, "-MQ");
				    add_string(list, outfile);
				  }
				  else
				    add_string(list, change_suffix(drop_path(source_file), "d"));
				}
				string_md = TRUE;
			}
			else {
				string_md = FALSE;
				add_string(list, iname);
			}
		}
		/* Fix for bug 433 and friends */
		else if ( strcmp(iname, "-MMD") ==0 ) {
			if ( string_mmd == FALSE ) {
				add_string(list, "-MM");
				if (!previous_mf_exists (list)) {
				  add_string(list, "-MF");
				  // bug 3342: see comments above
				  if (outfile)
				  {
				    add_string(list, change_suffix(outfile, "d"));
				    add_string(list, "-MQ");
				    add_string(list, outfile);
				  }
				  else
				    add_string(list, change_suffix(drop_path(source_file), "d"));
				}
				string_mmd = TRUE;
			}
			else {
				string_mmd = FALSE;
				add_string(list, iname);
			}
		}
		else
			add_string(list, iname);
	}

}

static void
copy_phase_options (string_list_t *phase_list, phases_t phase)
{
	int flag;
	int iflag;
	FOREACH_OPTION_SEEN(flag) {
		FOREACH_IMPLIED_OPTION(iflag, flag) {
			boolean matches_phase = FALSE;

			/* Only add if option is legal for phase and lang.
			 * Make sure it matches both parent and implied lang.
			 * Also, don't add object options. */

			// Hack to filter out -OPT: options to spin_cc1,
			// which takes all front-end options except -OPT:.  Bug
			// 10209.  (Why is -OPT classified as a front-end
			// option in the first place?)
			//
			// Pass -OPT: options to wgen for bug 10262.
			if (!strcmp("-OPT:", get_option_name(iflag))) 
			{
#ifdef PATH64_ENABLE_GNU_FRONTEND
			  if (phase == P_spin_cc1 ||
			      phase == P_spin_cc1plus)
			    continue;
			  else if (phase == P_wgen)
			    matches_phase = TRUE;
			  else
#endif // PATH64_ENABLE_GNU_FRONTEND
			    matches_phase = option_matches_phase(iflag, phase);
			}
#ifdef PATH64_ENABLE_GNU_FRONTEND
			else if(strcmp("-fpic", get_option_name(iflag)) == 0 && phase == P_wgen)
			    matches_phase = TRUE;
            else if(strcmp("-fPIC", get_option_name(iflag)) == 0 && phase == P_wgen)
                matches_phase = TRUE;
#endif // PATH64_ENABLE_GNU_FRONTEND
			else
			    matches_phase = option_matches_phase(iflag, phase);

			if (matches_phase
			    && option_matches_language(flag, source_lang)
			    && !is_object_option(iflag) )
			{
			    /* add check for cpp options that transcend real
			     * source language. */
			    if (option_matches_language(iflag, source_lang)
				|| (is_matching_phase(get_phase_mask(phase), P_any_cpp)
				    && option_matches_language(iflag, L_cpp) ) )
			    {
				add_implied_string (phase_list, iflag, flag,
						    phase);
			    }
			}
		}
	}
}

/* ====================================================================
 *
 * add_language_option
 *
 * The back end needs to know the language, so that -LIST:cite can
 * determine which of whirl2c/whirl2f to invoke.  Pass it as the
 * no-name -LANG: suboption.
 *
 * ====================================================================
 */

static void
add_language_option ( string_list_t *args )
{
  switch ( invoked_lang ) {
    case L_f90:
	add_string ( args, "-LANG:=f90" );
	break;
    case L_cc:
	if ( ansi == KR_ANSI ) {
	  add_string ( args, "-LANG:=kr_c" );
	} else {
	  add_string ( args, "-LANG:=ansi_c" );
	}
	break;
    case L_CC:
	add_string ( args, "-LANG:=cplus" );
	break;
  }
}

/* ====================================================================
 *
 * add_targ_options
 *
 * Add -TARG options to the back-end.
 *
 * ====================================================================
 */

static void
add_targ_options ( string_list_t *args )
{
  // -TARG:abi=xxx
  if(abi == ABI_M32 || abi == ABI_N32) {
    add_string(args, "-TARG:abi=n32");
  } else {
    add_string(args, "-TARG:abi=n64");
  }

  // -TARG:processor=xxx
  if (target_cpu != NULL) {
    char buf[100];
    sprintf(buf, "-TARG:processor=%s", target_cpu);
    add_string(args, buf);
  }

#ifdef TARG_X8664
  if (is_target_arch_X8664()) {
    // SSE, SSE2, SSE3, 3DNow, SSE4a
    if (sse == TRUE)
      add_string(args, "-TARG:sse=on");
    else
      add_string(args, "-TARG:sse=off");

    if (sse2 == TRUE)
      add_string(args, "-TARG:sse2=on");
    else
      add_string(args, "-TARG:sse2=off");

    if (sse3 == TRUE)
      add_string(args, "-TARG:sse3=on");
    else
      add_string(args, "-TARG:sse3=off");

    if (ssse3 == TRUE)
      add_string(args, "-TARG:ssse3=on");
    else
      add_string(args, "-TARG:ssse3=off");

    if (sse4a == TRUE)
      add_string(args, "-TARG:sse4a=on");
    else
      add_string(args, "-TARG:sse4a=off");

    if (sse4_2 == TRUE)
      add_string(args, "-TARG:sse4_1=on");
    else
      add_string(args, "-TARG:sse4_1=off");

    if (sse4_2 == TRUE)
      add_string(args, "-TARG:sse4_2=on");
    else
      add_string(args, "-TARG:sse4_2=off");

    if (m3dnow == TRUE)
      add_string(args, "-TARG:3dnow=on");
    else
      add_string(args, "-TARG:3dnow=off");
  }
#endif
}

static char basebuf[4096];

char *driver_basename(char *const s)
{
    register char *p;
    register char *const t = basebuf;

    if (s == NULL || *s == 0) {
	return strcpy(t, ".");
    } else {
	p = strcpy(t, s);
	p += strlen(p);
        while( p != t  &&  *--p == '/' )        /* skip trailing /s */
	    *p = '\0';
        while( p != t ) {
	    if( *--p == '/' )
		return  ++p;
	}
	return p;
    }
}
static char dirbuf[4096];

char *dirname(char *const s)
{
    register char *p;
    register char *const t = dirbuf;

    if (s == NULL || *s == 0) {
	return strcpy(t, ".");
    } else {
	p = strcpy(t, s);
	p += strlen(p);
        while( p != t  &&  *--p == '/' )        /* skip trailing /s */
	    ;

        if ( p == t && *p == '/' )
	    return strcpy(t, "/");

        while( p != t ) {
	    if( *--p == '/' ) {
		if ( p == t )
		    return strcpy(t, "/");
		while ( *p == '/' )
		    p--;
		*++p = '\0';
		return  t;
	    }
	}
	return strcpy(t, ".");
    }
}


static char *input_source ;	/* src to next phase */

static void add_arg(string_list_t *args, const char *format, ...)
	__attribute__((format (printf, 2, 3)));

static void
add_arg(string_list_t *args, const char *format, ...)
{
	char *arg;
	va_list ap;

	va_start(ap, format);

	vasprintf(&arg, format, ap);
	add_string(args, arg);
	free(arg);

	va_end(ap);
}


// Returns path to library directory


/* Keep this in sync with print_file_path over in opt_actions.c. */

static void
set_library_paths(string_list_t *args)
{
    char *lib_path = target_library_path();
	char *arg = concat_strings("-L", lib_path);
    add_string(args, arg);

    free(lib_path);
    free(arg);
}

/*
 * If we are compiling stdin, we only pass the name we're given ("-")
 * to the preprocessor.  For every other phase, we must fake up a
 * name, so that the default output file name will have a sensible
 * extension.
 */
static char *
fix_name_by_phase (char *name, phases_t phase)
{
	char *new_name = name;
	
	if (name != NULL && strcmp(name, "-") == 0) {
		switch (phase) {
		case P_cpp:
#ifdef PATH64_ENABLE_GNU_FRONTEND
		case P_gcpp:
		case P_gcpp_plus:
#endif // PATH64_ENABLE_GNU_FRONTEND
#ifdef PATH64_ENABLE_PSCLANG
        case P_psclang_cpp:
#endif // PATH64_ENABLE_PSCLANG
			break;
		default:
			switch (source_kind) {
			case S_f:
			case S_F:
			case S_f90:
			case S_F90:
				new_name = "stdin.f90";
				break;
			case S_C:
			case S_ii:
				new_name = "stdin.cc";
				break;
			case S_s:
			case S_S:
				new_name = "stdin.s";
				break;
			case S_c:
			default:
				new_name = "stdin.c";
				break;
			}
		}
	}

	return new_name;
}

char *
fix_name_by_lang(char *name)
{
	char *new_name = name;

	if (name != NULL && strcmp(name, "-") == 0) {
		switch (source_lang) {
		case L_f90:
			new_name = "stdin.f";
			break;
		case L_as:
			new_name = "stdin.s";
			break;
		case L_CC:
			new_name = "stdin.cc";
			break;
		default:
			new_name = "stdin.c";
		}
	}
	return new_name;
}
		
static void
add_inc_path(string_list_t *args, const char *fmt, ...)
	__attribute__((format (printf, 2, 3)));
    
static void
add_inc_path(string_list_t *args, const char *fmt, ...)
{
	char *path;
	va_list ap;
	
	va_start(ap, fmt);
	vasprintf(&path, fmt, ap);
	if (is_directory(path)) {
		add_string(args, "-isystem");
		add_string(args, path);
	}
	free(path);
	va_end(ap);
}

boolean platform_is_64bit(void)
{
    int platform_abi = get_platform_abi();
    return platform_abi == ABI_64 || platform_abi == ABI_M64;
}


// Adds common preprocessor definitions
void add_common_cpp_definitions(string_list_t *args) {
    if (option_was_seen(O_pthread))
        add_string(args, "-D_REENTRANT");

    if (!option_was_seen(O_no_pathcc)) {
        add_string(args, "-D__PATHSCALE__=\"" PSC_FULL_VERSION "\"");
        add_string(args, "-D__PATHCC__=" PSC_MAJOR_VERSION);
        add_string(args, "-D__PATHCC_MINOR__=" PSC_MINOR_VERSION);
        add_string(args, "-D__PATHCC_PATCHLEVEL__=" PSC_PATCH_LEVEL);
    }
}


// Like add_file_args but add args that must precede options specified on the
// command line.
static void
add_file_args_first (string_list_t *args, phases_t index)
{
  switch (index) {
#ifdef PATH64_ENABLE_GNU_FRONTEND
    case P_gcpp:
    case P_gcpp_plus:
      // -Dfoo before user options, since user might specify -Ufoo.  Bug 6874.
      add_common_cpp_definitions(args);
      break;
#endif // PATH64_ENABLE_GNU_FRONTEND

#ifdef PATH64_ENABLE_PSCLANG
    case P_psclang_cpp:
      add_common_cpp_definitions(args);
      break;

    case P_psclang:
#endif // PATH64_ENABLE_PSCLANG

    default:
      break;
  }
}


#if defined(TARG_MIPS) && !defined(ARCH_MIPS)
#define MIPS_CROSS_LIB_TOP_DIR "/usr/mips64el-gentoo-linux-gnu"
#endif

/* Bug 11265 */
/*
 * Normally "-isystem" can be handled entirely by cpp. For Fortran, we want
 * the -isystem directories to be inserted in the module-search list after
 * the directories specified by the user ("-I whatever", which gets mapped
 * onto "-include=whatever") but before any "system" directory(s) like
 * "-include=<PSC_FULL_VERSION>/include" specified by the driver. It seems
 * safer and simpler to do that here, since the Fortran front end doesn't
 * know where the user directories end and the system directories start.
 */
static void
add_isystem_dirs(string_list_t *args)
{
  string_item_t *p;
  if (isystem_dirs) {
    for (p = isystem_dirs->head; p != NULL; p = p->next)  {
      add_string(args, p->name);
    }
  }
}
/* Bug 11265 */
/* Mac port */
static void
add_abi(string_list_t *args) {
#ifdef TARG_X8664
    if (is_target_arch_X8664()) {
#if defined(BUILD_OS_DARWIN) || defined(__sun)
		add_string(args, (abi == ABI_M32) ? "-m32" : "-m64");
#else /* defined(BUILD_OS_DARWIN) */
		if( abi == ABI_M32 ){
		  add_string(args, "-m32");
		}
#endif /* defined(BUILD_OS_DARWIN) */
    }
#endif /* TARG_X8664 */
#ifdef TARG_MIPS
    if (is_target_arch_MIPS()) {
		if( abi == ABI_N32 )
		  add_string(args, "-mabi=n32");
		else
		  add_string(args, "-mabi=64");
    }
#endif /* TARG_MIPS */
}


// Adds target specific linker args
static void
add_target_linker_args(string_list_t *args, boolean is_ipa) {
    // Adding path to dynamic linker for linux and netbsd platforms
#if defined(__linux__) /* LINUX */
    char *dyn_link_opt = concat_strings("--dynamic-linker=",
                                        target_dynamic_linker());
    add_arg(args, "%s", dyn_link_opt);
    free(dyn_link_opt);
#elif defined(__NetBSD__)
    add_arg(args, "--dynamic-linker=/libexec/ld.elf_so");
#endif /* LINUX */

    // Adding -rpath argument
    {
        if(!option_was_seen(O_nodefaultlibs) &&
           !option_was_seen(O_nostdlib)) {
            char * lib_path = target_library_path();
            add_arg(args, "-rpath");
            add_arg(args, "%s", lib_path);
            free(lib_path);
        }
    }

    // Adding abi flag
#ifdef TARG_X8664
    if(is_target_arch_X8664()) {
#if defined(__sun)
        if((abi == ABI_M64 || abi == ABI_64) && !is_ipa) {
            add_string(args, "-64");
        }
#elif defined(__FreeBSD__)
    add_string(args, (abi == ABI_N32) ? "-melf_i386_fbsd" : "-melf_x86_64_fbsd");
#else
        add_string(args, (abi == ABI_M32) ? "-melf_i386" : "-melf_x86_64");
#endif
    }
#endif // TARG_X8664
}


/* Mac port */

#ifdef TARG_MIPS
static void
add_sysroot(string_list_t *args, phases_t phase)  // 15149
{
  buffer_t buf;
  char *sysroot = (abi == ABI_N32 ? sysroot_path_n32 : sysroot_path_64);
  if (sysroot == NULL) sysroot = sysroot_path;
  if (sysroot == NULL) return;
  switch (phase) {
  case P_ld:
  case P_ldplus:
  case P_collect:
  case P_ipa_link:
    sprintf(buf, "--sysroot=%s", sysroot);
    add_string(args, buf);
    break;
  case P_f90_fe:
  case P_cppf90_fe:
    /* 15149: Currently, -isysroot (and hence -sysroot) is not honored by
     * the Fortran frontends.  As a workaround, we generate some extra
     * -include=<path> options.
     */
    sprintf(buf, "-include=%s/usr/local/include", sysroot);
    add_string(args, buf);
    sprintf(buf, "-include=%s/usr/include", sysroot);
    add_string(args, buf);
    break;
#ifdef PATH64_ENABLE_GNU_FRONTEND
  case P_gcpp:
  case P_gcpp_plus:
#endif // PATH64_ENABLE_GNU_FRONTEND
#ifdef PATH64_ENABLE_PSCLANG
  case P_psclang_cpp:
#endif // PATH64_ENABLE_PSCLANG
    add_string(args, "-isysroot");
    add_string(args, sysroot);
    break;
  default:
    internal_error("add_sysroot: unexpected phase");
  }
}
#endif


// Adds input asm source to args.  Append .s to input source if
// it doesn't already end in .s.
void add_asm_source(string_list_t *args, const char *isrc) {
    buffer_t buf;
    int len = strlen(isrc);
    if (isrc[len - 1] != 's' ||
        isrc[len - 2] != '.') {

        sprintf(buf, "%s.s", isrc);
        add_string(args, buf);
    } else {
        add_string(args, isrc);
    }
}


// Adds output for asm phase
void add_asm_output(string_list_t *args, const char *the_file) {
    add_string(args, "-o");
    /* cc -c -o <file> puts output from as in <file>,
     * unless there are multiple source files. */
    if (outfile != NULL
        && last_phase == P_any_as
        && !multiple_source_files
       ) {
#ifdef FAT_WHIRL_OBJECTS
        if(ipa == TRUE){
            strcpy(buf, ".native-");
            strcat(buf, outfile);
            add_string(args, buf);
        } else
#endif
            add_string(args, outfile);
    } else {
        // bug 2025
        // Create .o files in /tmp in case the src dir is not
        // writable.
	char *the_file_copy = strdup(the_file);
        if (!(keep_flag ||
             (ipa == TRUE) ||
             remember_last_phase == P_any_as)) {
            char *temp_obj_file = get_object_file (the_file_copy);
            add_string(args, temp_obj_file);
        } else {
            char * o_name;
            o_name = construct_given_name(the_file_copy,"o",
              (keep_flag || multiple_source_files || last_phase == P_any_as || 
                            ((shared == RELOCATABLE) && (ipa == TRUE))) ? TRUE : FALSE);
#ifdef FAT_WHIRL_OBJECTS
            if(ipa == TRUE) {
                strcpy(buf, ".native-");
                strcat(buf, o_name);
                add_string(args, buf);
            } else 
#endif //FAT_WHIRL_OBJECTS
                add_string(args, o_name);
        }
	free(the_file_copy);
    }
}


#ifdef TARG_X8664
static void
add_sse_cc1_options(string_list_t *args, boolean sys_cpp)
{
	if (sse == TRUE)
		add_string(args, "-msse");
	else {
		// -mno-sse is broken in cc142/cc1plus42 front-end
		// (see COMPILER-8935)
		// add_string(args, "-mno-sse");
	}
		
	if (sse2 == TRUE)
		add_string(args, "-msse2");
	else
		add_string(args, "-mno-sse2");

	if (sse3 == TRUE)
		add_string(args, "-msse3");
	else
		add_string(args, "-mno-sse3");

	// ssse3/sse4 options are not supported in gcc-4.2, use them
	// only when using cc142/cc142plus
#ifdef PATH64_ENABLE_PSCRUNTIME
	if(!sys_cpp) {
		if (ssse3 == TRUE)
			add_string(args, "-mssse3");
		else
			add_string(args, "-mno-ssse3");
        
		if (sse4a == TRUE)
			add_string(args, "-msse4a");
		else
			add_string(args, "-mno-sse4a");
        
		if (sse4_1 == TRUE)
			add_string(args, "-msse4_1");
		else
			add_string(args, "-mno-sse4_1");
        
		if (sse4_2 == TRUE)
			add_string(args, "-msse4_2");
		else
			add_string(args, "-mno-sse4_2");
        }
#endif // PATH64_ENABLE_PSCRUNTIME
}
#endif // TARG_X8664


#ifdef PATH64_ENABLE_PSCRUNTIME
// Adds standard include directories for preprocessor
void add_std_includes(string_list_t *args) {
    char *root;
    char *exe_dir;

    if(option_was_seen(O_nostdinc))
        return;

    exe_dir = get_executable_dir();
    root = directory_path(exe_dir);
    free(exe_dir);

    if (!option_was_seen(O_nostdinc__)) {
        add_inc_path(args, "%s/include/" PSC_FULL_VERSION, root);
    }

    if (source_lang == L_CC) {

        add_arg(args, "-D__STDCXX_CONFIG=<__stl_config-%s.h>",
                current_target->targ_name);

        if(option_was_seen(O_fstrict_stl)) {
            add_arg(args, "-D_RWSTD_STRICT_HEADERS");
        }

        // Claim to be compatible with GCC 4.2.1
        add_arg(args, "-D__GNUC__=4");
        add_arg(args, "-D__GNUC_MINOR__=2");
        add_arg(args, "-D__GNUC_PATCHLEVEL__=1");
        add_arg(args, "-D__GXX_ABI_VERSION=1002");

        if(!option_was_seen(O_nostdinc__)) {
            add_inc_path(args, "%s/include/" PSC_FULL_VERSION "/stl", root);
            add_inc_path(args, "%s/include/" PSC_FULL_VERSION "/stl/ansi", root);
            if(stl_threadsafe) {
                add_string(args,"-D_RWSTD_POSIX_THREADS");
            }
            add_string(args,"-nostdinc++");
        }
    }

    add_inc_path(args, "%s/lib/" PSC_FULL_VERSION "/include", root);
    free(root);
}
#endif // PATH64_ENABLE_PSCRUNTIME


// Adds language args for psclang
void add_psclang_language_args(string_list_t *args) {
    add_string(args, "-x");
    switch (source_lang) {
    case L_as:
        add_string(args, "assembler-with-cpp");
        break;
    case L_CC:
        add_string(args, "c++");
        break;
    case L_cc:
    default:
    	add_string(args, "c");
    	break;
    }
}


static void
add_file_args (string_list_t *args, phases_t index)
{
	buffer_t buf;
	char *temp;
	string_item_t *p;
	char *count_file_name;
	char *the_file = fix_name_by_phase(source_file, index);

	/* current_phase is used to say which file might be the last output 
	 * file and thus might need a local name rather than a tmp name */
	current_phase = P_NONE;
	switch (index) {
	case P_cpp:
		if (source_lang == L_as) {
			add_string(args, "-Xdo_linesplice");
			add_string(args, "-dollar");
		}
		/* fallthru */
#ifdef PATH64_ENABLE_GNU_FRONTEND
	case P_gcpp:
	case P_gcpp_plus:
#endif // PATH64_ENABLE_GNU_FRONTEND
#ifdef TARG_X8664
        if(is_target_arch_X8664())
            add_sse_cc1_options(args, index == P_cpp);
#endif // TARG_X8664

		if (show_but_not_run)
			add_string(args, "-###");
#ifdef PATH64_ENABLE_PSCRUNTIME
		if (quiet_flag) 
			add_string(args, "-quiet");
#endif // PATH64_ENABLE_PSCRUNTIME
#ifdef TARG_MIPS
        if (is_target_arch_MIPS())
            add_sysroot(args, index);
#endif
#ifdef PATH64_ENABLE_GNU_FRONTEND
#ifndef PATH64_ENABLE_PSCRUNTIME
#ifdef CROSS_COMPILATION
		{
			char *phase_name = get_full_phase_name(index);
		    if (strcmp(phase_name + strlen(phase_name) - 3, index == P_gcpp ? "gcc" : "g++") == 0) {
		    	/* Cross compilers may install cc1 at non-default location,
		    	 * make sure gcc -E gets the same copy corresponding to our SPIN GCC */
		    	add_string(args, concat_strings("-B", get_phase_dir(index == P_gcpp ? P_spin_cc1: P_spin_cc1plus)));
		    }
		}
#endif /* CROSS_COMPILATION */
#endif // !PATH64_ENABLE_PSCRUNTIME
#endif // PATH64_ENABLE_GNU_FRONTEND
		add_abi(args);

		if( ospace == TRUE ){	// bug 4953
		  add_string(args, "-Os");
		}
		
#ifndef PATH64_ENABLE_PSCRUNTIME
		switch (source_lang) {
		case L_as:
			add_string(args, "-xassembler-with-cpp");
			break;
		case L_CC:
			break;
		case L_f90:
			add_string(args, "-traditional");

			if (!option_was_seen(O_Wendif_labels))
				add_string(args, "-Wno-endif-labels");
		case L_cc:
		default:
			break;
		}
#else // !PATH64_ENABLE_PSCRUNTIME
                switch (source_lang) {
                case L_as:
                    add_string(args, "-lang-asm");
                    break;
                  
		case L_f90:
                    add_string(args, "-traditional-cpp");
                    break;

                default:
                    break;
                }
#endif // !PATH64_ENABLE_PSCRUNTIME

		if (gnu_exceptions == FALSE &&		// bug 11732
		    (source_lang == L_cc ||
		     source_lang == L_CC)) {
		  add_string(args, "-fno-exceptions");
		}

		if (mpkind == NORMAL_MP) { 
#ifndef PATH64_ENABLE_PSCRUNTIME
		  int v[4];
		  get_gcc_version(v, 4);
		  if (v[0] == 4 &&	// GCC 4.2 or greater.  Bug 14856.
		      v[1] >= 2) {
#endif // !PATH64_ENABLE_PSCRUNTIME
		    add_string(args, "-fopenmp");
#ifndef PATH64_ENABLE_PSCRUNTIME
		  }
#endif // !PATH64_ENABLE_PSCRUNTIME
		}

#ifdef PATH64_ENABLE_PSCRUNTIME
		if (source_lang == L_CC) {
			add_string(args, "-D_GNU_SOURCE");
		}
#endif // PATH64_ENABLE_PSCRUNTIME

#ifndef PATH64_ENABLE_PSCRUNTIME
		if (!option_was_seen(O_nostdinc)) {
			char *root = directory_path(get_executable_dir());
			add_inc_path(args, "%s/include/" PSC_FULL_VERSION,
				     root);
			if (source_lang == L_CC) {
				int v[4];
				get_gcc_version(v, 4);
				if (v[0] > 3 || (v[0] == 3 && v[1] >= 3)) {
					add_inc_path(args, "%s/include/"
						     PSC_FULL_VERSION
						     "/backward",
						     root);
				}
			}
			add_inc_path(args, "%s/include", root);
			add_inc_path(args, "%s/lib/" PSC_FULL_VERSION "/include", root);
		}
#else
        add_std_includes(args);
#endif //PATH64_ENABLE_PSCRUNTIME
		
		// Call gcc preprocessor using "gcc -E ...".
		if (source_kind != S_h)	// SiCortes 5034
		  add_string(args, "-E");

#ifdef TARG_X8664 
        if (is_target_arch_X8664()) {
		  // Add a workaround for bug 3082 and bug 6186.
		  add_string(args, "-mfpmath=387");
        }
#endif

        add_string(args, "-U_FORTIFY_SOURCE");
        add_string(args, "-U__GNUC__");
        add_string(args, "-D__GNUC__=4");
        add_string(args, "-U__GNUC_MINOR__");
        add_string(args, "-D__GNUC_MINOR__=2");
        add_string(args, "-U__GNUC_PATCHLEVEL__");
        add_string(args, "-D__GNUC_PATCHLEVEL__=1");
		
		add_string(args, input_source);
		if (source_kind == S_h)	{	// SiCortex 5034
		  if (outfile != NULL) {
		    add_string(args, "-o");
		    add_string(args, outfile);
		  }
		} else
		if (option_was_seen(O_E) && outfile != NULL) {
			add_string(args, "-o");
			add_string(args, outfile);
		}
		else if (last_phase != P_any_cpp) {
			current_phase = P_any_cpp;
			if (source_lang == L_CC)
			    input_source = construct_name(input_source,"ii");
			else if (source_lang == L_as) {
			    input_source = construct_name(input_source,"s");
			}
			else
			    input_source = construct_name(input_source,"i");
			add_string(args, "-o");		// gcc -o ...
			add_string(args, input_source);
		}

		// processing options for generating dependencies

		if(option_was_seen(O_M))
			add_string(args, "-M");

		if(option_was_seen(O_MD)) {
			add_string(args, "-MD");
#ifdef PATH64_ENABLE_PSCRUNTIME
			if (!previous_mf_exists (args)) {
			    if (outfile) {
				add_string(args, change_suffix(outfile, "d"));
				add_string(args, "-MQ");
				add_string(args, outfile);
			    } else {
				add_string(args, change_suffix(drop_path(source_file), "d"));
			    }
			} else {
			    add_string(args, dependency_file);
			}
#endif
		}

		if(dependency_file != NULL) {
			remove_previous_mf (args);
			add_string(args, "-MF");
			add_string(args, dependency_file);
		}

		if(option_was_seen(O_MM))
			add_string(args, "-MM");

		if(option_was_seen(O_MMD)) {
			add_string(args, "-MMD");
#ifdef PATH64_ENABLE_PSCRUNTIME
			if (!previous_mf_exists (args)) {
			    if (outfile) {
				add_string(args, change_suffix(outfile, "d"));
				add_string(args, "-MQ");
				add_string(args, outfile);
			    } else {
				add_string(args, change_suffix(drop_path(source_file), "d"));
			    }
			} else {
			    add_string(args, dependency_file);
			}
#endif
		}
		
		break;

#ifdef PATH64_ENABLE_PSCLANG
    case P_psclang_cpp:
        // psclang preprocessor

        if(option_was_seen(O_M) || option_was_seen(O_MM)) {
            add_string(args, "-Eonly");
        } else {
            add_string(args, "-E");
        }

        add_abi(args);
        
        if (ospace == TRUE ) {
            add_string(args, "-Os");
        }
        
        // language
        add_psclang_language_args(args);

        if (source_lang == L_CC) {
            add_string(args, "-D_GNU_SOURCE");
        }

#ifdef PATH64_ENABLE_PSCRUNTIME
        add_std_includes(args);
#else // !PATH64_ENABLE_PSCRUNTIME
        {
            char *exe_dir = get_executable_dir();
            char *root = directory_path(exe_dir);
            free(exe_dir);
            add_inc_path(args, "%s/lib/" PSC_FULL_VERSION "/include", root);
            free(root);
        }
#endif // !PATH64_ENABLE_PSCRUNTIME

        // input source
        add_string(args, input_source);

        // processing options for generating dependencies

        if(option_was_seen(O_M) || option_was_seen(O_MM) ||
           option_was_seen(O_MD) || option_was_seen(O_MMD)) {

            add_string(args, "-dependency-file");

            // determining dependency file name
            if(dependency_file != NULL) {
                // using user defined file name
                add_string(args, dependency_file);
            } else if(option_was_seen(O_MD) || option_was_seen(O_MMD)) {
                // using source name with .d suffix
                char *dep_file = change_suffix(drop_path(input_source), "d");
                add_string(args, dep_file);
                free(dep_file);
            } else {
                // printing dependencies to stdout
                add_string(args, "-");
            }

            if(option_was_seen(O_M) || option_was_seen(O_MD)) {
                // system headers should be added to list of dependencies
                add_string(args, "-sys-header-deps");
            }

            // setting dependency target
            if(dependency_target == NULL)
                dependency_target = change_suffix(drop_path(input_source), "o");

            add_string(args, "-MT");
            add_string(args, dependency_target);
        }

        // output
        if (last_phase == P_any_cpp && outfile != NULL) {
            // it's last phase, using outfile as output
            add_string(args, "-o");
            add_string(args, outfile);

        } else if (last_phase != P_any_cpp) {
            // constructing temporary file name for output

            current_phase = P_any_cpp;
            input_source = construct_name(input_source,"i");

            add_string(args, "-o");
            add_string(args, input_source);
        }

        break;
#endif // PATH64_ENABLE_PSCLANG

	case P_f_coco:	// bug 9058
		{
		  char *fortran_source = input_source;
		  current_phase = P_any_cpp;
		  add_string(args, "-s");
		  sprintf(buf, "-#%s", coco_setfile ? coco_setfile : "");
		  add_string(args, buf);
		  if (option_was_seen(O_E)) {
		    if (outfile == NULL)
		      add_string(args, "-E");
		    else
		      add_string(args, outfile);	// coco output file
		  } else {
		      input_source = construct_name(input_source, "i");
		      add_string(args, input_source);	// coco output file
		  }
		  add_string(args, fortran_source);	// coco input file
		}
		break;
	case P_pca:
		sprintf(buf, "-I=%s", input_source);
		add_string(args, buf);
		sprintf(buf, "-original_filename=%s", the_file);
		add_string(args, buf);

		if (keep_listing) {
		   sprintf(buf, "-lo=ocktl");
		   add_string(args, buf);
		}
		sprintf(buf, "-CMP=%s", input_source);
		add_string(args, buf);
		add_string(args, "-cp=i");
		break;
        case P_mpc:
                sprintf(buf, "%s", input_source);
                add_string(args, buf);
                if (keep_mp) {
                   input_source = construct_given_name(the_file, "P", TRUE);
                } else {
                   input_source = construct_name(input_source, "P");
                }
                sprintf(buf, "-XK%s", input_source);
                add_string(args, buf);
                break;
        case P_lister:
		if (keep_listing) {
		   char *listing_file;
		   char *cif_file;
		   char *has_path;
		   input_source = string_copy(the_file);
		   listing_file = construct_given_name(input_source,"L", TRUE);
		   cif_file = construct_given_name(drop_path(the_file), "T", keep_flag || keep_cif);
		   add_string(args, "-rs");
		   add_string(args, "-rx");
		   add_string(args, "-o");
		   add_string(args, listing_file);
		   add_string(args, input_source);
		   has_path = strrchr (input_source, '/');
		   if (has_path != NULL)
		     /* The .f file is not in the current directory */
		     add_string(args, cif_file);
                }
		break;

	case P_f90_fe:
	case P_cppf90_fe:
		// Give -ansi to FTN fe and to the cpp embedded in the FTN fe.
		// Bug 8029.
		if (ansi == STRICT_ANSI) {
		  add_string(args, "-ansi");
		}
		add_targ_options ( args ); /* Bug 5089 */
                if (index == P_cppf90_fe) {
                   if (expand_ftpp_macros) {
                      add_string(args, "-F");
                   }

                   if (use_ftpp == 1) {
                      add_string(args, "-ftpp");
                   }
                }

		replace_string ( args, "-fpic", "" );
		replace_string ( args, "-fPIC", "" );
		replace_string ( args, "-fno-pic", "" );	// bug 8463
		replace_string ( args, "-fno-PIC", "" );

		if ( ipa == TRUE || inline_t == TRUE ) {
		  add_string ( args, "-FE:full_split=off" );
		}

		if (keep_listing) {
			add_string(args, "-Ca");
		}

		sprintf(buf, "-fB,%s", construct_name(the_file,"B"));
 		add_string(args, buf); 
		sprintf(buf, "-fC,%s", construct_given_name(the_file,"T",TRUE));
 		add_string(args, buf); 
		sprintf(buf, "-fD,%s", construct_given_name(the_file,"l",TRUE));
 		add_string(args, buf); 
		/* It's much easier for the driver to generate this */
		if (option_was_seen(O_MDupdate) && !option_was_seen(O_MDtarget)) {
		   add_string(args, "-MDtarget"); 
		   add_string(args, construct_given_name(the_file,"o",TRUE));
		}
		if (keep_listing) {
			sprintf(buf, "-fl,%s",construct_given_name(the_file,"L",TRUE));
			add_string(args, buf);
		}
		do_f90_common_args(args) ;

#ifdef TARG_MIPS
        if (is_target_arch_MIPS())
            add_sysroot(args, index);  /* 15149 */
#endif
                add_isystem_dirs(args);  /* Bug 11265 */
		{
		  char *root = directory_path(get_executable_dir());
		  sprintf (buf, "-include=%s/include/" PSC_FULL_VERSION, root);
		  add_string(args, buf);
		}
#if defined(TARG_MIPS) && defined(ARCH_MIPS)
        if(is_target_arch_MIPS()) {
            sprintf (buf, "-include=/usr/include/gentoo-multilib/%s",
                 abi == ABI_N32 ? "n32" : "n64");
            add_string(args, buf);
        }
#endif

                if (Disable_open_mp) {
                   add_string(args,"-disable_open_mp");
                }

                if (Disable_old_mp) {
                   add_string(args,"-disable_old_mp");
                }
		if (dashdash_flag)
		  add_string(args,"--");
		add_string(args, input_source);

		if (option_was_seen(O_E) && outfile != NULL) {
			add_string(args, "-o");
			add_string(args, outfile);
		}

		break;

#ifdef PATH64_ENABLE_GNU_FRONTEND
	case P_spin_cc1:
	case P_spin_cc1plus:
		add_abi(args);

		if (gnu_exceptions == FALSE) {			// bug 11732
		  add_string(args, "-fno-gnu-exceptions");
		}
		if (fcxx_openmp == 1) {
		  add_string(args, "-fcxx-openmp");
		}
		else if (fcxx_openmp == 0) {
		  add_string(args, "-fno-cxx-openmp");
		}
	        // fall through
#endif // PATH64_ENABLE_GNU_FRONTEND
	case P_c_gfe:
	case P_cplus_gfe:
#ifdef TARG_X8664
		if (is_target_arch_X8664())
			add_sse_cc1_options(args, FALSE);
#endif // TARG_X8664

		if (show_but_not_run)
			add_string(args, "-###");
		add_string(args, "-dx");
		if (show_version) {
			add_string(args, "-version");
		}
		if (quiet_flag && index != P_cpp) 
			add_string(args, "-quiet");
		add_abi(args);
#if defined(TARG_MIPS)
        if (is_target_arch_MIPS()) {
            // endianness
            if (endian == ENDIAN_LITTLE)
              add_string(args, "-mel");
            else
              add_string(args, "-meb");
        }
#endif

		if (!option_was_seen(O_fpreprocessed) &&
		    !option_was_seen(O_fno_preprocessed)) {
		  add_string(args, "-fpreprocessed");
		}

		if (!option_was_seen(O_ffreestanding)) /* bug 14509 */
		{
		if( fbuiltin != 0 )
		  add_string(args, "-fbuiltin" );
		else
		  add_string(args, "-fno-builtin" );
		}

		if( fmath_errno == 0 )
		  add_string(args, "-fno-math-errno");

		if( ffast_math_prescan == 1 )
		  add_string(args, "-ffast-math");

		add_string(args, "-dumpbase");
		// Bug 2458 - path to the source file should not be dropped
		// in the command to the front-end because this is used in
		// building the DST info (DW_TAG_name in DW_TAG_compile unit)
		add_string(args, the_file);
		add_string(args, input_source);

#ifdef PATH64_ENABLE_GNU_FRONTEND
		if (index == P_spin_cc1 ||
		    index == P_spin_cc1plus) {
		  add_string(args, "-spinfile");
		  add_string(args, construct_name(the_file, "spin"));
		  break;
		}
#endif // PATH64_ENABLE_GNU_FRONTEND

		add_string(args, "-o");
		add_string(args, construct_name(the_file,"B"));
		break;

#ifdef PATH64_ENABLE_GNU_FRONTEND
	case P_wgen:
		sprintf(buf, "-fS,%s", construct_name(the_file, "spin"));
		add_string(args, buf);
		sprintf(buf, "-fB,%s", construct_name(the_file, "B"));
		add_string(args, buf);
		break;
#endif // PATH64_ENABLE_GNU_FRONTEND

#ifdef PATH64_ENABLE_PSCLANG
    case P_psclang:
        // psclang front-end

        if (show_version) {
        	add_string(args, "-version");
        }
        
        add_abi(args);
        add_psclang_language_args(args);
        
        if (!option_was_seen(O_ffreestanding) && !fbuiltin)
            add_string(args, "-fno-builtin" );
        
        if (!fmath_errno)
            add_string(args, "-fno-math-errno");

        add_string(args, "-Wno-all");
        add_string(args, "-c");
        add_string(args, "-fcolor-diagnostics");
        add_string(args, "-whirl");
        
        add_string(args, input_source);
        
        add_string(args, "-o");
        add_string(args, construct_name(the_file,"B"));
        break;
#endif // PATH64_ENABLE_PSCLANG

	case P_inline:
        add_targ_options (args);
		if (source_kind == S_B)
		    sprintf (buf, "-fB,%s", the_file);
		else
		    sprintf(buf, "-fB,%s",
			    construct_name(the_file,"B"));
		add_string (args, buf);
		sprintf (buf, "-fI,%s", construct_name(the_file,"I"));
		add_string (args, buf);
		if (dashdash_flag)
		  add_string(args,"--");
		add_string(args, the_file);
		break;
	case P_ipl:
		add_language_option ( args );
		add_targ_options ( args );	// bug 14041
		if (source_kind == S_B)
		    sprintf (buf, "-fB,%s", the_file);
		else
		    sprintf(buf, "-fB,%s",
			    construct_name(the_file,"B"));
		add_string (args, buf);

		if (instrumentation_invoked == TRUE) {
		  if (fb_file != NULL)
		    sprintf(buf, "-fi,%s.instr", fb_file);
		  else if (outfile != NULL)
		    sprintf (buf, "-fi,%s.instr", outfile);
                  else
		    sprintf (buf, "-fi,a.out.instr");
		  add_string(args,buf);
                }
		else if (opt_file != NULL) {
		  /* pass feedback file */
		  sprintf(buf, "-ff,%s.instr", opt_file);
		  add_string(args, buf);
		}

		current_phase = P_any_as;
		if (outfile != NULL && last_phase == current_phase
	 			    && !multiple_source_files
				    && !(remember_last_phase == P_any_ld &&
				         (
				          (invoked_lang==L_CC))))
			{
#ifdef FAT_WHIRL_OBJECTS
			  if(ipa == TRUE){
    			      sprintf(buf, "-fp,.ipa-%s", outfile);
			  } else 
#endif //FAT_WHIRL_OBJECTS
			      sprintf(buf, "-fp,%s", outfile);
			} else {
#ifdef KEY  // TODO : Fixme : Please look at this code again and determine if we can remove the #if KEY and the code under the #else block
#ifdef FAT_WHIRL_OBJECTS
			  if(ipa == TRUE){
			     sprintf(buf, "-fp,.ipa-%s", get_object_file(the_file));
			  } else 
#endif //FAT_WHIRL_OBJECTS
			  // Create unique .o files for a.c and foo/a.c.
			  // Bug 9097.
			  sprintf(buf, "-fp,%s", get_object_file(the_file));
#else
			  sprintf(buf, "-fp,%s", 
			    construct_given_name(the_file,"o",
				(keep_flag || multiple_source_files || ((shared == RELOCATABLE) && (ipa == TRUE))) ? TRUE : FALSE));
#endif
			}
		add_string(args, buf);
		if (dashdash_flag)
		  add_string(args,"--");
		add_string(args, the_file);

		/* ipl_cmds must be added last */
		if (ipl_cmds != 0) {
		    add_string (args, "-cmds");
		    append_string_lists (args, ipl_cmds);
		}
		break; 
#ifdef FAT_WHIRL_OBJECTS
        case P_merge:
		add_string(args, "-r");
                if (outfile != NULL && last_phase == current_phase && !multiple_source_files){
                    sprintf(buf, ".native-%s",outfile);
		    add_string(args, buf);
                    sprintf(buf, ".ipa-%s",outfile);
		    add_string(args, buf);
                    add_string(args,"-o");
                    sprintf(buf, "%s",outfile);
		    add_string(args, buf);
                } else {
                    char *o_name;
                    o_name = get_object_file(the_file);
                    sprintf(buf, ".native-%s",construct_given_name(the_file,"o",(keep_flag || multiple_source_files || ((shared == RELOCATABLE) && (ipa == TRUE))) ? TRUE : FALSE));
		    add_string(args, buf);
                    sprintf(buf, ".ipa-%s",construct_given_name(the_file,"o",(keep_flag || multiple_source_files || ((shared == RELOCATABLE) && (ipa == TRUE))) ? TRUE : FALSE));
		    add_string(args, buf);
                    add_string(args,"-o");
                    sprintf(buf, "%s",o_name);
		    add_string(args, buf);
                }
	        break; 
#endif //FAT_WHIRL_OBJECTS
	case P_be:
		add_language_option ( args );
		add_targ_options ( args );

#ifdef TARG_X8664
        if (is_target_arch_X8664()) {
		  if (msseregparm == TRUE) {
		    add_string (args,"-TENV:msseregparm=on");
		  }
		  if (mregparm) {
		    sprintf (buf, "-TENV:mregparm=%d", mregparm);
		    add_string (args, buf);
		  }
        }
#endif

		if (option_was_seen(O_ffreestanding)) /* bug 14816 */
		  add_string (args, "-OPT:freestanding=on");

		switch (source_kind) {
		case S_B:
		    if (post_fe_phase () == P_inline) {
			temp = construct_name(the_file,"I");
			break;
		    }
		    /* fall through */
		case S_I:
		case S_N:
		case S_O:
		    temp = the_file;
		    break;
		default:
		    if (post_fe_phase () == P_inline)
			temp = construct_name(the_file,"I");
		    else
			temp = construct_name(the_file,"B");
		    break;
		}
		sprintf (buf, "-fB,%s", temp);
		add_string(args, buf);

		if (instrumentation_invoked == TRUE) {
		  if (fb_file != NULL)
		    sprintf(buf, "-fi,%s.instr", fb_file);
		  else if (outfile != NULL)
		    sprintf (buf, "-fi,%s.instr", outfile);
                  else
		    sprintf (buf, "-fi,a.out.instr");
		  add_string(args,buf);
                }
		else if (opt_file != NULL) {
		  /* pass feedback file */
		  sprintf(buf, "-ff,%s.instr", opt_file);
		  add_string(args, buf);
		}

		if (internal_fb_file != NULL) {
		  /* pass feedback file */
		  sprintf(buf, "-ff,%s.instr", internal_fb_file);
		  add_string(args, buf);
		}

		/* create .s file */
		add_string(args, "-s");
		current_phase = P_be;
		if (last_phase == P_be && outfile != NULL)
			input_source = outfile;
		else
			input_source = construct_name(the_file,"s");
		sprintf(buf, "-fs,%s", input_source);
		add_string(args, buf);

		if (dashdash_flag)
		  add_string(args,"--");
		add_string(args, the_file);
		break;
#ifdef PATH64_ENABLE_PATHAS
	case P_pathas:
		add_string(args, "-g");
		add_string(args, "dwarf2pass");

		add_string(args, "-Wno-uninit-contents");
		add_string(args, "-f");
		add_string(args, (abi == ABI_M64 || abi == ABI_64) ? "elf64" : "elf32");
		add_string(args, "-p");
		add_string(args, "gas");

		add_asm_source(args, input_source);
		add_asm_output(args, the_file);

		current_phase = P_any_as;

		break;
#endif // PATH64_ENABLE_PATHAS
	case P_gas:
		if (source_lang == L_as &&
		    glevel >= 2) {
		  add_string(args, "--gdwarf2");	// bug 5990
		}
		if (dashdash_flag)
		  add_string(args,"--");
		if (show_but_not_run)
		  add_string(args, "-###");

#ifdef TARG_X8664
		if (is_target_arch_X8664()) {
		  if( abi == ABI_M32 ){
		    add_string(args, "--32");
		  } else {
		    add_string(args, "--64");
		  }
		}
#endif //TARG_X8664

#ifdef TARG_MIPS
		if (is_target_arch_MIPS()) {
		  add_string(args, abi == ABI_N32 ? "-mabi=n32" : "-mabi=64");
		  if(pic)
		    add_string(args, "-KPIC");
		}
#endif // TARG_MIPS

		add_asm_source(args, input_source);

		add_asm_output(args, the_file);

		current_phase = P_any_as;

		break;
	case P_ld:
	case P_ldplus:
		append_libraries_to_list (args);
                append_rlibraries_to_list (args);
		if (show_but_not_run)
			add_string(args, "-###");
#ifdef TARG_MIPS
        if (is_target_arch_MIPS()) {
            add_string(args, abi == ABI_N32 ? "-melf32ltsmipn32" : "-melf64ltsmip");
            add_sysroot(args, index);
        }
#endif
		add_target_linker_args(args, FALSE);

		set_library_paths(args);
		if (outfile != NULL) {
			add_string(args, "-o");
			add_string(args, outfile);
        	}
		if (ftz_crt) {
			add_string(args, find_crt_path("ftz.o"));
		}

                /* Add first part of ld args here. Start with appropriate crt*.o objects. 
                 * See the other part in add_final_ld_args() */
                if( ! option_was_seen(O_nostartfiles)){
                       if ((shared != DSO_SHARED) && (shared != RELOCATABLE)){

               const char *runtime_path = target_runtime_path();
	       char *crt1_path;
	       if (option_was_seen(O_pg) || option_was_seen(O_profile)) {
		   crt1_path = concat_strings(runtime_path, "/gcrt1.o");
	       } else {
		   crt1_path = concat_strings(runtime_path, "/crt1.o");
	       }
               char *crti_path = concat_strings(runtime_path, "/crti.o");
               char *cb_path = target_crtbegin_path();
               char *crtbegin_path = concat_strings(cb_path, "/crtbegin.o");
               free(cb_path);

			   add_string_if_new_basename(args, crt1_path);
			   add_string_if_new_basename(args, crti_path);
               free(crt1_path);
               free(crti_path);

			   add_string(args, crtbegin_path);
               free(crtbegin_path);

		       } else {
               const char *runtime_path = target_runtime_path();
               char *crti_path = concat_strings(runtime_path, "/crti.o");
               char *cb_path = target_crtbegin_path();
               char *crtbeginS_path = concat_strings(cb_path, "/crtbeginS.o");
               free(cb_path);

			   add_string_if_new_basename(args, crti_path);
			   add_string(args, crtbeginS_path);

               free(crtbeginS_path);
               free(crti_path);
		       }


                }
		break;
	case P_collect:
	case P_ipa_link:
#ifdef TARG_MIPS
        if (is_target_arch_MIPS())
          add_sysroot(args, index);
#endif
        add_target_linker_args(args, TRUE);
#ifdef TARG_X8664
        if (is_target_arch_X8664()) {
          if( abi == ABI_M32 ) {
            add_string(args, "-m");
            add_string(args,"elf_i386");
          }
        }
#elif defined(TARG_MIPS)
        if (is_target_arch_MIPS()) {
		  if( abi == ABI_N32 ) {
		    add_string(args, "-melf32ltsmipn32");
		  }
		  else {
		    add_string(args, "-melf64ltsmip");
		  }
		  // Pass top level library dir to ipa so that
		  // it finds libraries like lib32/libc.so.6
#ifndef ARCH_MIPS
		  add_library_dir (MIPS_CROSS_LIB_TOP_DIR);
#endif
        }
#endif
		/* TODO: Handle MIPS here */
		/* add lib paths for standard libraries */
		add_library_dir(target_runtime_path());
		append_libraries_to_list (args);
                append_rlibraries_to_list (args);

		set_library_paths(args);

		/* -shared only adds user objects, no predefined stuff */
		if ((shared != DSO_SHARED) && (shared != RELOCATABLE)
		    && ! option_was_seen(O_nostartfiles)) 
		{

            const char *runtime_path = target_runtime_path();
            char *cb_path = target_crtbegin_path();
            char *crt1_path = concat_strings(runtime_path, "/crt1.o");
            char *crti_path = concat_strings(runtime_path, "/crti.o");
            char *crtbegin_path = concat_strings(cb_path, "/crtbegin.o");
            free(cb_path);

            add_string_if_new_basename(args, crt1_path);
            add_string_if_new_basename(args, crti_path);
            free(crt1_path);
            free(crti_path);

			add_string(args, crtbegin_path);
			free(crtbegin_path);
			if (ftz_crt) {
				add_string(args, find_crt_path("ftz.o"));
			}
		}
                if (outfile != NULL) {
			add_string(args, "-o");
			add_string(args, outfile);
		}

		if (instrumentation_invoked == TRUE && index != P_collect) {
		  if (fb_file != NULL) 
		    sprintf(buf, "-IPA:propagate_feedback_file=%s", fb_file);
		  else if (outfile != NULL)
		    sprintf (buf, "-IPA:propagate_feedback_file=%s", outfile);
		  else
		    sprintf (buf, "-IPA:propagate_feedback_file=a.out");
		  add_string(args,buf);
		}

		if (opt_file != NULL && index != P_collect){
		  sprintf(buf, "-IPA:propagate_annotation_file=%s", opt_file);
		  add_string(args,buf);
		}
                
                if(show_flag){
                  add_string(args,"-show");
                }

		/* object file should be in list of options */
		break;
	case P_cord:
		/* specify the output file */
		if (outfile != NULL)
			{
			add_string(args, "-o");
			add_string(args, outfile);
			}
		else
			{
			add_string(args, "-o");
			add_string(args, "a.out");
			}
		/* specify the input file */
		add_string(args, create_temp_file_name("C"));
		/* now specify the feedback file(s) */
		if (feedback_files->head) {
			/* has user specified feedback files */
			append_string_lists(args, feedback_files);
			}
		else if (outfile != NULL) {
			/* feedback file is output file + .fb */
			add_string(args, concat_strings(outfile,".fb"));
			}
		else {
			/* feedback file is a.out.fb */
			add_string(args, "a.out.fb");
		}
		break;
          case P_pixie:
		/* Specify the output file */
		if (outfile != NULL) {
		   add_string(args, "-pixie_file");
		   if (fb_xdir != NULL) {
		     temp = concat_strings(fb_xdir,  "/");
		     temp = concat_strings(temp, drop_path(outfile));
		     add_string(args, temp);
                   } else {
		     add_string(args, outfile);
                   }
	           count_file_name = concat_strings(drop_path(outfile), ".x.Counts");
                } else {
		   add_string(args, "-pixie_file");
		   if (fb_xdir != NULL) {
		      temp = concat_strings(fb_xdir,  "/");
		      temp = concat_strings(temp, "a.out");
		      add_string(args, temp);
                   } else {
		      add_string(args, "a.out");
                   }
		   count_file_name = concat_strings("a.out", ".x.Counts");
                }
		if (fb_xdir != NULL) {
		   add_string(args, "-directory");
		   add_string(args, fb_xdir);
		   add_string(args, "-rpath");
		   add_string(args, fb_xdir);
                }
		if (fb_cdir != NULL) {
		   add_string(args, "-counts_file");
		   temp = concat_strings(fb_cdir, "/");
		   temp = concat_strings(temp, count_file_name);
		   add_string(args, temp);
                }
		if (ldpath_for_pixie != NULL) {
		   add_string(args, "-ldpath");
		   add_string(args, ldpath_for_pixie);
                }
                add_string(args, input_source);
		break;
         case P_prof:
		/* Generate the .fb files */
		add_string(args, "-pixie");
		add_string(args, "-feedback");
		if (prof_file != NULL) {
                  add_string(args, concat_strings(prof_file, ".x"));
                } else {
		   internal_error("No count file was specified for a prof run");
		   perror(program_name);
                }
		for (p = count_files->head; p != NULL; p = p->next) 
		   add_string(args, p->name);
                add_string(args, ">");
		add_string(args, "/dev/null");
		break;
	}
}


// Adds trailing crt*.o objects if needed. See the first part in add_file_args() */
void add_crtend(string_list_t *args) {
    char *cb_path = target_crtbegin_path();
    const char *runtime_path = target_runtime_path();
    char *crtn_path = concat_strings(runtime_path, "/crtn.o");
    const char *crtend_name = ((shared != DSO_SHARED) && (shared != RELOCATABLE)) ?
                              "/crtend.o" :
                              "/crtendS.o";
    char *crtend_path = concat_strings(cb_path, crtend_name);
    free(cb_path);

    add_string(args, crtend_path);
    add_string_if_new_basename(args, crtn_path);
}


static void
add_final_ld_args (string_list_t *args)
{
	char *temp;

    if(ipa == TRUE) {
        if(option_was_seen(O_static_libgcc)) {
            add_arg(args, "-static-libgcc");
        }

	// Must add fstart or the Fortran main will get dropped by ipa
	// processing.
	if (shared != RELOCATABLE) {
	    if (invoked_lang == L_f90) {
		if (!option_was_seen(O_shared)) {
			add_library(args, PSC_NAME_PREFIX "fstart");
		}
	    }
	}
        // don't add standard libraries and paths to standard libraries if
        // ipa is enabled because all standard libraries will be added by
        // driver
        return;
    }

#if 0 // Bug 4813 - acml_mv is not in yet.
    extern boolean link_with_mathlib;
    if ((link_with_mathlib || source_lang == L_CC) && 
        option_was_seen(O_m64) && 
        strcmp(target_cpu, "em64t") && strcmp(target_cpu, "anyx86"))
        // Bug 4680 - Link with libacml_mv by default.
        add_library(args, "acml_mv");
#endif
    if (option_was_seen(O_nodefaultlibs) || option_was_seen(O_nostdlib)) {
#ifndef PATH64_ENABLE_PSCRUNTIME
        //add default search paths even with -nostdlib
        add_arg(args, "-L%s", current_target->libgcc_path);
#endif

        // If -compat-gcc, link with pscrt even if -nostdlib.  Bug 4551.
        if (option_was_seen(O_compat_gcc) &&
        !option_was_seen(O_fno_fast_stdlib) &&
        !option_was_seen(O_nolibpscrt)) {	// bug 9611
            add_library(args, "pscrt");
        }

        if( ! option_was_seen(O_nostartfiles)) add_crtend(args);
        return;
    }
#ifdef PATH64_ENABLE_PSCRUNTIME
    if(source_lang == L_CC &&
       !option_was_seen(O_nodefaultlibs) &&
       !option_was_seen(O_nostdlib) &&
       !option_was_seen(O_nostdlib__)) {
    
        // STL library
    	add_library(args, "stl");
    
        // C++ support library
        add_library(args, "cxxrt");
    
        // other dependencies
        add_library(args, "pthread");

#ifndef  __FreeBSD__
        add_library(args, "dl");
#endif // !__FreeBSD__

    }

    // static libc should be grouped with libgcc and libeh
    // because there is loop in dependencies between these libs
    if (option_was_seen(O_static) || option_was_seen(O__static)) {
        add_arg(args, "--start-group");
    }

    if(option_was_seen(O_static_libgcc)) {
        add_arg(args, "-Bstatic");
    }

    if(option_was_seen(O_static_libgcc)) {
        add_arg(args, "-Bstatic");
    }

    add_library(args, "gcc");

    // Exception support library (used by stl and gcc)
    // staic libc also depends on libeh
    if(option_was_seen(O_static) ||
       source_lang == L_CC ||
       option_was_seen(O_fexceptions)) {
        add_library(args, "eh");
    }

    if(option_was_seen(O_static_libgcc)) {
        add_arg(args, "-Bdynamic");
    }

    add_library(args, "c");

    if (option_was_seen(O_static) || option_was_seen(O__static)) {
        add_arg(args, "--end-group");
    }
#else
    if(source_lang == L_CC) {
        add_arg(args, "-L%s", current_target->libstdcpp_path);
        add_library(args, "stdc++");
    }

    if (option_was_seen(O_static) || option_was_seen(O__static)){
        add_arg(args, "--start-group");

        add_arg(args, "-L%s", current_target->libgcc_path);
        add_library(args, "gcc");

        add_arg(args, "-L%s", current_target->libgcc_eh_path);
        add_library(args, "gcc_eh");

        add_library(args, "c");  /* the above libs should be grouped together */

        add_arg(args, "--end-group");
         
        if(invoked_lang == L_CC){
            add_arg(args, "-L%s", current_target->libsupcpp_path);
            add_library(args, "supc++");
        }
    }
#endif
	
    if( ! option_was_seen(O_nostartfiles)) add_crtend(args);

	if (shared != RELOCATABLE) {
	    if (invoked_lang == L_f90) {
		if (!option_was_seen(O_shared)) {
			add_library(args, PSC_NAME_PREFIX "fstart");
		}
		add_library(args, PSC_NAME_PREFIX "fortran");
		if (!option_was_seen(O_shared)) {
			add_library(args, PSC_NAME_PREFIX "fstart");
		}
		if (! option_was_seen(O_fbootstrap_hack)) {
		  add_string(args, "-lmv");
#ifdef TARG_MIPS
          if (is_target_arch_MIPS()) {
		    if (ffast_math_prescan == 1) {  // Bug 14245
		      // Link with libscm and open64 libmpath before libm
		      add_string(args, "-lscm");
		      add_string(args, "-lm" PSC_NAME_PREFIX);
		    }
          } else {
#endif // TARG_MIPS
		    add_string(args, "-lm" PSC_NAME_PREFIX);
#ifdef TARG_MIPS
          }
#endif // TARG_MIPS
		}
		add_string(args, "-lm");
		if (! option_was_seen(O_fbootstrap_hack)) {
		  add_library(args, "mv");
#ifdef TARG_MIPS
        if (is_target_arch_MIPS()) {
		  if (ffast_math_prescan == 1) {  // Bug 14245
		    // Link with libscm and open64 libmpath before libm
		    add_library(args, "scm");
		    add_library(args, "m" PSC_NAME_PREFIX);
		  }
        } else {
#endif // TARG_MIPS
		  add_library(args, "m" PSC_NAME_PREFIX);
#ifdef TARG_MIPS
        }
#endif // TARG_MIPS
		}
		add_library(args, "m");
	    }
	    if (option_was_seen(O_mp) ||
		option_was_seen(O_apo) ||	// bug 6334
		option_was_seen(O_fopenmp)) {
                add_string(args, "-lopenmp");
            }

            if (option_was_seen (O_fprofile_arcs))
                add_string(args, "-lgcov");    // bug 12754
  // bug 4230
	    if (option_was_seen(O_pthread) ||
		option_was_seen(O_mp) ||
		option_was_seen(O_fopenmp) ||
		option_was_seen(O_apo)) {	// bug 6334
		add_string(args, "-lpthread");
	    }
	}

    // Put pscrt & gcc after all the libraries that are built with PathScale
    // compilers, since those libraries could use PathScale routines.
    // Bug 3995.
    if (!option_was_seen(O_fno_fast_stdlib) &&
        !option_was_seen(O_nolibpscrt)) {	// bug 9611
        add_library(args, "pscrt");
    }

#if !defined(PATH64_ENABLE_PSCRUNTIME)
    if(option_was_seen(O_static) || option_was_seen(O__static)) {
    	add_arg(args, "-L%s", current_target->libgcc_path);
    	add_library(args, "gcc");
    } else {
    	add_arg(args, "-L%s", current_target->libgcc_s_path);
    	add_library(args, "gcc_s");
    }
#endif


#ifdef TARG_MIPS
    if (is_target_arch_MIPS()) {
        // Put libscstr after all libraries that are built with PathScale
        // compilers, since those libraries could use PathScale routines.
        // Bug 3995.
        if (!option_was_seen(O_fno_fast_stdlib) &&
            !option_was_seen(O_fbootstrap_hack)) {
          add_library(args, "scstr");
        }
    }
#endif 


	if (shared != RELOCATABLE) {
	  if ( fbuiltin != 0 && ! option_was_seen(O_fbootstrap_hack) ) {
	    /* Once -fbuiltin is used, some functions, i.e., __sincos, are only
	       provided by libmblah.a lib.
	    */
	    if (invoked_lang != L_cc) {
	      add_library(args, "mv");			// bug 5527
#ifdef TARG_MIPS
          if (is_target_arch_MIPS()) {
	        if (ffast_math_prescan == 1) {  // Bug 14245
              // Link with libscm and open64 libmpath before libm
              add_library(args, "scm");
              add_library(args, "m" PSC_NAME_PREFIX);	// bug 3092
	        }
          } else {
#endif // TARG_MIPS
            add_library(args, "m" PSC_NAME_PREFIX);	// bug 3092
#ifdef TARG_MIPS
          }
#endif // TARG_MIPS
	    }
	  }
	}
}


static void
postprocess_ld_args (string_list_t *args, phases_t phase)
{
    string_item_t *p;
    boolean add_huge_lib = FALSE;
    boolean do_link = FALSE;
#ifndef PATH64_ENABLE_PSCRUNTIME
    boolean gcc_for_ld = FALSE;		/* never assume we use GCC to link, some may just call linker directly, especially bare board target */
#endif // !PATH64_ENABLE_PSCRUNTIME
    char *phase_name = get_full_phase_name(phase);

    /* TODO: FIXME: Temp hack to replace gcc as a linker */

    phase_name = "/usr/bin/" CROSS_PFX "ld";

    //init_stdc_plus_plus_path();

    // adding libc to resolve references in crt* files
    if(!option_was_seen(O_nostartfiles))
        add_library(args, "c");

#ifndef PATH64_ENABLE_PSCRUNTIME
    if (strcmp(phase_name + strlen(phase_name) - 3, "gcc") == 0 ||
    		strcmp(phase_name + strlen(phase_name) - 3, "g++") == 0) {
    	gcc_for_ld = TRUE;

    } else {
    	if (strstr(phase_name, "gcc") != NULL ||
    			strstr(phase_name, "g++") != NULL) {
    		internal_error("driver have gcc\?\?/g++\?\? as linker status, please add code to handle gcc_for_ld");
    	}
    }
#endif // !PATH64_ENABLE_PSCRUNTIME

    if (option_was_seen(O_pg) && !option_was_seen(O_nostdlib)) {
	if (prof_lib_exists("c"))
	    add_library(args, "c");
    }

    /*
     * For some reason, our cross linker won't find libraries in some
     * directories unless it's told about them with -rpath-link.
     * Here, we scan all -L flags and pass them as -rpath-link flags,
     * too.
     */

#if !defined(BUILD_OS_DARWIN) /* -rpath-link not supported on Mac */
    for (p = args->head; p != NULL; p = p->next) {
	char *dir = NULL;

	//we intercept -Wl,<option> and pass to ld <option>
	//here we deal with the -Wl passed explicitly only
	if (phase != P_ipa_link && !strncmp(p->name, "-Wl,", 4)) {
	    // cut away -Wl, prefix and split the rest in pieces at each ","
	    // advance p to avoid iterating over the newly added strings
	    // 
	    // pathcc -v f1.o -Wl,-whole-archive somelib.a -Wl,-no-whole-archive,-arg1,-arg2 f2.o -o somelib.so -Wl,-soname,thelib -Wl, f3.o
	    // This produces (as far as the -Wl, args are concerned)
	    // ld ... f1.o -whole-archive somelib.a -no-whole-archive -arg1 -arg2 f2.o -soname thelib f3.o
	    // Note that the empty -Wl, is removed (turned into an empty string).
	    int i, prev;
	    char *s;
	    string_item_t *q;
	    p->name[3] = '\0';
	    p->name+=3;
	    q = p;
	    prev = 1;
	    s = p->name;
	    for (i = 1; s[i]; i++) {
		if (s[i] == ',') {
		    s[i] = '\0';
		    add_after_string(args, q, s + prev);
		    p = q;
		    q = q->next;
		    prev = i+1;
		}
	    }
	    add_after_string(args, q, s + prev);
	    p = q->next;

	    continue;
	}

	if (strncmp(p->name, "-L", 2))
	    continue;

	
	if (strlen(p->name) > 2) {
	    dir = p->name + 2;
	}
	else if (p->next) {
	    dir = p->next->name;
	}
	if (dir) {
	    char * root_prefix = directory_path(get_executable_dir());

            if (option_was_seen(O_HP) && (strstr(dir, root_prefix) != NULL)
                && (instrumentation_invoked != TRUE)) {
                HUGEPAGE_DESC desc;

#ifdef PATH64_ENABLE_PSCRUNTIME
                add_after_string(args, p, concat_strings("-rpath=", dir));
#else // !PATH64_ENABLE_PSCRUNTIME
                add_after_string(args, p, concat_strings(gcc_for_ld ? "-Wl,-rpath," : "-rpath=", dir));
#endif // !PATH64_ENABLE_PSCRUNTIME

                for (desc = hugepage_desc; desc != NULL; desc = desc->next) {
                    if (desc->alloc == ALLOC_BDT && !do_link) {
                        /* libhugetlbfs linker script only supports dynamic link. 
                         */
                        if (!option_was_seen(O_static)) {
                            if (desc->size == SIZE_2M)
                                dir = concat_strings(dir, "/elf.xBDT");
                            else if (desc->size == SIZE_1G)
                                dir = concat_strings(dir, "/elf_1G.xBDT");
                                
#ifdef PATH64_ENABLE_PSCRUNTIME
                            add_after_string(args, p, concat_strings("-T", dir));
#else // !PATH64_ENABLE_PSCRUNTIME
                            add_after_string(args, p, concat_strings(gcc_for_ld ? "-Wl,-T" : "-T", dir));
#endif // !PATH64_ENABLE_PSCRUNTIME
                            do_link = TRUE;
                            add_huge_lib = TRUE;
                        }
                    }
                    else if (desc->alloc == ALLOC_HEAP)
                        add_huge_lib = TRUE;
                }

                if (add_huge_lib && option_was_seen(O_static))
#ifdef PATH64_ENABLE_PSCRUNTIME
                    add_after_string(args, p, "--undefined=setup_libhugetlbfs");
#else // !PATH64_ENABLE_PSCRUNTIME
                    add_after_string(args, p, gcc_for_ld ? "-Wl,--undefined=setup_libhugetlbfs" : "--undefined=setup_libhugetlbfs");
#endif // !PATH64_ENABLE_PSCRUNTIME
            }
	}
    }

    if (add_huge_lib) {
        add_library(args, "hugetlbfs-psc");
    }
#endif /* defined(BUILD_OS_DARWIN) */
}

#define MAX_PHASE_ORDER 10
static phases_t phase_order[MAX_PHASE_ORDER];
static int phase_order_index = 0;

static void
add_phase (phases_t p)
{
	phase_order[phase_order_index] = p;
	phase_order_index++;

	// P_NONE is used to mark the end of the phase list.  Set P_NONE here
	// instead of relying on the caller to set it.
	phase_order[phase_order_index] = P_NONE;

	if (phase_order_index >= MAX_PHASE_ORDER)
		internal_error("too many phases");
}

/* determine which of inline, ipl, or be should be executed after the front
   end */

/* Regarding P_inline, change the semantics of post_fe_phase to mean
   (bug 11325):
    1)  If post_fe_phase is called before the front-end executes, then it
	determines if the inline *could* be run.
    2)  If post_fe_phase is called after the front-end executes, then it
	determines if the inliner will actually run (or already did run).
*/
static phases_t
post_fe_phase (void)
{
    if (ipa == TRUE)
      return P_ipl;
    // run_inline is the final gating variable that controls whether or not to
    // run the inliner.  It is UNDEFINED before the front-end is run.
    // Afterward, it is set to inline_t if the inline setting is explicitly
    // given on the command line; otherwise it is set to the inline request
    // from the front-end.  Bug 11325
    else if (run_inline != UNDEFINED)
      return run_inline == TRUE ? P_inline : P_be;
    else if (inline_t == TRUE || inline_t == UNDEFINED)
	return P_inline;
    else
	return P_be;
} /* post_fe_phase */
    
/* If -INLINE:%s option was seen, pass it to ld if ipa run, or inline if
   stand-alone inliner run */

static void
add_inline_option(void)
{
  if (option_was_seen(O_INLINE_) || ( inline_t!= FALSE)) {
    switch (post_fe_phase()) {
    case P_ipl:
      add_phase_for_option(O_INLINE_, P_ipa_link);
      break;
    case P_inline:
      add_phase_for_option(O_INLINE_, P_inline);
      break;
    }
  }
}


#ifdef PATH64_ENABLE_PSCLANG
// Returns true if psclang should be used as preprocessor/front-end
int is_psclang_enabled() {
#ifdef PATH64_ENABLE_DEFAULT_PSCLANG
    return !option_was_seen(O_fno_psclang);
#else // !PATH64_ENABLE_DEFAULT_PSCLANG
    return option_was_seen(O_fpsclang);
#endif // !PATH64_ENABLE_DEFAULT_PSCLANG
}
#endif // PATH64_ENABLE_PSCLANG


static void
determine_phase_order (void)
{
	phases_t next_phase = P_NONE;
	phases_t cpp_phase = P_NONE;
	phases_t asm_phase;
	phases_t link_phase;
	phase_order[0] = P_NONE;
	phase_order_index = 0;
 
	/* determine which cpp to use */
#ifdef PATH64_ENABLE_PSCLANG
    if ((source_lang == L_cc || source_lang == L_CC) && is_psclang_enabled()) {
        cpp_phase = P_psclang_cpp;
    } else
#endif // PATH64_ENABLE_PSCLANG
#ifdef PATH64_ENABLE_GNU_FRONTEND
	if (source_lang == L_CC) {
		cpp_phase = P_gcpp_plus;
	} else if (source_lang == L_cc) {
		// bug 5495
		if (option_was_seen(O_mp)) {
			/* power C */
                        cpp_phase = P_cpp;
		}
		else
		cpp_phase = P_gcpp;
	} else
#endif // PATH64_ENABLE_GNU_FRONTEND
        if (use_coco == TRUE) {		// bug 9058
		cpp_phase = P_f_coco;
	} else if (source_lang == L_f90) {
// bug 5946
	   if (option_was_seen(O_ftpp)) {
	      cpp_phase = P_cppf90_fe;
	   }
       else if (option_was_seen(O_cpp)
		|| option_was_seen(O_P) 
		|| option_was_seen(O_E)
		|| (!option_was_seen(O_nocpp) &&
		    (source_kind == S_F || source_kind == S_F90)))
	   {
#ifdef PATH64_ENABLE_GNU_FRONTEND
	      cpp_phase = P_gcpp; 
#else // !PATH64_ENABLE_GNU_FROENTEND
              cpp_phase = P_cpp;
#endif // !PATH64_ENABLE_GNU_FRONTEND
	   }
       else {
	      cpp_phase = P_NONE;
	   }
	} else {
#ifdef PATH64_ENABLE_GNU_FRONTEND
        cpp_phase = P_gcpp;
#elif defined(PATH64_ENABLE_PSCLANG)
        if (source_lang == L_as) {
            cpp_phase = P_psclang_cpp;
        } else {
            cpp_phase = P_cpp;
        }
#else
        cpp_phase = P_cpp;
#endif // PATH64_ENABLE_GNU_FRONTEND
	}

	if (last_phase == P_any_cpp
	    || source_kind == S_h	// SiCortex 5034
	   ) {
		add_phase(cpp_phase);
		return;
	}

	/* determine which asm to run */
#ifdef PATH64_ENABLE_PATHAS
#ifdef PATH64_ENABLE_DEFAULT_PATHAS
	asm_phase = option_was_seen(O_fno_pathas) ? P_gas : P_pathas;
#else // !PATH64_ENABLE_DEFAULT_PATHAS
	asm_phase = option_was_seen(O_fpathas) ? P_pathas : P_gas;
#endif // !PATH64_ENABLE_DEFAULT_PATHAS
#else
	asm_phase = P_gas;
#endif // !PATH64_ENABLE_PATHAS

	/* determine which linker to run */
	if (ipa == TRUE)
		link_phase = P_ipa_link;
	else if (invoked_lang == L_CC)
		link_phase = P_ldplus;
	else
		link_phase = P_ld;

	phases_t c_fe = P_NONE;
	phases_t cplus_fe = P_NONE;

#ifdef PATH64_ENABLE_GNU_FRONTEND
	c_fe = P_spin_cc1;
	cplus_fe = P_spin_cc1plus;
#endif // PATH64_ENABLE_GNU_FRONTEND

	switch (source_kind) {
	case S_c:
	case S_C:
		if (first_phase != P_any_cpp) {
		    next_phase = (source_lang == L_CC ? cplus_fe : c_fe);
		} else {
            add_phase(cpp_phase);

#ifdef PATH64_ENABLE_PSCLANG
            if (is_psclang_enabled()) {
                next_phase = P_psclang;
            }
            else
#endif // PATH64_ENABLE_PSCLANG
		    next_phase = (source_lang == L_CC ? cplus_fe : c_fe);
		}
		break;
	case S_i:
	case S_ii:
		if (source_lang == L_f90)
			next_phase = P_f90_fe;
		else if (source_lang == L_as)
			next_phase = asm_phase;
		else if (source_lang == L_CC)
			next_phase = cplus_fe;
		else if (source_lang == L_cc)
			next_phase = c_fe;
		else if (source_kind == S_ii)
			next_phase = cplus_fe;
		else
			next_phase = c_fe;
		break;
        case S_f:
        case S_f90:
		if (cpp_phase == P_NONE) {
			next_phase = P_f90_fe;
		} else if (cpp_phase == P_cppf90_fe) {
			/* invoke combined cpp/fe phase */
			next_phase = P_cppf90_fe;
		}
		else {
			add_phase(cpp_phase);
			next_phase = P_f90_fe;
		}
		break;
	case S_F:
	case S_F90:
		if ( option_was_seen(O_nocpp)) {
			next_phase = P_f90_fe;
		} else if (cpp_phase == P_NONE || cpp_phase == P_cppf90_fe) {
			/* invoke combined cpp/fe phase */
			next_phase = P_cppf90_fe;
		} else {
			add_phase(cpp_phase);
			next_phase = P_f90_fe;
		}
		break;
	case S_s:
		if (option_was_seen(O_cpp)) {
			add_phase(cpp_phase);
		}
		next_phase = asm_phase;
		break;
	case S_S:
		if ( ! option_was_seen(O_nocpp)) {
			add_phase(cpp_phase);
		}
		next_phase = asm_phase;
		break;
	case S_B:
		next_phase = post_fe_phase ();
		break;
	case S_I:
	case S_N:
	case S_O:
		next_phase = P_be;
		break;
	case S_o:
		next_phase = link_phase;
		break;
	}

	while (next_phase != P_NONE) {
		if (last_phase < next_phase) {
			add_phase(P_NONE);
			next_phase = P_NONE;
		}
		switch (next_phase) {
		case P_lister:
		case P_c_gfe:
		case P_cplus_gfe:
			add_phase(next_phase);
			next_phase = post_fe_phase ();
			break;

#ifdef PATH64_ENABLE_GNU_FRONTEND
		case P_spin_cc1:
		case P_spin_cc1plus:
			add_phase(next_phase);
			next_phase = P_wgen;
			break;
#endif // PATH64_ENABLE_GNU_FRONTEND

#ifdef PATH64_ENABLE_PSCLANG
        case P_psclang:
			add_phase(next_phase);
			next_phase = post_fe_phase ();
			break;
#endif // PATH64_ENABLE_PSCLANG

#ifdef PATH64_ENABLE_GNU_FRONTEND
		case P_wgen:
			add_phase(next_phase);
			next_phase = post_fe_phase ();
			break;
#endif // PATH64_ENABLE_GNU_FRONTEND

		case P_f90_fe:
                case P_cppf90_fe:
			if (keep_listing) {
			   add_phase(next_phase);
			   next_phase = P_lister;
                        } else {
			   add_phase (next_phase);
			   next_phase = post_fe_phase ();
                        }
			break;
		case P_inline:
			add_phase (next_phase);
			next_phase = P_be;
			break;
		case P_ipl:
			add_phase(next_phase);
#ifdef FAT_WHIRL_OBJECTS
			next_phase = P_be;
			break;
#endif
			if (option_was_seen(O_ar)) {
			    next_phase = P_ar;
			}
			else {
			    next_phase = link_phase;
			}
			break;
#ifdef FAT_WHIRL_OBJECTS
		case P_merge:
			add_phase(next_phase);

			if (option_was_seen(O_ar)) {
			    next_phase = P_ar;
			}
			else {
			    next_phase = link_phase;
			}
			break;
#endif
		case P_be:
			add_phase(next_phase);
			next_phase = asm_phase;
			break;
#ifdef PATH64_ENABLE_PATHAS
		case P_pathas:
#endif // PATH64_ENABLE_PATHAS
		case P_gas:
			add_phase(next_phase);
#ifdef FAT_WHIRL_OBJECTS
                        /* After be produces assembler and it is copiled into .o
                         * we're going to call P_ipl and P_merge */
			if(ipa == TRUE){
			   next_phase = P_merge;
			   break;
			}
#endif
			if (option_was_seen(O_ar)) {
			    next_phase = P_ar;
			}
			else {
			    next_phase = link_phase;
			}
			break;
		case P_ld:
		case P_ldplus:
		case P_collect:
		case P_ipa_link:
			add_phase(next_phase);
                        if (cordflag==TRUE) {
			   next_phase = P_cord;
			} else {
			  add_phase(P_NONE);
			  next_phase = P_NONE;
			}
			break;
		case P_cord:
			add_phase(next_phase);
			add_phase(P_NONE);
			next_phase = P_NONE;
			break;
		case P_NONE:
			break;
		default:
			internal_error("unexpected phase order");
		}
	}
}

static void
check_existence_of_phases (void)
{
    int i;
    for (i = 0; phase_order[i] != P_NONE; i++) {
	int give_warning = FALSE;
	switch (phase_order[i]) {

	case P_mpc:
	    /* pca also invokes mpc, so just check mpc */
	    if (!file_exists(get_full_phase_name(phase_order[i]))) {
		error("Power C is not installed on this system");
	    }
	    break;

			
	    /* check if be phase exists, to warn about wrong toolroot */
	case P_ipl:
	    if (!file_exists (concat_strings (get_phase_dir(phase_order[i]),
					      "/" FN_DSO("ipl"))))
		give_warning = TRUE;

	    /* fall through */
	    
	case P_be:

	    if (!file_exists (concat_strings (get_phase_dir(phase_order[i]),
					      "/" FN_DSO("be"))))
		give_warning = TRUE;

	    if (!file_exists(get_full_phase_name(phase_order[i])))
		give_warning = TRUE;

	    if (give_warning)
		warning ("%s does not contain all of the PathScale compiler"
			 " phases.", get_phase_dir(phase_order[i])); 
	    break;
	}
    }
}

static void
add_instr_archive (string_list_t* args)
{
  extern int profile_type;

  /* Add instrumentation archives */
  if (instrumentation_invoked != UNDEFINED && instrumentation_invoked) {

    unsigned long f = WHIRL_PROFILE | CG_EDGE_PROFILE | CG_VALUE_PROFILE |
      CG_STRIDE_PROFILE ;
    if (!(profile_type & ~f)) {
      if (profile_type & (CG_EDGE_PROFILE |
			  CG_VALUE_PROFILE | CG_STRIDE_PROFILE)) {
	add_library (args,"cginstr");
      }

      add_library (args, "instr2");
#ifndef PATH64_ENABLE_PSCRUNTIME
      if (!option_was_seen(O_static) && !option_was_seen(O__static)) {
        add_arg(args, "-L%s", current_target->libgcc_s_path);
	add_library(args, "gcc_s");
      }
#endif // PATH64_ENABLE_PSCRUNTIME
    } else {
      fprintf (stderr, "Unknown profile types %#lx\n", profile_type & ~f);
    }
  }
}


// Determine the prefix for the gcc name according to how pathcc is invoked.
// If pathcc is invoked as mips64el-key-linux-pathcc, then gcc willl be
// mips64el-key-linux-gcc.
static char *
get_gnu_prefix (void)
{
  char *prefix, *cmd, *x;
  char path[MAXPATHLEN];
  int i, j, len, rval;

  /* Look in this special place for a link to the executable.  This only
     works on Linux, but it should work since pathcc runs only on Linux. */
  rval = readlink ("/proc/self/exe", path, sizeof(path));
  if (rval <= 0) {
    strncpy(path, orig_program_name, sizeof(path));
    rval = strlen(path);
  } else {
    path[rval] = '\0';		// readlink doesn't append NULL
  }

  // Extract command from command path.  If command path is
  // /foo/mips64el-key-linux-pathcc, then command is mips64el-key-linux-pathcc.
  for (i = strlen(path)-1; i >= 0; i--) {
    if (path[i] == '/')
      break;
  }
  if (i < 0) {
    // '/' not found.  path is the command itself.
    cmd = path;
  } else {
    // Found '/'.  Command begins at the first char after '/'.
    cmd = path + i + 1;
  }

  // Get the prefix of the command.  Examples:
  //   command:  mips64el-key-linux-pathcc
  //   prefix:   mips64el-key-linux-
  //
  //   command:  mips64el-key-linux-pathcc-
  //   prefix:   mips64el-key-linux-
  prefix = strdup(cmd);
  if ((x = strstr(prefix, "-" PSC_FULL_VERSION))) {
    *x = '\0';
  }
  // Skip all trailing '-', if any.
  for (i = strlen(prefix)-1; i >= 0 && prefix[i] == '-'; i--);
  // Skip the last group of non-'-' chars.
  for (j = i; j >= 0; j--) {
    if (prefix[j] == '-') {
      prefix[j+1] = '\0';
      break;
    }
  }
  if (j < 0) prefix = NULL;  // no prefix

  return prefix;
}


// Search the given list of paths for a file with the given name.
// If not found, return NULL.  If found, return the path where the
// file is found.
static char *
find_file_in_path(const char *path, const char *name)
{
  char *tmp = NULL;
  int i, j = 0;
  if (path && name) {
    tmp = (char *) alloca(strlen(path) + strlen(name) + 1);
    for (i = 0; path[i] != '\0'; ++i) {
      if (path[i] != ':') {
	tmp[j++] = path[i];
      } else {
	tmp[j] = '/';
	strcpy(tmp + j + 1, name);
	if (file_exists(tmp)) {
	  tmp[j] = '\0';
	  return string_copy(tmp);
	} else {
	  j = 0;
	}
      }
    }
    tmp[j] = '/';
    strcpy(tmp + j + 1, name);
    if (file_exists(tmp)) {
      tmp[j] = '\0';
      return string_copy(tmp);
    }
  }
  return NULL;
}


// Find gcc for preprocesser, assembler, linker
void
init_gnu_phase_info (void)
{
  // Extract the prefix for the gcc name according to how pathcc is invoked:
  //   mips64el-key-linux-pathcc  -->  mips64el-key-linux-gcc.
  char *prefix = get_gnu_prefix();

//#ifdef TARG_MIPS
//  if (is_target_arch_MIPS()) {
//    const char  name_short[] = "gcc";
//    const char *name_long = prefix ? concat_strings(prefix, name_short) : NULL;
//    char *directory = NULL;
//    const char *gcc_path = getenv("PSC_GCC_PATH");
//    if (name_long) directory = find_file_in_path(gcc_path, name_long);
//    if (directory == NULL) {
//      directory = find_file_in_path(gcc_path, name_short);
//      if (directory) prefix = NULL;
//#if 1  // 14839: Check PSC_GCC_PATH, but not PATH for gcc.
//      else {
//        gcc_path = getenv("PATH");
//        if (name_long) directory = find_file_in_path(gcc_path, name_long);
//        if (directory == NULL) {
//      directory = find_file_in_path(gcc_path, name_short);
//      if (directory) prefix = NULL;
//        }
//      }
//#endif
//    }
//    // TODO: Conflict with TOOLROOT code below
//
//    if (directory) set_phase_dir(GCC_MASK, directory);
//  }
//#endif

  if (prefix) prefix_all_phase_names(GCC_MASK, prefix);
}


extern char *get_binutils_lib_path(void);

void
init_phase_info (void)
{
	char *toolroot;
	char *comp_target_root;

	init_gnu_phase_info();

	if (getenv("_XPG") != NULL) 
	   xpg_flag = TRUE;
	ld_library_path = getenv("LD_LIBRARY_PATH");
	ld_libraryn32_path = getenv("LD_LIBRARYN32_PATH");
	old_ld_library_path = string_copy(ld_library_path);
	// Replace ":" with ";" because ":" has special meaning to -INTERNAL.
	if (old_ld_library_path) {
	  int i;
	  for (i=0; i<strlen(old_ld_library_path); i++) {
	    if (old_ld_library_path[i] == ':')
	      old_ld_library_path[i] = ';';
	  }
	  asprintf(&old_ld_library_path, "%s;%s", old_ld_library_path,
		   get_binutils_lib_path());
	} else {
	  old_ld_library_path = get_binutils_lib_path();
	}

	toolroot = getenv("TOOLROOT");
	if (toolroot != NULL) {
		/* add toolroot as prefix to phase dirs */
                prefix_all_phase_dirs(PHASE_MASK, toolroot);
	}
	comp_target_root = getenv("COMP_TARGET_ROOT");
	if (comp_target_root != NULL) {
		/* add comp_target_root as prefix to phase dirs */
                prefix_all_phase_dirs(LIB_MASK, comp_target_root);
	}
}


// Change the front-end names to reflect the GNU version.
void init_frontend_phase_names ()
{
#ifdef PATH64_ENABLE_GNU_FRONTEND
    set_phase_name(P_spin_cc1, "cc142");
    set_phase_name(P_spin_cc1plus, "cc1plus42");
    set_phase_name(P_wgen, "wgen42");
#endif // PATH64_ENABLE_GNU_FRONTEND
}

void
run_dsm_prelink(void)
{
	string_list_t *args = init_string_list();
	copy_phase_options(args, P_dsm_prelink);
	append_objects_to_list(args);
 	run_phase(P_dsm_prelink, get_full_phase_name(P_dsm_prelink), args);
}


void
run_ld (void)
{
	phases_t ldphase;
	char *ldpath;
	string_list_t *args = init_string_list();

	if (ipa == TRUE) {
		ldphase = P_ipa_link;
	}
	else if (invoked_lang == L_CC) {
		ldphase = P_ldplus;
	}
	else {
		ldphase = P_ld;
	}

#ifdef TARG_X8664
    if (is_target_arch_X8664()) {
        //Supply ld with additional arguments required on x8664
        // TODO: Make sure ld is called correctly for -m32
        if ( ldphase != P_ipa_link ) {
                add_multi_strings(args, AUXILIARY_LD_FLAGS, FALSE);
        }
    }
#endif

	if (ipa == TRUE) {
	    ldpath = get_phase_dir (ldphase);
	    ldpath = concat_strings (ldpath, "/" FN_DSO("ipa"));
	    if (!file_exists (ldpath)) {
		error (FN_DSO("ipa") " is not installed on %s", 
			   get_phase_dir (ldphase));
		return;
	    }
	    // Tell ipa_link about the LD_LIBRARY_PATH that was in effect
	    // before the compiler was run.
	    char *str = "-INTERNAL:old_ld_lib_path=";
	    if (old_ld_library_path)
	      str = concat_strings (str, old_ld_library_path);
	    add_string(args, str);

	    // Tell ipa_link which pathcc to invoke.  Bug 14371.
	    str = concat_strings ("-INTERNAL:cc_name=",
				  file_utils_get_program_name());
	    add_string(args, str);

	    // Tell ipa_link about the source language.
	    switch (invoked_lang) {
	      case L_f90:	str = "F90";	break;
	      case L_cc:	str = "C";	break;
	      case L_CC:	str = "CC";	break;
	      default:		internal_error("run_ld: unknown language\n");
	    }
	    add_string(args, concat_strings("-INTERNAL:lang=", str));

	    // 14839: Tell ipa_link where to find the final gcc linker
	    phases_t ldphase_final = (invoked_lang == L_CC ? P_ldplus : P_ld);
	    char *linker = get_full_phase_name(ldphase_final);
	    add_string(args, concat_strings("-INTERNAL:linker=", linker));

	    // bug 14359: maintain source PU order under -g -ipa
	    if (glevel > 1) {
	      add_string(args, "-IPA:source_pu_order=on");
	    }

//	    if (invoked_lang == L_CC ||
//		instrumentation_invoked == TRUE ) {
//	      init_stdc_plus_plus_path();
//	    }
	}
    ldpath = get_full_phase_name(ldphase);

    if ( ldphase != P_ipa_link )
        ldpath = "/usr/bin/" CROSS_PFX "ld";

	/* for ld, we first have options, then files, then objects,
	 * where objects includes object-specific options */
	add_inline_option ();
	copy_phase_options (args, ldphase);

	if (invoked_lang == L_CC) {
	    if (!multiple_source_files && !((shared == RELOCATABLE) && (ipa == TRUE) && (outfile == NULL)) && !keep_flag)
		mark_saved_object_for_cleanup();
	}
	add_file_args (args, ldphase);

	if (shared == RELOCATABLE && source_file != NULL)
	    add_string(args, construct_given_name(
			       fix_name_by_lang(source_file),
			       "o",
			       outfile == NULL ? TRUE : keep_flag));
  	else
	    append_objects_to_list (args);


#if defined(TARG_MIPS)
    if (is_target_arch_MIPS())
        add_string(args, "-mips64");	// call gcc with -mips64
#endif

	add_instr_archive (args);

	add_final_ld_args (args);
	postprocess_ld_args (args, ldphase);

	run_phase (ldphase, ldpath, args);
}

void
run_ar(void)
{
     string_list_t *args = init_string_list();
     char *arpath;
     arpath = get_full_phase_name(P_ar);

     add_string (args, "-rc");
     copy_phase_options (args, P_ar);
     add_string (args, outfile);
     append_ar_objects_to_list(args);
     run_phase (P_ar, arpath, args);
}



void
run_pixie (void)
{
#ifdef _WIN32
    error("run_pixie is unimplemented\n");
#else
 int link_status;
 string_list_t *args = init_string_list();
 char *pixie_file;
 char *temp;

 copy_phase_options (args, P_pixie);
 add_file_args (args, P_pixie);
 run_phase (P_pixie, get_full_phase_name(P_pixie), args);
 if (outfile != NULL) 
   temp = string_copy(outfile);
 else
   temp = string_copy("a.out");
 if (fb_xdir != NULL) {
   pixie_file = concat_strings(fb_xdir,  "/");
   pixie_file = concat_strings(pixie_file, drop_path(temp));
   link_status = symlink(pixie_file, temp);
   if (link_status != 0) {
      if (errno == EEXIST) {
          warning("%s already exists; link from %s to %s not created",
                            temp, temp, pixie_file);
      } else {
          internal_error("cannot create link from %s to %s",
                                 temp, pixie_file );
          perror(program_name);
      }
   }
 }
#endif
}

void
run_prof (void)
{
#ifdef _WIN32
    error("run_prof is unimplemented\n");
#else
 int link_status;
 string_list_t *args = init_string_list();
 char *bin_dot_pixie, *bin_plain;
 boolean delete_link = TRUE;

 if (prof_file != NULL) {
    bin_plain = string_copy (prof_file);
    bin_dot_pixie = concat_strings(prof_file, ".x.pixie");
 } else {
    internal_error("No count file was specified for a prof run");
    perror(program_name);
 }
 link_status = link(bin_plain, bin_dot_pixie);
 if (link_status != 0) {
    delete_link = FALSE;
    if (errno == EEXIST) {
        warning("Link from %s to %s already exists",
                          bin_dot_pixie, bin_plain );
    } else {
        internal_error("cannot create link from %s to %s",
                               bin_dot_pixie, bin_plain );
        perror(program_name);
    }
 }

 copy_phase_options (args, P_prof);
 add_file_args (args, P_prof);
 run_phase (P_prof, get_full_phase_name(P_prof), args);
  
 if (delete_link) {
    int unlink_status;
    unlink_status = unlink(bin_dot_pixie);
    if (unlink_status != 0 && errno != ENOENT) {
      internal_error("cannot unlink %s", bin_dot_pixie );
      perror(program_name);
    }
 }
#endif
}

void
run_compiler (int argc, char *argv[])
{
	int i;
	string_list_t *args;
	boolean inst_info_updated = FALSE;
	boolean cmd_line_updated = FALSE;
        buffer_t rii_file_name;
        buffer_t ii_file_name;

	clear_current_errors();
	determine_phase_order();
	add_inline_option();
	if (execute_flag) {
		check_existence_of_phases();
	}
	input_source = source_file;

#ifdef PSCSUB
        obtain_license (get_executable_dir(), argc, argv) ;
#endif

	// Set stack size to the hard limit.  Bug 3212.
	set_stack_size();

	for (i = 0; phase_order[i] != P_NONE; i++) {
	        /* special case where the frontend decided that
		   inliner should not be run */
	        if (
		    run_inline == FALSE &&	// bug 11325
		    phase_order[i] == P_inline)
		    continue;

		if (is_matching_phase(get_phase_mask(phase_order[i]), P_any_ld)) {
			source_kind = S_o;
			/* reset source-lang to be invoked-lang for linking */
			source_lang = get_source_lang(source_kind);
			run_ld ();
			if (Gen_feedback)
			   run_pixie ();
		} else {
			args = init_string_list();
			add_file_args_first (args, phase_order[i]);  // bug 6874
			if (phase_order[i] == P_inline &&
			    run_inline == TRUE &&
			    olevel == 0 &&
			    !(option_was_seen(O_INLINE_) || 
			    option_was_seen(O_INLINE) || 
			    option_was_seen(O_inline) || 
			    option_was_seen(O_finline) || 
			    option_was_seen(O_finline_functions))) {
				prepend_option_seen (add_string_option(O_INLINE_, "none"));
			}
			copy_phase_options (args, phase_order[i]);
                        
			if (!cmd_line_updated &&
			    phase_order[i] > P_any_optfe &&
			    phase_order[i] != P_c_gfe &&
			    phase_order[i] != P_cplus_gfe &&
#ifdef PATH64_ENABLE_GNU_FRONTEND
			    phase_order[i] != P_spin_cc1 &&
			    phase_order[i] != P_spin_cc1plus &&
			    phase_order[i] != P_wgen &&
#endif // PATH64_ENABLE_GNU_FRONTEND
#ifdef PATH64_ENABLE_PSCLANG
                phase_order[i] != P_psclang &&
#endif // PATH64_ENABLE_PSCLANG
			    phase_order[i] < P_any_fe) 
			{
			    add_command_line_arg(args, source_file);
			    cmd_line_updated = TRUE;
			}
			add_file_args (args, phase_order[i]);
			run_phase (phase_order[i],
				   get_full_phase_name(phase_order[i]), args);

#ifdef PATH64_ENABLE_GNU_FRONTEND
			if (phase_order[i] == P_gcpp_plus)
			  unsetenv("DEPENDENCIES_OUTPUT");  /* bug 13154 */
#endif // PATH64_ENABLE_GNU_FRONTEND

			if ( i == 0 && (string_md == TRUE || string_mmd == TRUE)){
			        /* Bug# 581, bug #932, bug# 1049, bug #433 */
				/* We've run the dependency phase, so
				 * let's clean all those switches out. */
				string_item_t * p;
				for (p = args->head; p != NULL; p = p->next) {
					if (strcmp (p->name, "-MF") == 0) {
						p->name = string_copy("");
						p->next->name = string_copy("");
					}
					if (strcmp (p->name, "-MT") == 0) {
						p->name = string_copy("");
						p->next->name = string_copy("");
					}
					if (strcmp (p->name, "-MQ") == 0) {
						p->name = string_copy("");
						p->next->name = string_copy("");
					}
				}
				replace_string (args, "-M", "");
				replace_string (args, "-MM", "");
				replace_string (args, "-MG", "");
				replace_string (args, "-MP", "");
			 	run_phase (phase_order[i],
					   get_full_phase_name(phase_order[i]), args);
				string_md = FALSE;
				string_mmd = FALSE;
			}
			if (!inst_info_updated &&
			    phase_order[i] > P_any_optfe &&
			    phase_order[i] < P_any_fe) {
			    /*
			     * Update the instantiation information file
			     * with the new command line, if applicable.
			     */
			    if (prelink_flag && source_lang == L_CC) {
		  		if (outfile != NULL && !multiple_source_files
				      	  && remember_last_phase != P_any_ld) 
				    sprintf(ii_file_name, "%s",
					    make_ii_file_name(outfile));
		  		else
				    sprintf(ii_file_name,
				   	    "./ii_files/%s",
					    construct_file_with_extension(
					        source_file, "ii"));
			        update_instantiation_info_file(
				        ii_file_name, source_file);

                            }
			    inst_info_updated = TRUE;
			}
		}
		if (has_current_errors()) break;
	}
}

/*
 * The mechanism for updating the template instantiation info file for
 * the current object file.
 *
 * We base the .ii filename on the object file name by creating a name
 * as follows: dirname(obj)/ii_files/basename(obj,.o).ii.
 *
 * If this file exists, we look for a terminator (a line with the contents
 * "----"), and replace everything before that line with updated information
 * about the command-line used to compile the object file, and the working
 * directory from where the command was issued. If the terminator is missing,
 * it is prepended to the file.
 */


/*
 * Is "c" a character that would need quoting to the shell?
 */

#define is_shell_special(c) \
    ( (c) == '\'' || \
      (c) == '|' || \
      (c) == '&' || \
      (c) == '*' || \
      (c) == '?' || \
      (c) == '[' || \
      (c) == ']' || \
      (c) == ';' || \
      (c) == '!' || \
      (c) == '(' || \
      (c) == ')' || \
      (c) == '^' || \
      (c) == '<' || \
      (c) == '>' || \
      (c) <= ' ' || \
      (c) == '\t' || \
      is_shell_quote_special(c) )

/*
 * Is "c" a character that would have to be \-escaped inside double-quotes?
 *
 * The answer to this one actually depends on the shell. Especially
 * troublesome is '!': for csh, it *must* be \-escaped inside quotes,
 * and for sh, it *must not* be \-escaped. sh preserves the \ if it
 * is not quoting something significant. Sigh.
 *
 * However, since most "make"s and "make"-clones use /bin/sh as the shell
 * regardless of the SHELL setting, and since system() always uses /bin/sh
 * to execute the command, I am going to stick with whatever sh needs.
 */
#define is_shell_quote_special(c) \
    ( (c) == '"' || \
      (c) == '\\' || \
      (c) == '`' || \
      (c) == '$' )


/*
 * returns the length of the argument after inserting any required quoting
 * characters.
 */
int quoted_length(char *p, int *quoted)
{
    int len = 0;
    char c;

    *quoted = 0;

    while (c = *p++) {
	if (!(*quoted) &&
	    is_shell_special(c)) {
	    (*quoted) = 1;
	    len += 2;
	}
	if (is_shell_quote_special(c))
	    len ++;
	len ++;	/* the character itself */
    }
    return len;
}

/*
 * writes a quoted (if necessary) copy of p into the buffer pointed to
 * by buf, and returns the length of the quoted string written.
 */
int quote_shell_arg(char *p, char *buf)
{
    char c;
    int quoted = 0;
    int len;

    len = quoted_length(p, &quoted);

    if (quoted)
	*buf++ = '"';
    while (c = *p++) {
	if (is_shell_quote_special(c))
	    *buf++ = '\\';
	*buf++ = c;
    }
    if (quoted)
	*buf++ = '"';

    return len;
}


int saved_argc;
char **saved_argv;
static int curr_argc;
static int add_c = 0;

/*
 * Save off the command-line in a quote-protected string that can be re-fed
 * to the shell later by edg_prelink.  We save off any "-o" and its argument
 * to a separate global. This is *not* emitted into the ii_file if we are
 * doing a single-source compile and link with libraries.
 */

void add_minus_c_option(void)
{
    add_c = 1;
}

void save_command_line(int argc, char **argv)
{
    int i;
    saved_argc = argc;
    saved_argv = malloc((saved_argc+1) * sizeof(char *));

    for (i = 0; i < argc; i++) {
	if (argv[i])
	    saved_argv[i] = string_copy(argv[i]);
	else
	    saved_argv[i] = NULL;
    }
    saved_argv[saved_argc] = NULL;
}

int check_for_saved_option(char *opt)
{
    int i;

    for (i = 1; i < saved_argc; i++) {
	if (strcmp(opt, saved_argv[i]) == 0)
	    return 1;
    }

    return 0;
}

void set_current_arg_pos(int n)
{
    curr_argc = n;
}

void cancel_saved_arg(int count)
{
    int i;
    for (i = 0; i < count; i++) {
	if (saved_argv[curr_argc+i] != NULL)
	    free(saved_argv[curr_argc+i]);
	saved_argv[curr_argc+i] = NULL;
    }
}

/*
 * Save off the command-line in a quote-protected string that can be re-fed
 * to the shell later by edg_prelink.  We save off any "-o" and its argument
 * to a separate global. This is *not* emitted into the ii_file if we are
 * doing a single-source compile and link with libraries.
 */
static void convert_saved_command_line_into_string ( void )
{
    int len = 0;
    int quoted = 0;
    int i;
    char *p;

    /* first, count the length */
    for (i = 1; i < saved_argc; i++) {
	if (saved_argv[i] != NULL) {
	    len += quoted_length(saved_argv[i], &quoted) + 1;
	}
    }

    /* allocate the space */
    command_line = p = malloc(len+1);

    /* Now copy the arguments */
    for (i = 1; i < saved_argc; i++) {
	if (saved_argv[i] != NULL) {
	    p += quote_shell_arg(saved_argv[i], p);
	    *p++ = ' ';
	}
    }

    if (p > command_line)
	p[-1] = '\0';
    else
	p[0] = '\0';
}

static void write_command_string_into_file(FILE *cmdfile,
					   char *sourcefile,
					   int for_ii_file)
{
    if (for_ii_file)
	fputs("CMDLINE=", cmdfile);
    
    fprintf(cmdfile, "%s ", orig_program_name);

    if (add_c) {
	/* always emit -c into file, because we will always be
	 * recompiling only one file at a time downstream in, e.g.,
	 * the prelinker, or the Fix and Continue mechanism
	 */
	fputs("-c ", cmdfile);
    }
    fprintf(cmdfile, "%s %s", command_line, sourcefile);
    if (outfile != NULL        && 
	!multiple_source_files && 
	(
	 invoked_lang != L_CC ||
         !for_ii_file || remember_last_phase != P_any_ld)) {
	/* Got a "cc -c -o foo foo.o", so emit the object file name */
	fprintf(cmdfile, " -o %s", outfile);
    }
    fprintf(cmdfile, "\n");
    
    /* Now write out current working directory */
    if (for_ii_file)
	fputs("PWD=", cmdfile);
    fprintf(cmdfile, "%s\n", get_cwd());

    /* trailer */
    if (for_ii_file)
	fputs("----\n", cmdfile);
}

static void add_command_line_arg(string_list_t *args, char *source_file)
{
    char *cmd_file_name;
    FILE *cmd_file;
    /*
     * convert the saved command line prefix
     * into a string (to which the filename will be
     * appended.
     */
    convert_saved_command_line_into_string();
    /*
     * Now write out a command line and pwd for
     * passing down to the frontends for DWARF.
     */
    cmd_file_name = create_temp_file_name("L");
    if (execute_flag) {
	cmd_file = fopen(cmd_file_name, "w");
    	if (cmd_file == NULL) {
	    error("write_command_string: could not create %s", cmd_file_name);
    	} else {
	    write_command_string_into_file(cmd_file, source_file,
				       /*for_ii_file=*/FALSE);
	    fclose(cmd_file);
	}
    }
    if (source_lang == L_cc ||
	source_lang == L_CC ||
	source_lang == L_f90) 
    {
	    add_string(args, concat_strings("-FE:cmdline=", cmd_file_name));
    }
}

/*
 * Skip over leading lines upto and including the terminator (a line that
 * starts with ----). If the terminator is not found, rewind back to start.
 */
void skip_old_ii_controls(FILE *f)
{
    int c;
    int terminator_found = 0;

    c = getc(f);
    while (c != EOF) {
	if (c == '-' &&
	    (c = getc(f)) == '-' &&
	    (c = getc(f)) == '-' &&
	    (c = getc(f)) == '-') {
	    terminator_found = 1;
	}
	while (c != '\n' && c != EOF)
	    c = getc(f);
	if (terminator_found)
	    break;
	if (c == '\n')
	    c = getc(f);
    }

    if (c == EOF)
	rewind(f);
}

/*
 * Make the ".ii" file name from the object file name.
 */
static char *make_ii_file_name(char *objname)
{
    char *base = driver_basename(objname);
    int baselen = strlen(base);

    base = concat_strings(base, "   ");
    if (base[baselen-2] == '.'&& base[baselen-1] == 'o')
	strcpy(&base[baselen-1], "ii");
    else
	strcpy(&base[baselen], ".ii");

    return concat_strings(
		dirname(objname),
		concat_strings("/ii_files/", base));
}

/*
 * Make the ".rii" file name from the object file name.
 */
static char *make_rii_file_name(char *objname)
{
    char *base = driver_basename(objname);
    int baselen = strlen(base);

    base = concat_strings(base, "    ");
    if (base[baselen-2] == '.' && base[baselen-1] == 'o')
	strcpy(&base[baselen-1], "rii");
    else
	strcpy(&base[baselen], ".rii");

    return concat_strings(
		dirname(objname),
		concat_strings("/rii_files/", base));
}

/*
 * The actual function that updates the instantiation information
 * in the .ii file.
 */
int update_instantiation_info_file(char *ii_file_name, char* sourcefile)
{
    char *new_ii_file_name = concat_strings(ii_file_name, ".NEW");
    
    FILE *old_ii = fopen(ii_file_name, "r");
    FILE *new_ii;
    int c;
    
    if (old_ii != NULL) {
	/* There is an existing .ii file (may be empty) */
	/* need to update that file! */

	if (show_flag) {
	    fprintf(stderr, "%s: update_instantiation_info_file %s\n",
		    program_name, ii_file_name);
	}

	new_ii = fopen(new_ii_file_name, "w");
	if (new_ii == NULL) {
	    /* could not open the (new) file: permission problem in
	     * directory? */
	    error("update_instantiation_info_file: error in creating file %s",
		  new_ii_file_name);
	    perror("update_instantiation_info_file");
	    return 1;
	}

	/*
	 * skip over the existing control information (upto the standard
	 * separator), if any.
	 */
	skip_old_ii_controls(old_ii);

	/*
	 * Now, write out the new control information. For now, we have
	 * only two lines:
	 * CMDLINE=<compiler name> <quote-protected command-line arguments>
	 * PWD=<current working directory>
	 */
	write_command_string_into_file(new_ii, sourcefile,
				       /*for_ii_file=*/TRUE);

	/* Now copy over the remainder of the old file. */
	while ((c = getc(old_ii)) != EOF)
	    putc(c, new_ii);

	/* Rename the new file to replace the existing .ii file */
	fclose(old_ii);
	fclose(new_ii);
	if (rename(new_ii_file_name, ii_file_name) < 0) {
	    error("update_instantiation_info_file: error in renaming %s to %s",
		  new_ii_file_name, ii_file_name);
	    perror("update_instantiation_info_file");
	    return 1;
	}
    }
    free(new_ii_file_name);
    return 0;
}


/* ====================================================================
 *
 * add_ipl_cmd_string
 *
 * Given an option identified by iflag, add the appropriate option
 * string(s) to the ipl_cmds list used by IPL as the back end command
 * list to be passed via the object file to the back end under IPA.
 *
 * ====================================================================
 */

static void
add_ipl_cmd_string (int iflag)
{
  if (option_matches_phase (iflag, P_be)) {
    /* Assume inside a FOREACH_IMPLIED_OPTION iteration */
    char *name = get_current_implied_name();
    
    /* If this is not one of the driver-recognized options, then
     * we need to prefix it with -Wb, -- doing so also requires
     * replacing internal spaces with commas:
     */
    if ( debug ) {
	fprintf ( stderr, "%s: %sternal, #%d of %d, %s\n",
		name,
		is_internal_option (iflag) ? "in" : "ex",
		iflag, LAST_PREDEFINED_OPTION,
		is_derived_option (iflag) ? "derived" : "primary"
	       );
    }

    if ( is_internal_option (iflag)
      || ( iflag >= LAST_PREDEFINED_OPTION
	&& ! is_derived_option (iflag) ) )
    {
      char *p;
      name = concat_strings ("-Wb,", name );
      for ( p = name; *p != 0; p++ ) {
	if ( *p == ' ' ) *p = ',';
      }
    }

    if (strcmp(name,"-pfa") == 0) {
      /* change pfa to -Wb,-pfa, since after IPA we only want auto-parallelism
       * from be/LNO, but not the mfef77/rii_files or be/dsm_clone, etc.
       */
      name = concat_strings ("-Wb,", name);
    }
    
    /* Add the option (possibly with blanks) to the list: */
    if (strcmp(name,"-dsm_clone")!=0)
      add_multi_strings (ipl_cmds, name, FALSE);
  }
} /* add_ipl_cmd_string */


void
save_ipl_commands (void)
{
    int i;
    int iflag;

    /* Build the string list initialized with the command name and -non_shared
     * if necessary:
     */
    if (ipl_cmds == 0) {
      ipl_cmds = init_string_list ();
      add_string (ipl_cmds, program_name);
      if ( shared == NON_SHARED ) {
	add_string (ipl_cmds, "-non_shared");
      }
    }
    
    FOREACH_OPTION_SEEN(i) {
	if ( i == O_Unrecognized || i == O_show )
	    continue;
	if (option_matches_language (i, invoked_lang)) {
	    FOREACH_IMPLIED_OPTION(iflag, i) {
	   	 add_ipl_cmd_string (iflag);
	    }
	}
    }

    // Add -TARG options.
    add_targ_options(ipl_cmds);

} /* save_ipl_commands */



/*
 * set the freeform/fixedform switch appropriately 
 * for mfef95 and ftpp. Also set the line length for ftpp, but only in 
 * fixed form.
 */
static void
set_f90_source_form(string_list_t *args,boolean set_line_length)
{
   char buf[16];
   
   sprintf(buf,"-N%d",fortran_line_length);

   if (fortran_form == 2) {
      /* User specified -freeform */
      add_string(args,"-ffree");
   } else if (fortran_form == 1) {
      /* User specified fixed form */
      add_string(args,"-ffixed");
      if (set_line_length) add_string(args,buf);
   } else {
      /* Unspecified, need to get the extension (.f or .f90) */
      if (source_kind == S_f90 || source_kind == S_F90) {
	 add_string(args,"-ffree");
      } else {
	 add_string(args,"-ffixed");
	 if (set_line_length) add_string(args,buf);
      }
   }
}

/*
 * utility routine to set arguments used by both passes of f90
 *
 */

static void
do_f90_common_args(string_list_t *args)
{
   /* Handle the source form options */
   set_f90_source_form(args,FALSE);

   add_string(args,"-LANG:=F90");
 /* Bug 4210 */
   if (0 != f90_module_dir) {
     /* -J is already prepended */
     add_string(args, f90_module_dir);
     }
 /* Bug 4210 */
}

// Set the stack size to the hard limit.
static void
set_stack_size()
{
#ifdef RLIMIT_STACK
  struct rlimit rl;
  rlim_t max_stack;

  // Read old limit.
  if (getrlimit(RLIMIT_STACK, &rl) == -1) {
    warning("cannot read stack size limit");
    return;
  }
  if (rl.rlim_cur == RLIM_INFINITY) {
    return;
  }

  // Set new limit.
  max_stack = rl.rlim_max;
  rl.rlim_cur = max_stack;
  if (setrlimit(RLIMIT_STACK, &rl) == -1) {
    warning("cannot change stack size limit");
    return;
  }
  if (getrlimit(RLIMIT_STACK, &rl) == -1 ||
      rl.rlim_cur != max_stack) {
    warning("cannot change stack size limit");
    return;
  }
#endif
}
