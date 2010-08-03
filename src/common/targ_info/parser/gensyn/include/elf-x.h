/** \file elf-x.h
    \brief types definition for ELF-32 standard.

*/
#ifndef __ELF_X_H__
#define __ELF_X_H__

#if defined(__sun)
typedef unsigned int		Elf32_Addr;    /**< ELF-32 address */
typedef unsigned short  Elf32_Half;    /**< ELF-32 half word */
typedef unsigned int		Elf32_Off;     /**< ELF-32 offset */
typedef int					Elf32_Sword;   /**< ELF-32 signed word */
typedef unsigned int		Elf32_Word;    /**< ELF-32 word */
#elif defined(linux)
typedef unsigned int		Elf32_Addr;    /**< ELF-32 address */
typedef unsigned short	Elf32_Half;    /**< ELF-32 half word */
typedef unsigned int		Elf32_Off;     /**< ELF-32 offset */
typedef int					Elf32_Sword;   /**< ELF-32 signed word */
typedef unsigned int		Elf32_Word;    /**< ELF-32 word */
#elif defined(WIN32) || defined(__CYGWIN__)
typedef unsigned int		Elf32_Addr;    /**< ELF-32 address */
typedef unsigned short	Elf32_Half;    /**< ELF-32 half word */
typedef unsigned int		Elf32_Off;     /**< ELF-32 offset */
typedef int					Elf32_Sword;   /**< ELF-32 signed word */
typedef unsigned int		Elf32_Word;    /**< ELF-32 word */
#else
#error "Unknown target, please adapt elf-x.h to your platform"
#endif


#if defined(__ELF64)

#if defined(__sun)
typedef unsigned long long	   Elf64_Addr;    /**< ELF-64 address */
typedef unsigned short		   Elf64_Half;    /**< ELF-64 Half word */
typedef unsigned long long	   Elf64_Off;     /**< ELF-64 offset */
typedef int					      Elf64_Sword;   /**< ELF-64 signed word */
typedef long long			      Elf64_Sxword;  /**< ELF-64 signed extended word */
typedef	unsigned int		   Elf64_Word;    /**< ELF-64 word */
typedef	unsigned long long	Elf64_Xword;   /**< ELF-64 extended word */
#else
#error "Unknown target, please adapt gensyn_defs.h to your platform"
#endif

#endif

#define	EI_MAG0			0		/**< Magic 0 : \177 */
#define	EI_MAG1			1		/**< Magic 1 : E */
#define	EI_MAG2			2		/**< Magic 2 : L */
#define	EI_MAG3			3		/**< Magic 3 : F */
#define	EI_CLASS       4		/**< index of file class */
#define	EI_DATA			5		/**< index of endianness */
#define	EI_VERSION		6		/**< file version */
#define	EI_OSABI		   7		/**< OS/ABI identification */
#define	EI_ABIVERSION	8		/**< ABI version */
#define	EI_PAD			9		/**< start of padding bytes */
#define EI_NIDENT		   16		/**< size of e_ident */


/* e_ident[EI_CLASS] standard values */

#define	ELFCLASSNONE	0
#define	ELFCLASS32		1		/**< Elf-32 format */
#define	ELFCLASS64		2		/**< Elf-64 format */

/* e_ident[EI_DATA] standard values */

#define	ELFDATANONE		0
#define	ELFDATA2LSB		1		/**< little endian */
#define	ELFDATA2MSB		2		/**< big endian */


/* e_ident[EI_OSABI] standard values */

#define	ELFOSABINONE		0		/**< not specified */
#define	ELFOSABI_HPUX		1		/**< Hp-UX */
#define	ELFOSABI_NETBSD	2		/**< NetBSD */
#define	ELFOSABI_LINUX		3		/**< Linux */

