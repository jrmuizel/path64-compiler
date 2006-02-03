
#ifndef __ELFR_H__
#define __ELFR_H__

#if defined(TARG_ST) && defined(STACK_TRACE)

#include <stdio.h>
#include <elf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct elfr_handle_t_ elfr_handle_t_, *elfr_handle_t;

extern void elfr_del(elfr_handle_t h);
extern elfr_handle_t elfr_new(FILE *file);
extern int elfr_read(elfr_handle_t h);
extern int elfr_start(elfr_handle_t h);
extern int elfr_has_section(elfr_handle_t h);
extern int elfr_section_num(elfr_handle_t h);
extern int elfr_section_data(elfr_handle_t h, int snum, const char **pdata);
extern int elfr_section_size(elfr_handle_t h, int snum);
extern int elfr_section_address(elfr_handle_t h, int num);
extern int elfr_section_is_alloc(elfr_handle_t h, int num);
extern int elfr_section_has_data(elfr_handle_t h, int num);
extern int elfr_section_is_symtab(elfr_handle_t h, int snum);
extern int elfr_section_name(elfr_handle_t h, int num, const char **pname);
extern const char *elfr_strerror(elfr_handle_t h);
extern int elfr_section_symbol_num(elfr_handle_t h, int snum);
extern int elfr_section_symbol(elfr_handle_t h, int snum, int sym, const Elf32_Sym **psym);
extern int elfr_section_string(elfr_handle_t h, int snum, int offset, const char **pname);
extern int elfr_get_section(elfr_handle_t h, const char *s_name);

#ifdef __cplusplus
}
#endif

#endif /* defined(TARG_ST) && defined(STACK_TRACE) */


#endif

