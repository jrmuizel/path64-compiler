
#if defined(TARG_ST) && defined(STACK_TRACE)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "elfr.h"

struct elfr_handle_t_ {
  FILE *file;
  int errnum;
  int swap;
  Elf32_Ehdr header;
  Elf32_Shdr *shdrs;
  Elf32_Sym *syms;
  char **datas;
};

enum {
  ELFR_ERR_OK,
  ELFR_ERR_EOF,
  ELFR_ERR_NOT_ELF,
  ELFR_ERR_NOT_ELF32,
  ELFR_ERR_ENCODING,
  ELFR_ERR_VERSION,
  ELFR_ERR_HEADER,
  ELFR_ERR_SECTION_HEADER,
  ELFR_ERR_SYMBOL,
  ELFR_ERR_NOMEM,
};

static const char *strerrors[] = {
  "",
  "EOF unexpected while reading file",
  "not an ELF file",
  "not an ELF32 file",
  "encoding not supported for ELF file",
  "version not supported for ELF file",
  "error while reading ELF header",
  "error while reading ELF section header",
  "error while reading ELF symbol table",
  "memory exhausted",
};

static int elfr_readn(elfr_handle_t h, void *buffer, int size);
static int elfr_read_char(elfr_handle_t h, void *buffer);
static int elfr_read_half(elfr_handle_t h, void *buffer);
static int elfr_read_word(elfr_handle_t h, void *buffer);
static int elfr_set_filepos(elfr_handle_t h, int pos);
static int elfr_access_section_data(elfr_handle_t h, int snum);
static int elfr_access_section_symbols(elfr_handle_t h, int snum);
static Elf32_Shdr *elfr_section_header(elfr_handle_t h, int snum);

const char *
elfr_strerror(elfr_handle_t h)
{
  return strerrors[h->errnum];
}

elfr_handle_t
elfr_new(FILE *file)
{
  elfr_handle_t h = (elfr_handle_t)calloc(sizeof(*h),1);
  h->file = file;
  return h;
}

void
elfr_del(elfr_handle_t h)
{
  if (h->syms != NULL) free(h->syms);
  if (h->datas != NULL) {
    int i;
    for (i = 0; i < elfr_section_num(h); i++) {
      if (h->datas[i] != NULL) free(h->datas[i]);
    }
    free(h->datas);
  }
  if (h->shdrs != NULL) free(h->shdrs);
  free(h);
}

static int 
elfr_read_elf_ident(elfr_handle_t h)
{
  int host_lsb;
  union { char c[2]; Elf32_Half h;} half;
  half.h = 0x0201;
  if (half.c[0] == 0x01) host_lsb = 1;
  else host_lsb = 0;

  /* Read the ELF header ident array.  */
  if (fread(&h->header.e_ident[0], 1, EI_NIDENT, h->file) != EI_NIDENT) {
    h->errnum = ELFR_ERR_NOT_ELF;
    return -1;
  }
  
  /* Check the ELF magic string. */
  if (memcmp(&h->header.e_ident[0], ELFMAG, SELFMAG) != 0) {
    h->errnum = ELFR_ERR_NOT_ELF;
    return -1;
  }

  /* Check the ELF class. */
  if (h->header.e_ident[EI_CLASS] != ELFCLASS32) {
    h->errnum = ELFR_ERR_NOT_ELF32;
    return -1;
  }

  /* Check the ELF encoding. */
  if (h->header.e_ident[EI_DATA] == ELFDATA2LSB) {
    if (host_lsb) h->swap = 0;
    else h->swap = 1;
  } else if (h->header.e_ident[EI_DATA] == ELFDATA2MSB) {
    if (host_lsb) h->swap = 1;
    else h->swap = 0;
  } else {
    h->errnum = ELFR_ERR_ENCODING;
    return -1;
  }

  /* Check the ELF version. */
  if (h->header.e_ident[EI_VERSION] != EV_CURRENT) {
    h->errnum = ELFR_ERR_VERSION;
    return -1;
  }
  return 0;
}

static int 
elfr_read_elf_header(elfr_handle_t h, Elf32_Ehdr *hdr)
{
  if ((elfr_read_half(h, &hdr->e_type) == -1) ||
      (elfr_read_half(h, &hdr->e_machine) == -1) ||
      (elfr_read_word(h, &hdr->e_version) == -1) ||
      (elfr_read_word(h, &hdr->e_entry) == -1) ||
      (elfr_read_word(h, &hdr->e_phoff) == -1) ||
      (elfr_read_word(h, &hdr->e_shoff) == -1) ||
      (elfr_read_word(h, &hdr->e_flags) == -1) ||
      (elfr_read_half(h, &hdr->e_ehsize) == -1) ||
      (elfr_read_half(h, &hdr->e_phentsize) == -1) ||
      (elfr_read_half(h, &hdr->e_phnum) == -1) ||
      (elfr_read_half(h, &hdr->e_shentsize) == -1) ||
      (elfr_read_half(h, &hdr->e_shnum) == -1) ||
      (elfr_read_half(h, &hdr->e_shstrndx) == -1)) {
    h->errnum = ELFR_ERR_HEADER;
    return -1;
  }
  return 0;
}