#define	ELFOSABI_SOLARIS	6		/**< Solaris */
#define	ELFOSABI_AIX		7		/**< Aix */
#define	ELFOSABI_IRIX		8		/**< Irix */
#define	ELFOSABI_FREEBSD	9		/**< FreeBSD */
#define	ELFOSABI_TRU64		10		/**< Compaq Tru64 */
#define	ELFOSABI_MODESTO	11		/**< Novell Modesto */
#define	ELFOSABI_OPENBSD	12		/**< Open BSD */



/** \typedef Elf32_Ehdr
    ELF Header.
*/

/** \struct Elf32_Ehdr
    this is the standard elf file header structure,
    at the front of an elf-32 object or executable file
*/

typedef struct Elf32_Ehdr
{
	unsigned char	e_ident[EI_NIDENT] ; 	/**< "\177ELF" class endianness etc. */
	Elf32_Half		e_type; 				      /**< file type, relocatable, exec, shared, core... */
	Elf32_Half		e_machine; 				   /**< architecture type... */
    Elf32_Word		e_version;				   /**< EV_NONE, EV_CURRENT */
    Elf32_Addr		e_entry;				      /**< entry point if executable */
    Elf32_Off		e_phoff;				      /**< file position of program header, or zero */
    Elf32_Off		e_shoff;				      /**< file position of section header, or zero */
	Elf32_Word		e_flags;				      /**< usually zero */
    Elf32_Half 		e_ehsize;				/**< size of this header */
    Elf32_Half		e_phentsize;			   /**< size of entry in program header */
    Elf32_Half		e_phnum;				      /**< number of entries in program header */
    Elf32_Half		e_shentsize;	   		/**< size of entry in section header */
    Elf32_Half		e_shnum;				      /**< number of entries in section header */
    Elf32_Half		e_shstrndx;				   /**< section number that contains section name strings */
} Elf32_Ehdr;


#if	defined(__ELF64__)

/** \typedef Elf64_Ehdr
    ELF Header.
*/

/** \struct Elf64_Ehdr
    this is the standard elf file header structure,
    at the front of an elf-64 object or executable file
*/

typedef struct Elf64_Ehdr
{
	unsigned char	e_ident[EI_NIDENT] ; 	/**< "\177ELF" class endianness etc. */
	Elf64_Half		e_type; 				      /**< file type, relocatable, exec, shared, core... */
	Elf64_Half		e_machine; 				   /**< architecture type... */
    Elf64_Word		e_version;				   /**< EV_NONE, EV_CURRENT */
    Elf64_Addr		e_entry;				      /**< entry point if executable */
    Elf64_Off		e_phoff;				      /**< file position of program header, or zero */
    Elf64_Off		e_shoff;				      /**< file position of section header, or zero */
	Elf64_Word		e_flags;				      /**< usually zero */
    Elf64_Half 		e_ehsize;				/**< size of this header */
    Elf64_Half		e_phentsize;			   /**< size of entry in program header */
    Elf64_Half		e_phnum;				      /**< number of entries in program header */
    Elf64_Half		e_shentsize;			   /**< size of entry in section header */
    Elf64_Half		e_shnum;				      /**< number of entries in section header */
    Elf64_Half		e_shtrndx;				   /**< section number that contains section name strings */
} Elf64_Ehdr;

#endif

/* e_type standard values */

#define ET_NONE		0        /**< None */
#define ET_REL		   1		   /**< Relocatable */
#define ET_EXEC		2		   /**< Executable */
#define ET_DYN		   3	   	/**< Shared object */
#define ET_CORE		4  		/**< Core file */
#define	ET_LOOS		0xfe00	/**< OS system specific low bound */
#define	ET_HIOS		0xfeff	/**< OS system specific high bound */
#define	ET_LOPROC	0xff00	/**< Processor specific low bound */
#define ET_HIPROC	   0xffff	/**< Processor specific high bound */

/* e_machine standard values */

