/* https://msdn.microsoft.com/en-us/library/windows/desktop/ms680547(v=vs.85).aspx
MSDOS stub
PE signature
COFF file header
optional header
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "backend.h"

#pragma pack(1)

#define PE_MAGIC "PE\0\0"
#define MAGIC_SIZE 4
#define MAGIC_LOCATOR 0x3C
#define STATE_ID_NORMAL 0x10B
#define STATE_ID_ROM 0x107
#define STATE_ID_PE32PLUS 0x20B

#define IMAGE_FILE_MACHINE_UNKNOWN     0x0 //The contents of this field are assumed to be applicable to any machine type
#define IMAGE_FILE_MACHINE_AM33        0x1d3 //Matsushita AM33
#define IMAGE_FILE_MACHINE_AMD64       0x8664 //x64
#define IMAGE_FILE_MACHINE_ARM         0x1c0 //ARM little endian
#define IMAGE_FILE_MACHINE_ARM64       0xaa64 //ARM64 little endian
#define IMAGE_FILE_MACHINE_ARMNT       0x1c4 //ARM Thumb-2 little endian
#define IMAGE_FILE_MACHINE_EBC         0xebc //EFI byte code
#define IMAGE_FILE_MACHINE_I386        0x14c //Intel 386 or later processors and compatible processors
#define IMAGE_FILE_MACHINE_IA64        0x200 //Intel Itanium processor family
#define IMAGE_FILE_MACHINE_M32R        0x9041 //Mitsubishi M32R little endian
#define IMAGE_FILE_MACHINE_MIPS16      0x266 //MIPS16
#define IMAGE_FILE_MACHINE_MIPSFPU     0x366 //MIPS with FPU
#define IMAGE_FILE_MACHINE_MIPSFPU16   0x466 //MIPS16 with FPU
#define IMAGE_FILE_MACHINE_POWERPC     0x1f0 //Power PC little endian
#define IMAGE_FILE_MACHINE_POWERPCFP   0x1f1 //Power PC with floating point support
#define IMAGE_FILE_MACHINE_R4000       0x166 //MIPS little endian
#define IMAGE_FILE_MACHINE_RISCV32     0x5032 //RISC-V 32-bit address space
#define IMAGE_FILE_MACHINE_RISCV64     0x5064 //RISC-V 64-bit address space
#define IMAGE_FILE_MACHINE_RISCV128    0x5128 //RISC-V 128-bit address space
#define IMAGE_FILE_MACHINE_SH3         0x1a2 //Hitachi SH3
#define IMAGE_FILE_MACHINE_SH3DSP      0x1a3 //Hitachi SH3 DSP
#define IMAGE_FILE_MACHINE_SH4         0x1a6 //Hitachi SH4
#define IMAGE_FILE_MACHINE_SH5         0x1a8 //Hitachi SH5
#define IMAGE_FILE_MACHINE_THUMB       0x1c2 //Thumb
#define IMAGE_FILE_MACHINE_WCEMIPSV2   0x169 //MIPS little-endian WCE v2

// Flags
#define COFF_FLAG_RELOCS_STRIPPED      0x0001 //Does not contain base relocations and must therefore be loaded at its preferred base address
#define COFF_FLAG_SHIFT_RELOCS_STRIPPED 0
#define COFF_FLAG_EXECUTABLE_IMAGE     0x0002 //The image file is valid and can be run
#define COFF_FLAG_SHIFT_EXECUTABLE_IMAGE 1
#define COFF_FLAG_LINE_NUMS_STRIPPED   0x0004 //COFF line numbers have been removed
#define COFF_FLAG_SHIFT_LINE_NUMS_STRIPPED 2
#define COFF_FLAG_LOCAL_SYMS_STRIPPED  0x0008 //COFF symbol table entries for local symbols have been removed
#define COFF_FLAG_SHIFT_LOCAL_SYMS_STRIPPED 3
#define COFF_FLAG_AGGRESSIVE_WS_TRIM   0x0010 //Obsolete. Aggressively trim working set
#define COFF_FLAG_SHIFT_AGGRESSIVE_WS_TRIM 4
#define COFF_FLAG_LARGE_ADDRESS_AWARE  0x0020 //Application can handle > 2-GB addresses.
#define COFF_FLAG_SHIFT_LARGE_ADDRESS 5
                                    //0x0040 This flag is reserved for future use.
#define COFF_FLAG_BYTES_REVERSED_LO    0x0080 //Little endian
#define COFF_FLAG_SHIFT_BYTES_REVERSED_LO 7
#define COFF_FLAG_32BIT_MACHINE        0x0100 //Machine is based on a 32-bit-word architecture.
#define COFF_FLAG_SHIFT_32BIT_MACHINE 8
#define COFF_FLAG_DEBUG_STRIPPED       0x0200 //Debugging information is removed from the image file.
#define COFF_FLAG_SHIFT_DEBUG_STRIPPED 9
#define COFF_FLAG_REMOVABLE_RUN_FROM_SWAP 0x0400 //If the image is on removable media, fully load it and copy it to the swap file.
#define COFF_FLAG_SHIFT_REMOVABLE_RUN_FROM_SWAP 10
#define COFF_FLAG_NET_RUN_FROM_SWAP    0x0800 //If the image is on network media, fully load it and copy it to the swap file.
#define COFF_FLAG_SHIFT_NET_RUN_FROM_SWAP 11
#define COFF_FLAG_SYSTEM               0x1000 //The image file is a system file, not a user program.
#define COFF_FLAG_SHIFT_SYSTEM 12
#define COFF_FLAG_DLL                  0x2000 //The image file is a dynamic-link library (DLL)
#define COFF_FLAG_SHIFT_DLL 13
#define COFF_FLAG_UP_SYSTEM_ONLY       0x4000 //The file should be run only on a uniprocessor machine.
#define COFF_FLAG_SHIFT_UP_SYSTEM_ONLY 14
#define COFF_FLAG_BYTES_REVERSED_HI    0x8000 //Big endian
#define COFF_FLAG_SHIFT_BYTES_REVERSED_HI 15

typedef enum subsystem
{
   IMAGE_SUBSYSTEM_UNKNOWN, //0 An unknown subsystem
   IMAGE_SUBSYSTEM_NATIVE,       //1 Device drivers and native Windows processes
   IMAGE_SUBSYSTEM_WINDOWS_GUI,  //2 The Windows graphical user interface (GUI) subsystem
   IMAGE_SUBSYSTEM_WINDOWS_CUI,  //3 The Windows character subsystem
   IMAGE_SUBSYSTEM_4,
   IMAGE_SUBSYSTEM_OS2_CUI,      //5 The OS/2 character subsystem
   IMAGE_SUBSYSTEM_6,
   IMAGE_SUBSYSTEM_POSIX_CUI,    //7 The Posix character subsystem
   IMAGE_SUBSYSTEM_NATIVE_WINDOWS, //8 Native Win9x driver
   IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, //9 Windows CE
   IMAGE_SUBSYSTEM_EFI_APPLICATION, //10 An Extensible Firmware Interface (EFI) application
   IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, //11 An EFI driver with boot services
   IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, //12 An EFI driver with run-time services
   IMAGE_SUBSYSTEM_EFI_ROM, //13 An EFI ROM image
   IMAGE_SUBSYSTEM_XBOX, //14 XBOX
   IMAGE_SUBSYSTEM_15, //15 unknown
   IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, //16 Windows boot application.
} subsystem;

typedef struct coff_header
{
   unsigned short machine; // see IMAGE_FILE_MACHINE_
   unsigned short num_sections;
   unsigned int  time_created;
   unsigned int  offset_symtab; // offset to symbol table
   unsigned int  num_symbols;   // number of entries in symbol table
   unsigned short size_optional_hdr;
   unsigned short flags;         // see COFF_FLAGS_
} coff_header;

typedef struct optional_header
{
   unsigned short state; // STATE_ID_
   unsigned char major_linker_ver;
   unsigned char minor_linker_ver;
   unsigned int code_size;
   unsigned int data_size;
   unsigned int uninit_data_size;
   unsigned int entry;
   unsigned int base;
   // only in PE32 (not PE32+) unsigned int data_base;
} optional_header;

typedef struct pe32_windows_header
{
   unsigned int base;
   unsigned int section_alignment;
   unsigned int file_alignment;
   unsigned short os_major;
   unsigned short os_minor;
   unsigned short image_major;
   unsigned short image_minor;
   unsigned short subsys_major;
   unsigned short subsys_minor;
   unsigned int win32ver;
   unsigned int image_size;
   unsigned int header_size;
   unsigned int checksum;
   unsigned short subsystem; // see IMAGE_SUBSYSTEM_
   unsigned short dll_chars;
   unsigned int stack_size;
   unsigned int stack_commit_size;
   unsigned int heap_size;
   unsigned int heap_commit_size;
   unsigned int loader_flags;
   unsigned int num_rva;
} pe32_windows_header;

struct machine_name
{
   unsigned short id;
   char name[31+1];
};

static const struct machine_name machine_lookup[] = 
{
   { IMAGE_FILE_MACHINE_UNKNOWN,    "Unknown machine" },
   { IMAGE_FILE_MACHINE_AM33,       "Matsushita AM33" },
   { IMAGE_FILE_MACHINE_AMD64,      "x64" },
   { IMAGE_FILE_MACHINE_ARM,        "ARM little endian" },
   { IMAGE_FILE_MACHINE_ARM64,      "ARM64 little endian" },
   { IMAGE_FILE_MACHINE_ARMNT,      "ARM Thumb-2 little endian" },
   { IMAGE_FILE_MACHINE_EBC,        "EFI byte code" },
   { IMAGE_FILE_MACHINE_I386,       "Intel 386 or later" },
   { IMAGE_FILE_MACHINE_IA64,       "Intel Itanium" },
   { IMAGE_FILE_MACHINE_M32R,       "Mitsubishi M32R little endian" },
   { IMAGE_FILE_MACHINE_MIPS16,     "MIPS16" },
   { IMAGE_FILE_MACHINE_MIPSFPU,    "MIPS with FPU" },
   { IMAGE_FILE_MACHINE_MIPSFPU16,  "MIPS16 with FPU" },
   { IMAGE_FILE_MACHINE_POWERPC,    "Power PC little endian" },
   { IMAGE_FILE_MACHINE_POWERPCFP,  "Power PC with floating point" },
   { IMAGE_FILE_MACHINE_R4000,      "MIPS little endian" },
   { IMAGE_FILE_MACHINE_RISCV32,    "RISC-V 32-bit address space" },
   { IMAGE_FILE_MACHINE_RISCV64,    "RISC-V 64-bit address space" },
   { IMAGE_FILE_MACHINE_RISCV128,   "RISC-V 128-bit address space" },
   { IMAGE_FILE_MACHINE_SH3,        "Hitachi SH3" },
   { IMAGE_FILE_MACHINE_SH3DSP,     "Hitachi SH3 DSP" },
   { IMAGE_FILE_MACHINE_SH4,        "Hitachi SH4" },
   { IMAGE_FILE_MACHINE_SH5,        "Hitachi SH5" },
   { IMAGE_FILE_MACHINE_THUMB,      "Thumb" },
   { IMAGE_FILE_MACHINE_WCEMIPSV2,  "MIPS little-endian WCE v2" },
};

static const char* flags_lookup[] = 
{
   "Does not contain base relocations and must therefore be loaded at its preferred base address",
   "The image file is valid and can be run",
   "COFF line numbers have been removed",
   "COFF symbol table entries for local symbols have been removed",
   "Obsolete. Aggressively trim working set",
   "Application can handle > 2-GB addresses",
   "RESERVED",
   "Little endian",
   "Machine is based on a 32-bit-word architecture",
   "Debugging information is removed from the image file.",
   "If the image is on removable media, fully load it and copy it to the swap file",
   "If the image is on network media, fully load it and copy it to the swap file",
   "The image file is a system file, not a user program",
   "The image file is a dynamic-link library (DLL)",
   "The file should be run only on a uniprocessor machine",
   "Big endian"
};

const char* lookup_machine(unsigned short machine)
{
   for (int i=0; i < sizeof(machine_lookup)/sizeof(struct machine_name); i++)
      if (machine_lookup[i].id == machine)
         return machine_lookup[i].name;
   return machine_lookup[0].name;
}

void dump_coff(coff_header* h)
{
   printf("machine: %s\n", lookup_machine(h->machine));
   printf("num sections: %u\n", h->num_sections);
   time_t creat = h->time_created;
   printf("created: %s", ctime(&creat));
   printf("symtab offset: %u\n", h->offset_symtab);
   printf("num symbols: %u\n", h->num_symbols);
   printf("exe header size: %u\n", h->size_optional_hdr);
   printf("flags: 0x%X\n", h->flags);
   for (int i=0; i < sizeof(h->flags)*8; i++)
      if (h->flags & 1<<i)
         printf("   - %s\n", flags_lookup[i]);
}

void dump_optional(optional_header* h)
{
   printf("state: 0x%x\n", h->state);
   printf("link ver: %i %i\n", h->major_linker_ver, h->minor_linker_ver);
   printf("code size: %i\n", h->code_size);
   printf("data size: %i\n", h->data_size);
   printf("uninit data size: %i\n", h->uninit_data_size);
   printf("entry: 0x%x\n", h->entry);
   printf("base: 0x%x\n", h->base);
}

backend_object* coff_read_file(const char* filename)
{
   char* buff = malloc(sizeof(coff_header));

   FILE* f = fopen(filename, "rb");
   if (!f)
   {
      printf("can't open file\n");
      free(buff);
      return 0;
   }

   // get size of the file
   fseek(f, 0, SEEK_END);
   int fsize = ftell(f);

   if (MAGIC_LOCATOR >= fsize)
   {
      free(buff);
      return 0;
   }

   // read location 0x3C to find the offset of the magic number
   fseek(f, MAGIC_LOCATOR, SEEK_SET);
   fread(buff, MAGIC_SIZE, 1, f);
   if (*(unsigned int*)buff >= fsize)
   {
      free(buff);
      return 0;
   }

   fseek(f, *(unsigned int*)buff, SEEK_SET);
   fread(buff, MAGIC_SIZE, 1, f);
   if (memcmp(buff, PE_MAGIC, 4) != 0)
   {
      free(buff);
      return 0;
   }
   
   printf("found PE magic number\n");
   backend_object* obj = malloc(sizeof(backend_object));
   if (!obj)
   {
      free(buff);
      return 0;
   }

   backend_set_type(obj, OBJECT_TYPE_PE32);
   // read the coff header
   fread(buff, sizeof(coff_header), 1, f);
   dump_coff((coff_header*)buff);
   int optional_header_size = ((coff_header*)buff)->size_optional_hdr;

   // read the optional header
   free(buff);
   buff = malloc(optional_header_size);
   fread(buff, sizeof(optional_header), 1, f);
   dump_optional((optional_header*)buff);
   return obj;
}

backend_ops coff =
{
   .read = coff_read_file
};

void pe_init(void)
{
   backend_register(&coff);
}