static int
elfr_read_section_header(elfr_handle_t h, Elf32_Shdr *hdr)
{
  if ((elfr_read_word(h, &hdr->sh_name) == -1) ||
      (elfr_read_word(h, &hdr->sh_type) == -1) ||
      (elfr_read_word(h, &hdr->sh_flags) == -1) ||
      (elfr_read_word(h, &hdr->sh_addr) == -1) ||
      (elfr_read_word(h, &hdr->sh_offset) == -1) ||
      (elfr_read_word(h, &hdr->sh_size) == -1) ||
      (elfr_read_word(h, &hdr->sh_link) == -1) ||
      (elfr_read_word(h, &hdr->sh_info) == -1) ||
      (elfr_read_word(h, &hdr->sh_addralign) == -1) ||
      (elfr_read_word(h, &hdr->sh_entsize) == -1)) {
    h->errnum = ELFR_ERR_SECTION_HEADER;
    return -1;
  }
  return 0;
}  

static int
elfr_read_symbol(elfr_handle_t h, Elf32_Sym *sym)
{
  if ((elfr_read_word(h, &sym->st_name) == -1) ||
      (elfr_read_word(h, &sym->st_value) == -1) ||
      (elfr_read_word(h, &sym->st_size) == -1) ||
      (elfr_read_char(h, &sym->st_info) == -1) ||
      (elfr_read_char(h, &sym->st_other) == -1) ||
      (elfr_read_half(h, &sym->st_shndx) == -1)) {
    h->errnum = ELFR_ERR_SECTION_HEADER;
    return -1;
  }
  return 0;
}  

static int
elfr_read_all_section_headers(elfr_handle_t h)
{
  int i;
  
  if (h->header.e_shoff == 0 || h->header.e_shnum == 0) return 0;
  h->shdrs = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr) * h->header.e_shnum);
  if (h->shdrs == NULL) {
    h->errnum = ELFR_ERR_NOMEM;
    return -1;
  }
  for (i = 0; i < h->header.e_shnum; i++) {
    elfr_set_filepos(h, h->header.e_shoff + i * h->header.e_shentsize);
    if (elfr_read_section_header(h, &h->shdrs[i]) == -1) return -1;
  }
  return 0;
}


int
elfr_read(elfr_handle_t h)
{
  if (elfr_read_elf_ident(h) == -1) return -1;
  if (elfr_read_elf_header(h, &h->header) == -1) return -1;
  if (elfr_read_all_section_headers(h) == -1) return -1;
  
  return 0;
}

static int
elfr_access_section_data(elfr_handle_t h, int snum)
{
  Elf32_Shdr *phdr;
  int data_size;
  assert(h->shdrs != NULL);
  if (!elfr_section_has_data(h, snum)) return 0;
  
  if (h->datas == NULL) {
    h->datas = (char **)calloc(sizeof(char *), h->header.e_shnum);
    if (h->datas == NULL) {
      h->errnum = ELFR_ERR_NOMEM;
      return -1;
    }
  }

  phdr = elfr_section_header(h, snum);
  data_size = phdr->sh_size;
  if (h->datas[snum] == NULL) {
    h->datas[snum] = (char *)malloc(data_size);
    if (h->datas == NULL) {
      h->errnum = ELFR_ERR_NOMEM;
      return -1;
    }
    elfr_set_filepos(h, phdr->sh_offset);
    if (elfr_readn(h, h->datas[snum], data_size) == -1) {
      return -1;
    }
  }
  return 0;
}

static int
elfr_access_section_symbols(elfr_handle_t h, int snum)
{
  int sym_num;
  int i;

  if (h->syms != NULL) return 0;
  
  sym_num = elfr_section_symbol_num(h, snum);
  if (sym_num == 0) return 0;
  
  if (!elfr_section_has_data(h, snum)) {
    h->errnum = ELFR_ERR_SYMBOL;
    return -1;
  }
  
  h->syms = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*sym_num);
  if (h->syms == NULL) {
    h->errnum = ELFR_ERR_NOMEM;
    return -1;
  }
  
  elfr_set_filepos(h, elfr_section_header(h, snum)->sh_offset);
  for (i = 0; i < sym_num; i++) {
    if (elfr_read_symbol(h, &h->syms[i]) == -1) return -1;
  }
  return 0;
}