#define EM_NONE			0     /**< None */
#define EM_M32			   1		/**< ATT WE 32100 */
#define	EM_SPARC		   2		/**< Sparc */
#define	EM_386			3		/**< Intel 80386 */
#define EM_68K			   4		/**< Motorola 68000 */
#define	EM_88K			5		/**< Motorola 88000 */

#define EM_860			   7		/**< Intel 80860 */
#define EM_MIPS			8		/**< MIPS I */
#define	EM_S370			9		/**< IBM S370 */
#define EM_MIPS_RS3_LE	10		/**< MIPS RS3000 little endian */

#define	EM_PARISC		15		/**< HP PA-RISC */

#define	EM_VPP500		17		/**< Fujitsu VPP500 */
#define	EM_SPARC32PLUS	18		/**< Enhanced Sparc */
#define	EM_960			19		/**< Intel 80960 */
#define EM_PPC			   20		/**< Power PC */
#define	EM_PPC64		   21		/**< 64-bit Power PC */
#define	EM_S390			22		/**< IBM S390 */

#define	EM_V800			36		/**< NEC V800 */
#define	EM_FR20			37		/**< Fujitsu FR20 */
#define	EM_RH32			38		/**< TRW RH-32 */
#define	EM_RCE			39		/**< Motorola RCE */
#define	EM_ARM			40		/**< Advanced RISC Machine ARM */
#define	EM_ALPHA		   41		/**< Digital Alpha */
#define	EM_SH			   42		/**< Hitachi SH */
#define	EM_SPARCV9		43		/**< Sparc V9 */
#define	EM_TRICORE		44		/**< Infineon Tricore */
#define	EM_ARC			45		/**< Argonaut RISC core */
#define	EM_H8_300		46		/**< Hitachi H8/300 */
#define	EM_H8_300H		47		/**< Hitachi H8/300H */
#define	EM_H8S			48		/**< Hitachi H8s */
#define	EM_H8_500		49		/**< Hitachi H8/500 */
#define	EM_IA_64		   50		/**< Intel IA-64 */
#define	EM_MIPS_X		51		/**< Stanford MIPS-X */
#define	EM_COLDFIRE		52		/**< Motorola	Coldfire */
#define	EM_68HC12		53		/**< Motorola	68HC12 */
#define	EM_MMA			54		/**< Fujitsu MM accelarator */
#define	EM_PCP			55		/**< Infineon PCP */
#define	EM_NCPU			56		/**< Sony nCPU */
#define	EM_NDR1			57		/**< Denso NDR1 */
#define	EM_STARCORE		58		/**< Motorola Star*Core */
#define	EM_ME16			59		/**< Toyota ME16 */
#define	EM_ST100		   60		/**< STMicroelectronics ST100 processor */
#define	EM_TINYJ		   61		/**< Advanced Logic Corp. TinyJ */
#define	EM_X86_64		62		/**< AMD x86-64 architecture */
#define	EM_PDSP			63		/**< Sony DSP */

