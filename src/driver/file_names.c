/*
   Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/
/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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

/* To work around older glibc that doesn't have strdupa(). This makes it so we don't have to define on the command line globally */
#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include "phases.h"
#include "lang_defs.h"
#include "string_utils.h"
#include "file_names.h"
#include "file_utils.h"
#include "errors.h"
#include "opt_actions.h"
#include "option_seen.h"
#include "option_names.h"
#include "run.h"
#include "version.h"

extern int errno;

boolean keep_flag = FALSE;

string_list_t *count_files = NULL;
static string_list_t *temp_files = NULL;
#ifdef KEY /* Bug 11265 */
string_list_t *isystem_dirs = NULL;
#endif /* KEY Bug 11265 */
static char *tmpdir = NULL;
static char *saved_object = NULL;

#ifdef _WIN32
#define DEFAULT_TMPDIR	"."
#else
#define DEFAULT_TMPDIR	"/tmp"
#endif

static string_pair_list_t *temp_obj_files = NULL;


/* get object file corresponding to src file */
char *
get_object_file (char *src)
{
	// bug 2025
	// Create .o files in /tmp in case the src dir is not writable.
	if (!(keep_flag || (ipa == TRUE) || remember_last_phase == P_any_as)) {
	  char *tmp_file = NULL;
	  char *obj_name = change_suffix(src, "o");
	  string_pair_item_t *p;
	  FOREACH_STRING_PAIR (p, temp_obj_files) {
	    if (strcmp (STRING_PAIR_KEY(p), obj_name) == 0)
	      return STRING_PAIR_VAL(p);
	  }
	  // Create temp file name as in create_temp_file_name.
	  tmp_file = concat_strings(tmpdir, "/cco.XXXXXX");
	  int fd = mkstemp(tmp_file);
      if(fd == -1) {
		error("mkstemp failed for template: %s", tmp_file);
        return NULL;
      }

	  close(fd);
	  add_string_pair (temp_obj_files, obj_name, tmp_file);
	  return tmp_file;
	}

	// Handle IPA .o files corresponding to sources with the same basename,
	// e.g., a.c and foo/a.c.  Create unique .o files by substituting '/'
	// in the source name with '%'.  Bugs 9097, 9130.
	if (ipa == TRUE &&
	    !option_was_seen(O_c) &&
	    keep_flag != TRUE) {
	  char *p;
#if !defined(__linux__)
	  src = strcpy(alloca(strlen(src) + sizeof '\0'), src);
#else /* defined(BUILD_OS_DARWIN) */
	  src = strdupa(src);
#endif /* defined(BUILD_OS_DARWIN) */
	  for (p = src; *p != '\0'; p++) {
	    if (*p == '/')
	      *p = '%';
	  }
	}

	return change_suffix(drop_path(src), "o");
}

/*
 * Need temp file names to be same if use same suffix
 * (because this can be called for both producer and consumer
 * of temp file), but also need names that won't conflict.
 * Put suffix in standard place so have easy way to check 
 * if file already created. 
 * Use mkstemp to generate unique file name;
 */
char *
create_temp_file_name (char *suffix)
{
	string_item_t *p;
#ifdef _WIN32
	char *s;
#endif
	char *prefix, *path;
	size_t prefix_len;
	int fd;

	asprintf(&prefix, "%s/pathcc-%s-", tmpdir, suffix);
	prefix_len = strlen(prefix);

#ifdef _WIN32
	char *s;
	while ((s = strchr (s, '/')) != NULL) {
		*s = '\\';
	}
#endif

	for (p = temp_files->head; p != NULL; p = p->next) {
		if (strncmp(p->name, prefix, prefix_len) == 0) {
			/* matches the suffix */
			free(prefix);
			return p->name;
		}
	}

	for (;;) {
		asprintf(&path, "%s%08x.%s", prefix,
		    (unsigned)random() & 0xffffffffU, suffix);
		fd = open(path, O_CREAT | O_EXCL, 0600);
		if (fd != -1)
			break;
	}

	close(fd);
	free(prefix);
	add_string (temp_files, path);
	return path;
}

char *
construct_name (char *src, char *suffix)
{
	if (keep_flag || current_phase == remember_last_phase) {
		char *srcname;
		/* 
		 * if -c -o <name>, then use name.suffix
		 * (this helps when use same source to create different .o's)
		 * if outfile doesn't have .o suffix, don't do this.
		 */
		if (outfile && option_was_seen(O_c) && get_suffix(outfile))
			srcname = outfile;
		else
			srcname = src;
		return change_suffix(drop_path(srcname), suffix);
	} else {
		return create_temp_file_name (suffix);
	}
}

