/*
* Description: Dissasemble the object binary and create a call graph
*/
#include "call_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <bfd.h>
struct call_graph_t
{
	struct call_graph_t * next;
	int from;
	int to;
};



static int init_symbol_table (const char * obj_fname, bfd ** pabfd, asection **ptext, asymbol ***psymtab, long * pnsymtab, arelent ***preltab, long * pnreltab)
{
	long size;
	bfd * abfd;
	*pabfd = abfd = bfd_openr (obj_fname, "default");
	if (NULL == abfd)
	{
		fprintf (stderr, "Cannot open object file: %s\n", obj_fname);
		return -1;
	}
	bfd_check_format (abfd, bfd_object);
	*ptext = bfd_get_section_by_name (abfd, ".text");
	size = bfd_get_symtab_upper_bound (abfd);
	if(size <= 0)
	{
		return -1;
	}
	*psymtab = malloc (size);
	if(NULL == *psymtab)
	{
		return -1;
	}
	*pnsymtab = bfd_canonicalize_symtab (abfd, *psymtab);

	size = bfd_get_reloc_upper_bound (abfd, *ptext);
	if(size <= 0)
	{
		return -1;
	}
	*preltab = malloc (size);
	if(NULL == *preltab)
	{
		return -1;
	}
	*pnreltab = bfd_canonicalize_reloc (abfd, *ptext, *preltab, *psymtab);
	return 0;
}

int compute_call_graph(const char * obj_fname, struct call_graph_t ** pfunc_calls)
{
	int rc;
	long nsymtab, nreltab;
	asection * text_section;
	asymbol **symtab;
	arelent **reltab;
	bfd *abfd = NULL;
	
	
	bfd_init();
	rc = init_symbol_table(obj_fname, &abfd, &text_section, &symtab, &nsymtab, &reltab, &nreltab);
	if(0 == rc)
	{
		printf("compute_call_graph\n");
	}
	//TODO:free: text_section, abfd
	free(reltab);
	free(symtab);
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