#define EM_FX66			66		/**< Infineon FX66 Mcu */
#define	EM_ST9PLUS		67		/**< STMicroelectronics ST9+ Mcu */
#define	EM_ST7			68		/**< STMicroelectronics ST7 Mcu */
#define	EM_68HC16		69		/**< Motorola MC68HC16 */
#define	EM_68HC11		70		/**< Motorola MC68HC11 */
#define	EM_68HC08		71		/**< Motorola MC68HC08 */
#define	EM_68HC05		72		/**< Motorola MC68HC05 */
#define	EM_SVX			73		/**< SGI SVx */
#define	EM_ST19			74		/**< STMicroelectronics ST19 Mcu */
#define	EM_VAX			75		/**< Digital VAX */
#define	EM_CRIS			76		/**< Axis com embedded cpu */
#define	EM_JAVELIN		77		/**< Infineon embedded cpu */
#define	EM_FIREPATH		78		/**< Element 14 DSP */
#define	EM_ZSP			79		/**< LSI Logic DSP */
#define	EM_MMIX			80		/**< Donald Knuth's educationnal 64-bit processor */
#define	EM_HUANY		   81		/**< Havard University Machine Independent object files */
#define	EM_PRISM		   82		/**< SiTera Prism */
#define EM_AVR			   83		/**< Atmel AVR */
#define	EM_FR30			84		/**< Fujitsu FR30 */
#define EM_D10V			85		/**< Mitsubishi D10V */
#define EM_D30V			86		/**< Mitsubishi D30V */
#define	EM_V850			87		/**< Nec V850 */
#define	EM_M32R			88		/**< Mitsubishi M32R */
#define EM_MN10300		89		/**< Matsushita MN10300 */
#define EM_MN10200		90		/**< Matsushita MN10200 */
#define	EM_PJ			   91		/**< picoJava */
#define	EM_OPENRISC		92		/**< OpenRisc */
#define	EM_ARC_A5		93		/**< ARC Cores Tangent-A5 */
#define	EM_XTENSA		94		/**< Tensilica Xtensa Architecture */

/* e_version and e_ident[EI_VERSION] standard values */

#define	EV_NONE			0     /**< e_version = none */
#define	EV_CURRENT		1     /**< e_version = current */

/** \typedef Elf32_Phdr
    Program header.
*/

/** \struct Elf32_Phdr
    program header table structure in an elf-32 file;
    resides at offset 'e_phoff' in the elf object or executable file
*/

typedef struct Elf32_Phdr
{
    Elf32_Word	p_type;			/* entry type */
    Elf32_Off	p_offset;		/* file offset for first byte of entry */
    Elf32_Addr	p_vaddr;		/* virtual address for first byte in memory */
    Elf32_Addr	p_paddr;		/* physical address for first byte in memory */
    Elf32_Word	p_filesz;		/* file size in bytes */
    Elf32_Word	p_memsz;		/* memory image in byte */
    Elf32_Word	p_flags;		/* entry flags */
    Elf32_Word	p_align;		/* entry alignment */
} Elf32_Phdr ;

#if defined(__ELF64__)

/** \typedef Elf64_Phdr
    Program header.
*/

/** \struct Elf64_Phdr
   program header table structure in an elf-64 file;
   resides at offset 'e_phoff' in the elf object or executable file
*/

typedef struct Elf64_Phdr
{
    Elf64_Word	   p_type;		/**< entry type */
    Elf64_Off	   p_offset;	/**< file offset for first byte of entry */
    Elf64_Addr	   p_vaddr;		/**< virtual address for first byte in memory */
    Elf64_Addr	   p_paddr;		/**< physical address for first byte in memory */
    Elf64_Word	   p_filesz;	/**< file size in bytes */
    Elf64_Xword	p_memsz;		/**< memory image in byte */
    Elf64_Xword	p_flags;		/**< entry flags */
    Elf64_Xword	p_align;		/**< entry alignment */
} Elf64_Phdr ;

#endif

/* p_type standard values */

#define PT_NULL 		0           /**< Unknown program type */
#define PT_LOAD 		1           /**< LOAD program type */
#define PT_DYNAMIC	2           /**< DYNAMIC program type */
#define PT_INTERP		3           /**< INTERP program type */
#define PT_NOTE		4           /**< NOTE program type */
#define PT_SHLIB		5           /**< SHLIB program type */
#define PT_PHDR 		6           /**< PHDR program type */

#define	PT_LOOS			0x60000000           /**< LOOS program type */
#define	PT_HIOS			0x6fffffff           /**< HIOS program type */

#define	PT_LOPROC		0x70000000           /**< LOPROC program type */
#define	PT_HIPROC		0x7fffffff           /**< HIPROC program type */

/* p_flags standard values */

#define	PF_X			0x1           /**< execute program flag */
#define	PF_W			0x2           /**< write program flag */
#define	PF_R			0x4           /**< read program flag */

