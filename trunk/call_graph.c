/*
* Description: Dissasemble the object binary and create a call graph
*/
#include "call_graph.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bfd.h>
#include <dis-asm.h>


#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define bfd_get_section_size_before_reloc bfd_get_section_size
#endif

// #############################################################################
//				Instruction list
// #############################################################################
struct instrnode_t
{
	int instrlen;
	struct disassemble_info disasm_info;
	struct instrnode_t * next;
	struct instrnode_t * prev;	
};
struct instrlist_t
{
	struct instrnode_t * head;
	struct instrnode_t * tail;
};
typedef struct instrlist_t instrlist_t;

int instrlist_create(instrlist_t ** pilist)
{
	*pilist = malloc(sizeof(instrlist_t));
	if(NULL == *pilist)
	{
		log_error_message(SYS_ERROR, errno, "Cannot allocate memory for instrlist_t");
		return -1;
	}
	(*pilist)->head = (*pilist)->tail = NULL;
	return 0;
}

int instrlist_add(instrlist_t * ilist, int instrlen, struct disassemble_info * pdisasm_info)
{
	struct instrnode_t * pinode = malloc(sizeof(struct instrnode_t));
	if(NULL == pinode)
	{
		log_error_message(SYS_ERROR, errno, "Cannot allocate memory for instrnode_t in instrlist_add()");
		return -1;
	}
	pinode->instrlen = instrlen;
	memcpy(&(pinode->disasm_info), pdisasm_info, sizeof(struct disassemble_info));
	pinode->prev = ilist->tail;
	pinode->next = NULL;
	
	
	// connect it to the list
	if(NULL == ilist->head)
	{
		ilist->head = ilist->tail = pinode;
	}
	else
	{
		ilist->tail->next = pinode;
		ilist->tail = pinode;
	}
	return 0;
}

void instrlist_destroy(instrlist_t * ilist)
{
	struct instrnode_t * pinode;
	if(NULL == ilist)
		return;
	pinode = ilist->head;
	while(NULL != pinode)
	{
		ilist->head = ilist->head->next;
		free(pinode);
		pinode = ilist->head;
	}
	free(ilist);
}


struct call_graph_t
{
	struct call_graph_t * next;
	int from;
	int to;
};


// #############################################################################
//				Dissasemble
// #############################################################################


static int get_text_section(const char * obj_fname, bfd ** pabfd, asection **ptext)
{
	bfd * abfd;
	*pabfd = abfd = bfd_openr (obj_fname, "default");
	if (NULL == abfd)
	{
		log_error_message(BFD_ERROR, bfd_get_error(), "error opening file %s", obj_fname);
		return -1;
	}
	bfd_check_format (abfd, bfd_object);
	*ptext = bfd_get_section_by_name (abfd, ".text");
	if(NULL == *ptext)
	{
		log_error_message(BFD_ERROR, bfd_get_error(), "error searching for .text section");
		return -1;
	}
	return 0;	
}

static int get_symbol_table(bfd * abfd, asection * text_section, asymbol ***psymtab, long * pnsymtab, arelent ***preltab, long * pnreltab)
{
	long size;
	size = bfd_get_symtab_upper_bound (abfd);
	if(size <= 0)
	{
		log_error_message(BFD_ERROR, bfd_get_error(), "could not find symbol table upperbound");
		return -1;
	}
	*psymtab = malloc (size);
	if(NULL == *psymtab)
	{
		log_message("not enough memory to allocate %d bytes for symbol table", size);
		return -1;
	}
	*pnsymtab = bfd_canonicalize_symtab (abfd, *psymtab);

	size = bfd_get_reloc_upper_bound (abfd, text_section);
	if(size <= 0)
	{
		log_error_message(BFD_ERROR, bfd_get_error(), "could not find reloc table upperbound");
		return -1;
	}
	*preltab = malloc (size);
	if(NULL == *preltab)
	{
		log_message("not enough memory to allocate %d bytes for reloc table", size);
		return -1;
	}
	*pnreltab = bfd_canonicalize_reloc (abfd, text_section, *preltab, *psymtab);
	return 0;
}


static int init_disasm_info(bfd * abfd, asection * text_section, struct disassemble_info * pdisasm_info)
{
	FILE *fp_null;
	
	enum bfd_endian endianness;
	bfd_byte *text_section_content;
	bfd_size_type text_section_content_size;
	
	// for disassembler to dispose all output to /dev/null
	if (NULL == (fp_null = fopen ("/dev/null", "w")))
	{
		log_error_message(SYS_ERROR, errno, "error opening /dev/null to redirect output");
		return -1;
	}
	
	INIT_DISASSEMBLE_INFO (*pdisasm_info, stdout, fprintf);
	//INIT_DISASSEMBLE_INFO (*pdisasm_info, fp_null, fprintf);
	
	//detect endianness
	if (bfd_big_endian (abfd))
	{
		endianness = BFD_ENDIAN_BIG;
	}
	else if (bfd_little_endian (abfd))
	{
		endianness = BFD_ENDIAN_LITTLE;
	}
	else
	{
		log_message("Could not detect either BIG not LITTLE endianness");
		return -1;
	}
	
	
	//begin init disasm_info
	pdisasm_info->flavour 			= bfd_get_flavour (abfd);
	pdisasm_info->arch 			= bfd_get_arch (abfd);
	pdisasm_info->mach 			= bfd_get_mach (abfd);
	pdisasm_info->disassembler_options 	= NULL;
	pdisasm_info->octets_per_byte 		= bfd_octets_per_byte (abfd);
	pdisasm_info->insn_info_valid 		= 1;
	pdisasm_info->display_endian 		= endianness;
	pdisasm_info->endian 			= endianness;
	pdisasm_info->skip_zeroes 		= 0;
	pdisasm_info->skip_zeroes_at_end 	= 0;
	pdisasm_info->disassembler_needs_relocs	= FALSE;
	
	disassemble_init_for_target(pdisasm_info);
	
	
	// get the .text section
	text_section_content_size = bfd_get_section_size_before_reloc (text_section);
	text_section_content = malloc (text_section_content_size);
	if(NULL == text_section_content)
	{
		log_error_message(SYS_ERROR, ENOMEM, "Could not allocate space for section contents");
		return -1;
	}
	bfd_get_section_contents (abfd, text_section, text_section_content, 0, text_section_content_size);

	
	pdisasm_info->buffer 		= text_section_content;
	pdisasm_info->buffer_vma 	= text_section->vma;
	pdisasm_info->buffer_length 	= text_section_content_size;
	pdisasm_info->section 		= text_section;
	return 0;
}

