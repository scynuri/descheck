#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <elf.h>
#include <dwarf.h>
#include <libelf.h>
#include <libdwarf.h>

typedef char * string;

struct dwconf_s {
	char *cf_config_file_path;
	char *cf_abi_name;

    /* 2 for old, 3 for frame interface 3. 2 means use the old
	mips-abi-oriented frame interface. 3 means use the new
	DWARF3-capable and configureable-abi interface.

	Now, anyone who revises dwarf.h and libdwarf.h to match their
	abi-of-interest will still be able to use cf_interface_number 2
	as before.  But most folks don't update those header files and
	instead of making *them* configurable we make dwarfdump (and
	libdwarf) configurable sufficiently to print frame information
	sensibly. */
	int cf_interface_number;

	/* The number of table rules , aka columns. For MIPS/IRIX is 66. */
	unsigned long cf_table_entry_count;

    /* Array of cf_table_entry_count reg names. Names not filled in
	from dwarfdump.conf have NULL (0) pointer value. 
	cf_regs_table_size must match size of cf_regs array.
	Set cf_regs_malloced  1  if table was malloced. Set 0
	if static.
    */
	char **cf_regs;
	unsigned long cf_regs_table_size;
	int    cf_regs_malloced; 

    /* The 'default initial value' when intializing a table. for MIPS
	is DW_FRAME_SAME_VAL(1035). For other ISA/ABIs may be
	DW_FRAME_UNDEFINED_VAL(1034). */
	int cf_initial_rule_value;

    /* The number of the cfa 'register'. For cf_interface_number 2 of 
	MIPS this is 0. For other architectures (and anytime using
	cf_interface_number 3) this should be outside the table, a
	special value such as 1036, not a table column at all).  */
	int cf_cfa_reg;
};

struct esb_s {
	string  esb_string; /* pointer to the data itself, or  NULL. */
	size_t  esb_allocated_size; /* Size of allocated data or 0 */
	size_t  esb_used_bytes; /* Amount of space used  or 0 */
};

char * get_TAG_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_children_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_FORM_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_AT_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_OP_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_ATE_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_DS_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_END_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_ACCESS_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_VIS_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_VIRTUALITY_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_LANG_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_ID_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_CC_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_INL_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_ORD_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_DSC_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_LNS_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_LNE_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_MACINFO_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_EH_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_FRAME_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_CHILDREN_name (Dwarf_Debug dbg, Dwarf_Half val);
char * get_ADDR_name (Dwarf_Debug dbg, Dwarf_Half val);

void esb_append(struct esb_s *data, string in_string);
void esb_appendn(struct esb_s *data, string in_string, size_t len);
string esb_get_string(struct esb_s *data);
void esb_empty_string(struct esb_s *data);
size_t esb_string_len(struct esb_s *data);
void esb_constructor(struct esb_s *data);
void esb_destructor(struct esb_s *data);
void esb_alloc_size(size_t size);
size_t esb_get_allocated_size(struct esb_s *data);

typedef string(*encoding_type_func) (Dwarf_Debug dbg, Dwarf_Half val);

extern int dense;
extern int ellipsis;
extern int dst_format;