#define	PF_MASKOS		0x0ff00000           /**< MASKOS program flag */
#define	PF_MASKPROC		0xf0000000           /**< MASKPROC program flag */

/** \typedef Elf32_Shdr
    Section header.
*/

/** \struct Elf32_Shdr
   standard elf-32 section header structure,
   found at offset 'e_shoff' in the elf object or executable file
*/

typedef struct Elf32_Shdr
{
    Elf32_Word	   sh_name;		   /**< index into section header string table */
    Elf32_Word 	sh_type;		   /**< section content and semantic SHT_NULL, SHT_PROGBITS, SHT_SYMTAB ... */
    Elf32_Word	   sh_flags;		/**< section flags SECTWRITE, SECTALLOC, SECTEXEC... */
    Elf32_Addr	   sh_addr;		   /**< base address where section has to be located in memory image */
    Elf32_Off	   sh_offset;		/**< first byte of section offset from start of file */
    Elf32_Word	   sh_size;		   /**< section size in bytes */
    Elf32_Word	   sh_link;			/**< link info */
    Elf32_Word	   sh_info;			/**< information */
    Elf32_Word	   sh_addralign;	/**< alignment constraint for base address of section */
    Elf32_Word	   sh_entsize;		/**< if section is an array */
} Elf32_Shdr;


#if	defined(__ELF64__)

/** \typedef Elf64_Shdr
    Section header.
*/

/** \struct Elf64_Shdr
   standard elf-64 section header structure,
   found at offset 'e_shoff' in the elf object or executable file
*/

typedef struct Elf64_Shdr
{
    Elf64_Word	   sh_name;		   /**< index into section header string table */
    Elf64_Word 	sh_type;		   /**< section content and semantic SHT_NULL, SHT_PROGBITS, SHT_SYMTAB ... */
    Elf64_Xword	sh_flags;		/**< section flags SECTWRITE, SECTALLOC, SECTEXEC... */
    Elf64_Addr	   sh_addr;		   /**< base address where section has to be located in memory image */
    Elf64_Off	   sh_offset;		/**< first byte of section offset from start of file */
    Elf64_Xword	sh_size;		   /**< section size in bytes */
    Elf64_Word	   sh_link;		   /**< link info */
    Elf64_Word	   sh_info;		   /**< information */
    Elf64_Xword	sh_addralign;	/**< alignment constraint for base address of section */
    Elf64_Xword	sh_entsize;		/**< if section is an array */
} Elf64_Shdr;

#endif

/* sh_type standard values */

#define SHT_NULL			0	/**< undefined section */
#define SHT_PROGBITS		1	/**< program defined section */
#define SHT_SYMTAB			2	/**< symbol table section */
#define SHT_STRTAB			3	/**< string table section*/
#define SHT_RELA			4	/**< 'rela' relocation type section */
#define SHT_HASH			5	/**< hash table section */
#define SHT_DYNAMIC			6	/**< dynamic linking info section */
#define SHT_NOTE			7	/**< file marking section */
#define SHT_NOBITS			8	/**< unallocated program defined section */
#define SHT_REL				9	/**< 'rel' relocation type section */
#define SHT_SHLIB			10	/**< reserved */
#define SHT_DYNSYM			11	/**< dynamic symbol table section */

#define SHT_INIT_ARRAY		14	/**< init function pointer array section */
#define SHT_FINI_ARRAY		15	/**< exit function pointer array section */
#define SHT_PREINIT_ARRAY	16	/**< preinit function pointer array section */
#define SHT_GROUP			17	/**< section group section */
#define SHT_SYMTAB_SHNDX	18	/**< symtab extended index section */

#define SHT_LOOS			0x60000000	/**< OS specific low bound section*/
#define SHT_HIOS			0x6fffffff	/**< OS specific high bound section*/