int
elfr_start(elfr_handle_t h)
{
  return h->header.e_entry;
}

int
elfr_has_section(elfr_handle_t h)
{
  return h->header.e_shoff != 0;
}

int
elfr_section_num(elfr_handle_t h)
{
  return h->header.e_shnum;
}

static Elf32_Shdr *
elfr_section_header(elfr_handle_t h, int snum)
{
  assert(snum >= 0 && snum < h->header.e_shnum);
  return &h->shdrs[snum];
}

int
elfr_section_data(elfr_handle_t h, int snum, const char **pdata)
{
  assert(snum >= 0 && snum < h->header.e_shnum);
  if (elfr_access_section_data(h, snum) == -1) return -1;
  if (elfr_section_has_data(h, snum)) *pdata = (const char *)h->datas[snum];
  else *pdata = NULL;
  return 0;
}

int
elfr_get_section(elfr_handle_t h, const char *s_name)
{
  int i;
  for (i = 0; i < elfr_section_num(h); i++) {
    const char *name;
    if (elfr_section_name(h, i, &name) == -1) {
      return -1;
    }
    if (name == NULL) continue;
    if (strcmp(name, s_name) == 0) return i;
  }
  return -1;
}

int
elfr_section_size(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_size;
}

int
elfr_section_address(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_addr;
}

int
elfr_section_is_alloc(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_flags & SHF_ALLOC;
}

int
elfr_section_is_symtab(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_type == SHT_SYMTAB;
}

int
elfr_section_has_data(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_type != SHT_NOBITS;
}

int
elfr_section_name(elfr_handle_t h, int snum, const char **pname)
{
  const char *data;
  Elf32_Word offset = elfr_section_header(h, snum)->sh_name;
  if (offset == 0) {
    *pname = NULL;
    return 0;
  }
  if (elfr_section_data(h, h->header.e_shstrndx, &data) == -1) return -1;
  *pname = &data[offset];
  return 0;
}

int
elfr_section_symbol_num(elfr_handle_t h, int snum)
{
  return elfr_section_header(h, snum)->sh_size / sizeof(Elf32_Sym);
}

int
elfr_section_symbol(elfr_handle_t h, int snum, int sym, const Elf32_Sym **psym)
{
  if (elfr_access_section_symbols(h, snum) == -1) return -1;
  *psym = &h->syms[sym];
  return 0;
}

int
elfr_section_string(elfr_handle_t h, int snum, int offset, const char **pname)
{
  const char *data;
  int strnum = elfr_section_header(h, snum)->sh_link;
  if (strnum == SHN_UNDEF || offset == 0) {
    *pname = NULL;
    return 0;
  }
  if (elfr_section_data(h, strnum, &data) == -1) return -1;
  *pname = &data[offset];
  return 0;
}

/*
 * File access implementation.
 */
static int
elfr_readn(elfr_handle_t h, void *buffer, int size)
{
  if (fread(buffer, 1, size, h->file) != size) {
    h->errnum = ELFR_ERR_EOF;
    return -1;
  }
  return 0;
}
  
static int
elfr_read_char(elfr_handle_t h, void *buffer)
{
  return elfr_readn(h, buffer, 1);
}

static int
elfr_read_half(elfr_handle_t h, void *buffer)
{
  void *read_buffer;
  char buffer_tmp[2];
  if (h->swap) read_buffer = buffer_tmp; 
  else read_buffer = buffer;
  if (elfr_readn(h, read_buffer, 2) == -1) return -1;
  if (h->swap) {
    char *out_buffer = (char *)buffer;
    out_buffer[0] = buffer_tmp[1];
    out_buffer[1] = buffer_tmp[0];
  }
  return 0;
}

static int
elfr_read_word(elfr_handle_t h, void *buffer)
{
  void *read_buffer;
  char buffer_tmp[4];
  if (h->swap) read_buffer = buffer_tmp; 
  else read_buffer = buffer;
  if (elfr_readn(h, read_buffer, 4) == -1) return -1;
  if (h->swap) {
    char *out_buffer = (char *)buffer;
    out_buffer[0] = buffer_tmp[3];
    out_buffer[1] = buffer_tmp[2];
    out_buffer[2] = buffer_tmp[1];
    out_buffer[3] = buffer_tmp[0];
  }
  return 0;
}

static int
elfr_set_filepos(elfr_handle_t h, int pos)
{
  fseek(h->file, pos, SEEK_SET);
  return 0;
}

#endif /* defined(TARG_ST) && defined(STACK_TRACE) */