/* use given src name, but check if treated as a temp file or not */
char *
construct_given_name (char *src, char *suffix, boolean keep)
{
	char *s;
	s = change_suffix(drop_path(src), suffix);
	if (keep || current_phase == remember_last_phase) {
		return s;
	} else {
		s = string_copy(s);
		add_string_if_new (temp_files, s);
		return s;
	}
}

void
mark_saved_object_for_cleanup ( void )
{
	if (saved_object != NULL)
		add_string_if_new (temp_files, saved_object);
}

/* Create filename with the given extension; eg. foo.anl from foo.f */
char *
construct_file_with_extension (char *src, char *ext)
{
	return change_suffix(drop_path(src),ext);
}

void
init_temp_files (void)
{
	const char *tmpdir_env[] = {"TMP", "TEMP", "TMPDIR"};
	int i;

	for (i = 0; i < 3; i++) {
		char *t = string_copy(getenv(tmpdir_env[i]));

#ifdef _WIN32
		if (t != NULL) {
			char *s = t;

			while ((s = strchr (s, '\\')) != NULL) {
				*s = '/';
			}
		}
#endif
		if (t != NULL && is_directory(t) && directory_is_writable(t)) {
			int len = strlen(t);

			if (IS_DIR_SLASH(t[len - 1])) {
				/* drop / or \ at end so strcmp matches */
				t[len - 1] = '\0';
			}
			tmpdir = t;
			break;
		}
	}
	if (tmpdir == NULL) {
		tmpdir = DEFAULT_TMPDIR;
	}

	temp_files = init_string_list();
	temp_obj_files = init_string_pair_list();
}

void
init_count_files (void)
{
	count_files = init_string_list();
}

static char *report_file;

void
init_crash_reporting (void)
{
	if ((report_file = getenv("PSC_CRASH_REPORT")) != NULL)
		goto bail;

	if (asprintf(&report_file, "%s/ekopath_crash_XXXXXX", tmpdir) == -1) {
		report_file = NULL;
		goto bail;
	}
	
	if (mkstemp(report_file) == -1) {
		report_file = NULL;
		goto bail;
	}
	
	setenv("PSC_CRASH_REPORT", report_file, 1);
bail:
	return;
}

static int save_count;