#define SHT_LOPROC			0x70000000	/**< processor specific low bound section*/
#define SHT_HIPROC			0x8fffffff	/**< processor specific high bound section*/

#define SHT_LOUSER			0x80000000	/**< user specific low bound section*/
#define SHT_HIUSER			0xffffffff	/**< user specific high bound section*/

/* sh_flags standard values */

#define SHF_WRITE		0x1		/**< writable data during execution */
#define SHF_ALLOC		0x2		/**< occupies memory during execution */
#define SHF_EXECINSTR	0x4		/**< executable instruction in section */

#define SHF_MERGE		0x10	/**< section to be merged */
#define SHF_STRINGS		0x20	/**< strings elements in section */
#define SHF_INFO_LINK	0x40	/**< sh_info should be interpreted as an header table index */
#define SHF_LINK_ORDER	0x80	/**< link order */
#define SHF_OS_NONCONFORMING	0x100 /**< OS-specific processing */
#define SHF_GROUP		0x200 	/**< member of a section group */
#define SHF_TLS			0x400 	/**< Thread Local Storage */
#define SHF_MASKOS		0x0ff00000	/**< OS-specific bits */
#define SHF_MASKPROC	0xf0000000	/**< processor specific bits*/


/* flag word standard values for a section of type SHT_GROUP */

#define GRP_COMDAT		0x1			/**< COMDAT group */
#define GRP_MASKOS		0x0ff00000	/**< OS-specific bits */
#define GRP_MASKPROC	0xf0000000	/**< processor specific bits*/

/* special section index standard values */

#define	SHN_UNDEF		0           /**< undefined */
#define	SHN_LORESERVE	0xff00		/**< lower bound of reserved indexes */

#define	SHN_LOPROC		0xff00		/**< processor specific lower bound indexes */
#define	SHN_HIPROC		0xff1f		/**< processor specific higher bound indexes */

#define	SHN_LOOS		0xff20		/**< OS-specific lower bound indexes */
#define	SHN_HIOS		0xff3f		/**< OS-specific higher bound indexes */

#define	SHN_ABS			0xfff1		/**< absolute section index */
#define	SHN_COMMON		0xfff2		/**< common symbol */
#define	SHN_XINDEX		0xffff		/**< escape value to specify extended section index */
#define	SHN_HIRESERVE	0xffff		/**< higher bound of reserved indexes */

/** \typedef Elf32_Sym
    Symbol table.
*/

/** \struct Elf32_Sym
   the elf-32 symbol table entry structure 
*/

typedef struct Elf32_Sym
{
    Elf32_Word		st_name;		/**< index into symbol string table */
    Elf32_Addr		st_value;		/**< symbol value */
    Elf32_Word		st_size;		/**< symbol size in bytes */
    unsigned char	st_info;		/**< (bind&0xf) << 4 | (type & 0xf) */
    unsigned char	st_other;		/**< used for symbol visibility */
    Elf32_Half		st_shndx;		/**< section number to which symbol is related  */
} Elf32_Sym;


#if defined(__ELF64__)

/** \typedef Elf64_Sym
    Symbol table.
*/

/** \struct Elf64_Sym
    the elf-64 symbol table entry structure
*/

typedef struct Elf64_Sym
{
    Elf64_Word		st_name;		/**< index into symbol string table */
    unsigned char	st_info;		/**< (bind&0xf) << 4 | (type & 0xf) */
    unsigned char	st_other;		/**< used for symbol visibility */
    Elf64_Half		st_shndx;		/**< section number to which symbol is related  */
    Elf64_Addr		st_value;		/**< symbol value */
    Elf64_Xword		st_size;		/**< symbol size in bytes */
} Elf64_Sym;

#endif

/* st_info macros */

#define	ELF_ST_BIND(info)		((info)>>4)
#define	ELF_ST_TYPE(info)		((info)&0xf)
#define	ELF_ST_INFO(bind,type)	(((bind)<<4)|((type)&0xf))

