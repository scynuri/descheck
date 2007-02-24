/*
* Description: Dissasemble the object binary and create a call graph
*/
#include "call_graph.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <bfd.h>

struct call_graph_t
{
	struct call_graph_t * next;
	int from;
	int to;
};

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

int compute_call_graph(const char * obj_fname, struct call_graph_t ** pfunc_calls)
{
	int rc = 0;
	long nsymtab, nreltab;
	asection * text_section;
	asymbol **symtab = NULL;
	arelent **reltab = NULL;
	bfd *abfd = NULL;
	
	
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
		//TODO: get the code bscg::process_disasm_code() called here 
		printf("compute_call_graph\n");
	}
	
// 	//TODO: deep-free reltab and symtab
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


