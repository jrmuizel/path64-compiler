#ifndef PEX_H
#define PEX_H

struct pex_obj;

extern struct pex_obj *pex_init (int flags, const char *pname,
				 const char *tempbase);

#define PEX_LAST		0x1
#define PEX_SEARCH		0x2
#define PEX_SUFFIX		0x4
#define PEX_STDERR_TO_STDOUT	0x8
#define PEX_BINARY_INPUT	0x10
#define PEX_BINARY_OUTPUT	0x20

extern const char *pex_run (struct pex_obj *obj, int flags,
			    const char *executable, char * const *argv,
			    const char *outname, const char *errname,
			    int *err);

extern const char *pex_run_in_environment (struct pex_obj *obj, int flags,
			                   const char *executable,
                                           char * const *argv,
                                           char * const *env,
              	          		   const char *outname,
					   const char *errname, int *err);

extern FILE *pex_input_file (struct pex_obj *obj, int flags,
                             const char *in_name);
extern int pex_get_status (struct pex_obj *, int count, int *vector);

extern void pex_free (struct pex_obj *);

#endif