static void print_instr(enum dis_insn_type  instype)
{
	switch(instype)
	{
		case dis_noninsn: 
			//printf("dis_noninsn");
			break;
		case dis_nonbranch: 
			printf("dis_nonbranch");
			break;
		case dis_branch: 
			printf("dis_branch");
			break;
		case dis_condbranch: 
			printf("dis_condbranch");
			break;
		case dis_jsr: 
			printf("dis_jsr");
			break;
		case dis_condjsr: 
			printf("dis_condjsr");
			break;
		case dis_dref: 
			printf("dis_dref");
			break;
		case dis_dref2: 
			printf("dis_dref2");
			break;
		default:
			printf("!!!!!!!!! WTF?");
			break;
	};
	printf("\n");
}

static int disassemble_create_instrlist(bfd * abfd, asection * text_section, instrlist_t ** pilist)
{
	struct disassemble_info disasm_info;
	disassembler_ftype disassemble_fn;
	int insn_len = 0;
	bfd_vma  p;
	int rc;
	
	rc = instrlist_create(pilist);
	if(0 != rc)
	{
		return -1;
	}
	
	rc = init_disasm_info(abfd, text_section, &disasm_info);
	if(0 != rc)
	{
		return -1;
	}
	
	disassemble_fn = disassembler (abfd);
	if(NULL == disassemble_fn)
	{
		log_error_message(BFD_ERROR, bfd_get_error(), "could not get a disassembler function");
		return -1;
	} 
	
	
	for (p = disasm_info.buffer_vma; p < disasm_info.buffer_vma + disasm_info.buffer_length; p += insn_len)
	{
		insn_len = disassemble_fn (p, &disasm_info);
		if(0 == disasm_info.insn_info_valid) //only for those architectures that support it
		{
			log_message("Found an invalid instruction at %d from the begining of the .text section.", p - disasm_info.buffer_vma);
			return -1;
		}
		print_instr(disasm_info.insn_type);
		rc = instrlist_add(*pilist, insn_len, &disasm_info);
		if(0 != rc)
		{
			log_message("Error while adding an instuction from addr %d to the list.", p - disasm_info.buffer_vma);
			instrlist_destroy(*pilist);
			*pilist = NULL;
			return -1;
		}
	}
	return 0;
}

static int scan_instrnode(instrlist_t * ilist, struct instrnode_t * inode)
{
	switch (inode->disasm_info.insn_type)
	{
	case dis_jsr:		// jump to subroutine
	case dis_condjsr:	// conditional jump to subroutine
		printf("found a jump to sub routine\n");
		break;
	default:
		//the other insns are just ignored
		break;
	}
	return 0;
}

static int scan_instrlist(instrlist_t * ilist)
{
	struct instrnode_t * inode = ilist->head;
	int rc = 0;
	while((NULL != inode) && (0 == rc))
	{
		
		rc = scan_instrnode(ilist, inode);
		inode = inode->next;
	}
	return rc;
}

int compute_call_graph(const char * obj_fname, struct call_graph_t ** pfunc_calls)
{
	int rc = 0;
	long nsymtab, nreltab;
	asection * text_section;
	asymbol **symtab = NULL;
	arelent **reltab = NULL;
	bfd *abfd = NULL;
	instrlist_t * ilist = NULL;
	
	bfd_init();
	if(0 == rc)
	{
		rc = get_text_section(obj_fname, &abfd, &text_section);
	}
	if(0 == rc)
	{
		rc = get_symbol_table(abfd, text_section, &symtab, &nsymtab, &reltab, &nreltab);
	}
	
	if(0 == rc)
	{
		rc = disassemble_create_instrlist(abfd, text_section, &ilist);
	}
	if(0 == rc)
	{
		rc = scan_instrlist (ilist);
		printf("compute_call_graph\n");
	}
	
	instrlist_destroy(ilist);
	//TODO: deep-free reltab and symtab
	free(reltab);
	free(symtab);
	//TODO:free: text_section, abfd
	bfd_close(abfd);
	return 0;
}


int print_call_graph(struct call_graph_t *gr)
{
	printf("print_call_graph\n");
	return 0;
}

void free_call_graph(struct call_graph_t *gr)
{
	printf("free_call_graph\n");
}