static int
save_cpp_output (char *path)
{
	char *save_dir, *save_path, *final_path;
	FILE *ifp = NULL, *ofp = NULL;
	char *name = drop_path(path);
	char buf[4096];
	int saved = 0;
	size_t nread;
	char *suffix;
	char *home;
	time_t now;
	int i;

	if (strncmp(name, "cci.", 4) == 0)
		suffix = ".i";
	else if (strncmp(name, "ccii.", 5) == 0)
		suffix = ".ii";
	else
		goto bail;

	if ((ifp = fopen(path, "r")) == NULL)
		goto bail;

	if ((save_dir = getenv("PSC_PROBLEM_REPORT_DIR")) == NULL &&
	    (home = getenv("HOME")) != NULL) {
		asprintf(&save_dir, "%s/.ekopath-bugs", home);
	}

	if (save_dir && mkdir(save_dir, 0700) == -1 && errno != EEXIST) {
		save_dir = NULL;
	}

	if (save_dir == NULL) {
		save_dir = tmpdir;
	}

	asprintf(&save_path, "%s/%s_error_XXXXXX", save_dir, program_name);

	if (mkstemp(save_path) == -1) {
		goto b0rked;
	}

	if ((ofp = fopen(save_path, "w")) == NULL) {
		goto b0rked;
	}
	
	now = time(NULL);
	fprintf(ofp, "/*\n\nPathScale(TM) compiler problem report - %s",
		ctime(&now));
	fprintf(ofp, "Please report this problem to http://support.pathscale.com .\n");
	fprintf(ofp, "If possible, please attach a copy of this file with your "
		"report.\n");
	fprintf(ofp, "\nPLEASE NOTE: This file contains a preprocessed copy of the "
		"source file\n"
		"that may have led to this problem occurring.\n");

	fprintf(ofp, "\nCompiler command line (%s ABI used):\n",
		abi == ABI_N32 ? "32-bit" : "64-bit");

	fprintf(ofp, " ");
	for (i = 0; i < saved_argc; ++i)
		if (saved_argv[i] &&
		    strcmp(saved_argv[i], "-default_options") != 0) {
			int len;
			len = quote_shell_arg(saved_argv[i], buf);
			buf[len] = '\0';
			fprintf(ofp, " %s", buf);
		}
	fprintf(ofp, "\n\n");

	fprintf(ofp, "Version %s build information:\n",
		compiler_version);
	fprintf(ofp, "  Changeset %s\n", cset_id);
	fprintf(ofp, "  Built by %s@%s in %s\n", build_user,
		build_host, build_root);
	fprintf(ofp, "  Build date %s\n", build_date);
	
	if (report_file) {
		int newline = 1;
		struct stat st;
		FILE *rfp;

		if (stat(report_file, &st) == -1)
			goto no_report;
		
		if (st.st_size == 0)
			goto no_report;

		fprintf(ofp, "\nDetailed problem report:\n");
		if ((rfp = fopen(report_file, "r")) == NULL) {
			goto no_report;
		}

		while (fgets(buf, sizeof(buf), rfp) != NULL) {
			int len = strlen(buf);
			if (newline)
				fputs("  ", ofp);
			fputs(buf, ofp);
			newline = buf[len - 1] == '\n';
		}
		if (!newline)
			putc('\n', ofp);

		fclose(rfp);
	}

no_report:	
	if (string_list_size(error_list)) {
		string_item_t *i;
		fprintf(ofp, "\nInformation from compiler driver:\n");
		FOREACH_STRING(i, error_list) {
			fprintf(ofp, "  %s\n", STRING_NAME(i));
		}
	}

	fprintf(ofp, "\nThe remainder of this file contains a preprocessed copy of "
		"the\n"
		"source file that appears to have led to this problem.\n\n*/\n");
	
	while ((nread = fread(buf, 1, sizeof(buf), ifp)) > 0) {
		size_t nwrit;
		if ((nwrit = fwrite(buf, 1, nread, ofp)) < nread) {
			if (nwrit != 0)
				errno = EFBIG;
			goto b0rked;
		}
	}
	
	fprintf(ofp, "\n/* End of PathScale problem report. */\n");
	
	asprintf(&final_path, "%s%s", save_path, suffix);
	rename(save_path, final_path);

	if (save_count == 0) {
		fprintf(stderr, "Please report this problem to "
			"http://support.pathscale.com.\n");
	}

	fprintf(stderr, "Problem report saved as %s\n", final_path);
	save_count++;
	saved = 1;
	
	goto bail;
b0rked:
	fprintf(stderr, "Could not save problem report to %s: %s\n",
		save_path, strerror(errno));
bail:
	if (ifp != NULL)
		fclose(ifp);
	if (ofp != NULL)
		fclose(ofp);
		
	return saved;
}

void
cleanup (void)
{
	/* cleanup temp-files */
	string_item_t *p;
	int status;
	if (temp_files == NULL) return;
	for (p = temp_files->head; p != NULL; p = p->next) {
		if (debug) printf("unlink %s\n", p->name);
		if (execute_flag) {
			if (internal_error_occurred)
				save_cpp_output(p->name);
			status = unlink(p->name);
			if (status != 0 && errno != ENOENT) {
				internal_error("cannot unlink temp file %s", p->name);
				perror(program_name);			
			}
		}
	}
	temp_files->head = temp_files->tail = NULL;

	if (save_count) {
		fprintf(stderr, "Please review the above file%s and, "
			"if possible, attach %s to your problem report.\n",
			save_count == 1 ? "" : "s",
			save_count == 1 ? "it" : "them");
		if (invoked_lang == L_f77 || invoked_lang == L_f90)
		  fprintf(stderr, "Additional included source files or modules may also be needed to reproduce the problem.\n");
	}
}

void
mark_for_cleanup (char *s)
{
	add_string_if_new (temp_files, s);
}

void
cleanup_temp_objects ()
{
  // Delete the mapped files.
  string_pair_item_t *p;
  FOREACH_STRING_PAIR (p, temp_obj_files) {
    char *s = STRING_PAIR_VAL(p);
    int status = unlink (s);
    if (status != 0 && errno != ENOENT) {
      internal_error("cannot unlink temp object file %s", s);
      perror(program_name);
    }
  }
  if (report_file) {
    unlink(report_file);
  }
}

#ifdef KEY
char *
get_report_file_name() {
  return report_file;
}
#endif