/* st_other macros */

#define	ELF_ST_VISIBILITY(other)	((other)&0x3)

/* symbol table binding standard values */

#define STB_LOCAL		0			/**< local symbol */
#define STB_GLOBAL		1			/**< global symbol */
#define STB_WEAK 		2			/**< weak symbol */

#define STB_LOOS		10			/**< OS-specific low bound symbol */
#define STB_HIOS		12			/**< OS-specific high bound symbol */

#define STB_LOPROC		13			/**< processor specific low bound symbol */
#define STB_HIPROC		15			/**< processor specific high bound symbol */

/* symbol table type standard values */

#define STT_NOTYPE		0			/**< undefined */
#define STT_OBJECT		1			/**< data object */
#define STT_FUNC		2			/**< function */
#define STT_SECTION		3			/**< section */
#define STT_FILE		4			/**< name of source file */
#define	STT_COMMON		5			/**< uninitialized common block */
#define	STT_TLS			6			/**< Thread local storage entity */
#define	STT_LOOS		10			/**< OS-specific symbol type lower bound */
#define	STT_HIOS		12			/**< OS-specific symbol type higher bound */
#define	STT_LOPROC		13			/**< processor specific symbol type lower bound */
#define	STT_HIPROC		15			/**< processor specific symbol type higher bound */

/* Visibility standard values */

#define	STV_DEFAULT		0			/**< default */
#define	STV_INTERNAL	1			/**< internal */
#define	STV_HIDDEN		2			/**< hidden */
#define	STV_PROTECTED	3			/**< protected */

/** \typedef Elf32_Rel
    Relocation entry.
*/

/** \struct Elf32_Rel
   elf-32 'rel' relocation type structure definition
*/

typedef	struct Elf32_Rel
{
	Elf32_Addr	r_offset;			/**< offset from base address of section to apply relocation */
	Elf32_Word	r_info;				/**< relocation information field */
} Elf32_Rel ;

/** \typedef Elf32_Rela
    Relocation entry.
*/

/** \struct Elf32_Rela
   elf-32 'rela' relocation type structure definition
*/

typedef	struct Elf32_Rela
{
	Elf32_Addr	r_offset;			/**< offset from base address of section to apply relocation */
	Elf32_Word	r_info;				/**< relocation information field */
	Elf32_Sword	r_addend;			/**< offset to add to relocation */
} Elf32_Rela ;

#define	ELF32_R_SYM(info)		((info)>>8)
#define	ELF32_R_TYPE(info)		((unsigned char) (info))
#define	ELF32_R_INFO(sym,type)	(((sym)<<8)|((unsigned char)(type)))

#if defined(__ELF64__)

/** \typedef Elf64_Rel
    Relocation entry.
*/

/** \struct Elf64_Rel
   elf-64 'rel' relocation type structure definition
*/

typedef	struct Elf64_Rel
{
	Elf64_Addr	r_offset;			/**< offset from base address of section to apply relocation */
	Elf64_Xword	r_info;				/**< relocation information field */
} Elf64_Rel ;

/** \typedef Elf64_Rela
    Relocation entry.
*/

/** \struct Elf64_Rela
   elf-64 'rela' relocation type structure definition
*/

typedef	struct Elf64_Rela
{
	Elf64_Addr	r_offset;			/**< offset from base address of section to apply relocation */
	Elf64_Xword	r_info;				/**< relocation information field */
	Elf64_Sxword	r_addend;		/**< offset to add to relocation */
} Elf64_Rela ;

#define	ELF64_R_SYM(info)		((info)>>32)
#define	ELF64_R_TYPE(info)		((info)&0xffffffffL)
#define	ELF64_R_INFO(sym,type)	(((sym)<<8)|((type)&0xffffffffL))

#endif /* __ELF64__ */



#endif /* __ELF_X_H__ */
