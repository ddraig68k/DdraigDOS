#ifndef DDRAIG_OS_ELF_H
#define DDRAIG_OS_ELF_H

/* These constants are for the segment types stored in the image headers */
#define PT_NULL                 0
#define PT_LOAD                 1
#define PT_DYNAMIC              2
#define PT_INTERP               3
#define PT_NOTE                 4
#define PT_SHLIB                5
#define PT_PHDR                 6
#define PT_TLS                  7

/* These constants define the different elf file types */
#define ET_NONE                 0
#define ET_REL                  1
#define ET_EXEC                 2
#define ET_DYN                  3
#define ET_CORE                 4
#define ET_LOPROC               0xff00
#define ET_HIPROC               0xffff

/* These constants define the various ELF target machines */
#define EM_NONE		            0
#define EM_M32		            1
#define EM_SPARC	            2
#define EM_386		            3
#define EM_68K		            4
#define EM_88K		            5
#define EM_486		            6	    /* Perhaps disused */
#define EM_860		            7
#define EM_MIPS		            8       /* MIPS R3000 (officially, big-endian only) */
				                        /* Next two are historical and binaries and
				                           modules of these types will be rejected by
				                           Linux.  */
#define EM_MIPS_RS3_LE	        10      /* MIPS R3000 little-endian */
#define EM_MIPS_RS4_BE	        10      /* MIPS R4000 big-endian */

#define EM_PARISC	            15	    /* HPPA */
#define EM_SPARC32PLUS	        18	    /* Sun's "v8plus" */
#define EM_PPC		            20	    /* PowerPC */
#define EM_PPC64	            21	    /* PowerPC64 */
#define EM_SPU		            23	    /* Cell BE SPU */
#define EM_ARM		            40	    /* ARM 32 bit */
#define EM_SH		            42	    /* SuperH */
#define EM_SPARCV9	            43	    /* SPARC v9 64-bit */
#define EM_H8_300	            46	    /* Renesas H8/300 */
#define EM_IA_64	            50	    /* HP/Intel IA-64 */
#define EM_X86_64	            62	    /* AMD x86-64 */
#define EM_S390		            22	    /* IBM S/390 */
#define EM_CRIS		            76	    /* Axis Communications 32-bit embedded processor */
#define EM_M32R		            88	    /* Renesas M32R */
#define EM_MN10300	            89	    /* Panasonic/MEI MN10300, AM33 */
#define EM_OPENRISC             92      /* OpenRISC 32-bit embedded processor */
#define EM_ARCOMPACT	        93	    /* ARCompact processor */
#define EM_XTENSA	            94	    /* Tensilica Xtensa Architecture */
#define EM_BLACKFIN             106     /* ADI Blackfin Processor */
#define EM_UNICORE	            110	    /* UniCore-32 */
#define EM_ALTERA_NIOS2	        113 	/* Altera Nios II soft-core processor */
#define EM_TI_C6000	            140	    /* TI C6X DSPs */
#define EM_HEXAGON	            164	    /* QUALCOMM Hexagon */
#define EM_NDS32	            167	    /* Andes Technology compact code size
				                           embedded RISC processor family */
#define EM_AARCH64	            183	    /* ARM 64 bit */
#define EM_TILEPRO	            188	    /* Tilera TILEPro */
#define EM_MICROBLAZE	        189	    /* Xilinx MicroBlaze */
#define EM_TILEGX	            191	    /* Tilera TILE-Gx */
#define EM_ARCV2	            195	    /* ARCv2 Cores */
#define EM_RISCV	            243	    /* RISC-V */
#define EM_BPF		            247	    /* Linux BPF - in-kernel virtual machine */
#define EM_CSKY		            252	    /* C-SKY */
#define EM_FRV		            0x5441	/* Fujitsu FR-V */

#define ID_32BIT                1
#define ID_64BIT                2
#define ID_LITTLE_ENDIAN        1
#define ID_BIG_ENDIAN           2

typedef struct __attribute__ ((__packed__))
{
    unsigned char  ident_magic[4];      /* 0x7f followed by "ELF" */
    unsigned char  ident_class;         /* 32-bit ot 64-bit */
    unsigned char  ident_data;          /* Endianess */
    unsigned char  ident_version;       /* 1 for current version of ELF */
    unsigned char  ident_osabi;         /* Target OS ABI */
    unsigned char  ident_abiversion;    /* OS ABI version */
    unsigned char  padding[7];
    unsigned short type;                /* Object type */
    unsigned short machine;             /* CPU type */
    unsigned long  version;             /* 1 for current version of ELF */
    unsigned long  entry;               /* Memory address of the entry point */
    unsigned long  phoff;               /* Pointer to start of program header table */
    unsigned long  shoff;               /* Pointer to start of section header table */
    unsigned long  flags;               /* Dependent of architecture */
    unsigned short ehsize;              /* Size of this header */
    unsigned short phentsize;           /* Size of the program heater table entry */
    unsigned short phnum;               /* Number of entries in program header table */
    unsigned short shentsize;           /* Size of the section heater table entry */
    unsigned short shnum;               /* Number of entries in section header table */
    unsigned short shtrndx;             /* Index of section header table that contains section names */
} elf32_header;

typedef struct __attribute__ ((__packed__))
{
    unsigned long type;                 /* Segment type */
    unsigned long offset;               /* Segment file offset */
    unsigned long vaddr;                /* Segment virtual address */
    unsigned long paddr;                /* Segment physical address */
    unsigned long filesz;               /* Segment size in file */
    unsigned long memsz;                /* Segment size in memory */
    unsigned long flags;                /* Segment dependent flags */
    unsigned long align;                /* 0 and 1 specify no alignment */
} elf32_program_header;

#endif
