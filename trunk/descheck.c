#include "descheck.h"

#define DIE_STACK_SIZE 50
static Dwarf_Die die_stack[DIE_STACK_SIZE];

#define PUSH_DIE_STACK(x) { die_stack[indent_level] = x; }
#define POP_DIE_STACK { die_stack[indent_level] = 0; }

#define SPACE(x) { register int i; for (i=0;i<x;i++) putchar(' '); }

static struct dwconf_s config_file_data;
Dwarf_Error err;
Dwarf_Unsigned cu_offset = 0;
static int indent_level = 0;
static int local_symbols_already_began = 1;
static struct esb_s esb_base;

int dense = 0;
int ellipsis = 0;
int dst_format = 0;

void formx_unsigned(Dwarf_Unsigned u, struct esb_s *esbp)
{
	char small_buf[40];
	snprintf(small_buf, sizeof(small_buf),
		 "%llu", (unsigned long long)u);
	esb_append(esbp, small_buf);

}

static void formx_signed(Dwarf_Signed u, struct esb_s *esbp)
{
	char small_buf[40];
	snprintf(small_buf, sizeof(small_buf),
		 "%lld", (long long)u);
	esb_append(esbp, small_buf);
}

/* We think this is an integer. Figure out how to print it.
   In case the signedness is ambiguous (such as on 
   DW_FORM_data1 (ie, unknown signedness) print two ways.
*/
static int formxdata_print_value(Dwarf_Attribute attrib, struct esb_s *esbp,
				      Dwarf_Error * err)
{
	Dwarf_Signed tempsd = 0;
	Dwarf_Unsigned tempud = 0;
	int sres = 0;
	int ures = 0;
	Dwarf_Error serr = 0;
	ures = dwarf_formudata(attrib, &tempud, err);
	sres = dwarf_formsdata(attrib, &tempsd, &serr);
	if(ures == DW_DLV_OK) {
		if(sres == DW_DLV_OK) {
			if(tempud == tempsd) {
	   /* Data is the same value, so makes no difference which
				we print. */
				formx_unsigned(tempud,esbp);
			} else {
				formx_unsigned(tempud,esbp);
				esb_append(esbp,"(as signed = ");
				formx_signed(tempsd,esbp);
				esb_append(esbp,")");
			}
		} else if (sres == DW_DLV_NO_ENTRY) {
			formx_unsigned(tempud,esbp);
		} else /* DW_DLV_ERROR */{
			formx_unsigned(tempud,esbp);
		}
		return DW_DLV_OK;
	} else  if (ures == DW_DLV_NO_ENTRY) {
		if(sres == DW_DLV_OK) {
			formx_signed(tempsd,esbp);
			return sres;
		} else if (sres == DW_DLV_NO_ENTRY) {
			return sres;
		} else /* DW_DLV_ERROR */{
			*err = serr;
			return sres;
		}
	} 
	/* else ures ==  DW_DLV_ERROR */ 
	if(sres == DW_DLV_OK) {
		formx_signed(tempsd,esbp);
	} else if (sres == DW_DLV_NO_ENTRY) {
		return ures;
	} 
	/* DW_DLV_ERROR */
	return ures;
}

int _dwarf_print_one_locdesc(Dwarf_Debug dbg, Dwarf_Locdesc * llbuf,
					 struct esb_s *string_out)
{

	Dwarf_Locdesc *locd;
	Dwarf_Half no_of_ops = 0;
	string op_name;
	int i;
	char small_buf[100];


	if (llbuf->ld_from_loclist) {
		snprintf(small_buf, sizeof(small_buf), "<lowpc=0x%llx>",
			 (unsigned long long) llbuf->ld_lopc);
		esb_append(string_out, small_buf);


		snprintf(small_buf, sizeof(small_buf), "<highpc=0x%llx>",
			 (unsigned long long) llbuf->ld_hipc);
		esb_append(string_out, small_buf);
	}


	locd = llbuf;
	no_of_ops = llbuf->ld_cents;
	for (i = 0; i < no_of_ops; i++) {
		Dwarf_Small op;
		Dwarf_Unsigned opd1, opd2;

	/* local_space_needed is intended to be 'more than big enough'
		for a short group of loclist entries.  */
		char small_buf[100];

		if (i > 0)
			esb_append(string_out, " ");

		op = locd->ld_s[i].lr_atom;
		if (op > DW_OP_nop) {
			fprintf(stderr, "dwarf_op unexpected value");
			return DW_DLV_ERROR;
		}
		op_name = get_OP_name(dbg, op);
		esb_append(string_out, op_name);

		opd1 = locd->ld_s[i].lr_number;
		if (op >= DW_OP_breg0 && op <= DW_OP_breg31) {
			snprintf(small_buf, sizeof(small_buf),
				 "%+lld", (Dwarf_Signed) opd1);
			esb_append(string_out, small_buf);
		} else {
			switch (op) {
				case DW_OP_addr:
					snprintf(small_buf, sizeof(small_buf), " %#llx", opd1);
					esb_append(string_out, small_buf);
					break;
				case DW_OP_const1s:
				case DW_OP_const2s:
				case DW_OP_const4s:
				case DW_OP_const8s:
				case DW_OP_consts:
				case DW_OP_skip:
				case DW_OP_bra:
				case DW_OP_fbreg:
					snprintf(small_buf, sizeof(small_buf),
							" %lld", (Dwarf_Signed) opd1);
					esb_append(string_out, small_buf);
					break;
				case DW_OP_const1u:
				case DW_OP_const2u:
				case DW_OP_const4u:
				case DW_OP_const8u:
				case DW_OP_constu:
				case DW_OP_pick:
				case DW_OP_plus_uconst:
				case DW_OP_regx:
				case DW_OP_piece:
				case DW_OP_deref_size:
				case DW_OP_xderef_size:
					snprintf(small_buf, sizeof(small_buf), " %llu", opd1);
					esb_append(string_out, small_buf);
					break;
				case DW_OP_bregx:
					snprintf(small_buf, sizeof(small_buf), "%llu", opd1);
					esb_append(string_out, small_buf);



					opd2 = locd->ld_s[i].lr_number2;
					snprintf(small_buf, sizeof(small_buf),
							"%+lld", (Dwarf_Signed) opd2);
					esb_append(string_out, small_buf);

					break;
				default:
					break;
			}
		}
	}

	return DW_DLV_OK;
}

int get_small_encoding_integer_and_name(Dwarf_Debug dbg,
		Dwarf_Attribute attrib,
		Dwarf_Unsigned * uval_out,
		char *attr_name,
		string * string_out,
		encoding_type_func val_as_string,
		Dwarf_Error * err)
{
	Dwarf_Unsigned uval = 0;
	char buf[100];		/* The strings are small. */
	int vres = dwarf_formudata(attrib, &uval, err);

	/*if (vres != DW_DLV_OK) {
		Dwarf_Signed sval = 0;

		vres = dwarf_formsdata(attrib, &sval, err);
		if (vres != DW_DLV_OK) {
			if (string_out != 0) {
				snprintf(buf, sizeof(buf),
					 "%s has a bad form.", attr_name);
				*string_out = makename(buf);
			}
			return vres;
		}
		*uval_out = (Dwarf_Unsigned) sval;
	} else {
		*uval_out = uval;
	}
	if (string_out)
		*string_out = val_as_string(dbg, (Dwarf_Half) uval);
*/
	return DW_DLV_OK;

}

static void get_attr_value(Dwarf_Debug dbg, Dwarf_Half tag, Dwarf_Attribute attrib,
			       char **srcfiles, Dwarf_Signed cnt, struct esb_s *esbp)
{
	Dwarf_Half theform;
	string temps;
	Dwarf_Block *tempb;
	Dwarf_Signed tempsd = 0;
	Dwarf_Unsigned tempud = 0;
	int i;
	Dwarf_Half attr;
	Dwarf_Off off;
	Dwarf_Die die_for_check;
	Dwarf_Half tag_for_check;
	Dwarf_Bool tempbool;
	Dwarf_Addr addr = 0;
	int fres;
	int bres;
	int wres;
	int dres;
	Dwarf_Half direct_form = 0;
	char small_buf[100];


	fres = dwarf_whatform(attrib, &theform, &err);
	/* depending on the form and the attribute, process the form */
	if (fres == DW_DLV_ERROR) {
		fprintf(stderr, "dwarf_whatform cannot find attr form");
	} else if (fres == DW_DLV_NO_ENTRY) {
		return;
	}

	dwarf_whatform_direct(attrib, &direct_form, &err);
	/* ignore errors in dwarf_whatform_direct() */


	switch (theform) {
		case DW_FORM_addr:
			bres = dwarf_formaddr(attrib, &addr, &err);
			if (bres == DW_DLV_OK) {
				snprintf(small_buf, sizeof(small_buf), "%#llx",
					 (unsigned long long) addr);
				esb_append(esbp, small_buf);
			} else {
				fprintf(stderr, "addr formwith no addr?!");
			}
			break;
		case DW_FORM_ref_addr:
	/* DW_FORM_ref_addr is not accessed thru formref: ** it is an
			address (global section offset) in ** the .debug_info
			section. */
			bres = dwarf_global_formref(attrib, &off, &err);
			if (bres == DW_DLV_OK) {
				snprintf(small_buf, sizeof(small_buf),
					 "<global die offset %llu>",
					 (unsigned long long) off);
				esb_append(esbp, small_buf);
			} else {
				fprintf(stderr,"DW_FORM_ref_addr form with no reference?!");
			}
			break;
		case DW_FORM_ref1:
		case DW_FORM_ref2:
		case DW_FORM_ref4:
		case DW_FORM_ref8:
		case DW_FORM_ref_udata:
			bres = dwarf_formref(attrib, &off, &err);
			if (bres != DW_DLV_OK) {
				fprintf(stderr,"ref formwith no ref?!");
			}
	/* do references inside <> to distinguish them ** from
			constants. In dense form this results in <<>>. Ugly for
			dense form, but better than ambiguous. davea 9/94 */
			snprintf(small_buf, sizeof(small_buf), "<%llu>", off);
			esb_append(esbp, small_buf);
			break;
		case DW_FORM_block:
		case DW_FORM_block1:
		case DW_FORM_block2:
		case DW_FORM_block4:
			fres = dwarf_formblock(attrib, &tempb, &err);
			if (fres == DW_DLV_OK) {
				for (i = 0; i < tempb->bl_len; i++) {
					snprintf(small_buf, sizeof(small_buf), "%02x",
							*(i + (unsigned char *) tempb->bl_data));
					esb_append(esbp, small_buf);
				}
				dwarf_dealloc(dbg, tempb, DW_DLA_BLOCK);
			} else {
				fprintf(stderr,"DW_FORM_blockn cannot get block\n");
			}
			break;
		case DW_FORM_data1:
		case DW_FORM_data2:
		case DW_FORM_data4:
		case DW_FORM_data8:
			fres = dwarf_whatattr(attrib, &attr, &err);
			if (fres == DW_DLV_ERROR) {
				fprintf(stderr, "FORM_datan cannot get attr");
			} else if (fres == DW_DLV_NO_ENTRY) {
				fprintf(stderr,"FORM_datan cannot get attr");
			} else {
				switch (attr) {
					case DW_AT_ordering:
					case DW_AT_byte_size:
					case DW_AT_bit_offset:
					case DW_AT_bit_size:
					case DW_AT_inline:
					case DW_AT_language:
					case DW_AT_visibility:
					case DW_AT_virtuality:
					case DW_AT_accessibility:
					case DW_AT_address_class:
					case DW_AT_calling_convention:
						case DW_AT_discr_list:	/* DWARF3 */
					case DW_AT_encoding:
					case DW_AT_identifier_case:
					case DW_AT_MIPS_loop_unroll_factor:
					case DW_AT_MIPS_software_pipeline_depth:
					case DW_AT_decl_column:
					case DW_AT_decl_file:
					case DW_AT_decl_line:
					case DW_AT_start_scope:
					case DW_AT_byte_stride:
					case DW_AT_bit_stride:
					case DW_AT_count:
					case DW_AT_stmt_list:
					case DW_AT_MIPS_fde:
						wres = get_small_encoding_integer_and_name(dbg,
								attrib,
								&tempud,
								/* attrname */
								(char *) NULL,
							  	(char **) NULL,
								(encoding_type_func) 0,
								&err);

						if (wres == DW_DLV_OK) {
							snprintf(small_buf, sizeof(small_buf), "%llu",
									tempud);
							esb_append(esbp, small_buf);
							if (attr == DW_AT_decl_file) {
								if (srcfiles && tempud > 0 && tempud <= cnt) {
									/* added by user request */
			    /* srcfiles is indexed starting at 0, but
									DW_AT_decl_file defines that 0 means no
									file, so tempud 1 means the 0th entry in
									srcfiles, thus tempud-1 is the correct
									index into srcfiles.  */
									char *fname = srcfiles[tempud - 1];

									esb_append(esbp, " ");
									esb_append(esbp, fname);
								}
							}
						} else {
							fprintf(stderr,"Cannot get encoding attribute ..");
						}
						break;
					case DW_AT_const_value:
						wres = formxdata_print_value(attrib,esbp, &err);
						if(wres == DW_DLV_OK){
							/* String appended already. */
						} else if (wres == DW_DLV_NO_ENTRY) {
							/* nothing? */
						} else {
							fprintf(stderr,"Cannot get DW_AT_const_value ");
						}
  
		
						break;
					case DW_AT_upper_bound:
					case DW_AT_lower_bound:
					default:
						wres = formxdata_print_value(attrib,esbp, &err);
						if (wres == DW_DLV_OK) {
							/* String appended already. */
						} else if (wres == DW_DLV_NO_ENTRY) {
							/* nothing? */
						} else {
							fprintf(stderr, "Cannot get formsdata..");
						}
						break;
				}
			}
			break;
		case DW_FORM_sdata:
			wres = dwarf_formsdata(attrib, &tempsd, &err);
			if (wres == DW_DLV_OK) {
				snprintf(small_buf, sizeof(small_buf), "%lld", tempsd);
				esb_append(esbp, small_buf);
			} else if (wres == DW_DLV_NO_ENTRY) {
				/* nothing? */
			} else {
				fprintf(stderr,"Cannot get formsdata..");
			}
			break;
		case DW_FORM_udata:
			wres = dwarf_formudata(attrib, &tempud, &err);
			if (wres == DW_DLV_OK) {
				snprintf(small_buf, sizeof(small_buf), "%llu", tempud);
				esb_append(esbp, small_buf);
			} else if (wres == DW_DLV_NO_ENTRY) {
				/* nothing? */
			} else {
				fprintf(stderr, "Cannot get formudata....");
			}
			break;
		case DW_FORM_string:
		case DW_FORM_strp:
			wres = dwarf_formstring(attrib, &temps, &err);
			if (wres == DW_DLV_OK) {
				esb_append(esbp, temps);
			} else if (wres == DW_DLV_NO_ENTRY) {
				/* nothing? */
			} else {
				fprintf(stderr,"Cannot get formstr/p....");
			}

			break;
		case DW_FORM_flag:
			wres = dwarf_formflag(attrib, &tempbool, &err);
			if (wres == DW_DLV_OK) {
				if (tempbool) {
					snprintf(small_buf, sizeof(small_buf), "yes(%d)",
							tempbool);
					esb_append(esbp, small_buf);
				} else {
					snprintf(small_buf, sizeof(small_buf), "no");
					esb_append(esbp, small_buf);
				}
			} else if (wres == DW_DLV_NO_ENTRY) {
				/* nothing? */
			} else {
				fprintf(stderr,"Cannot get formflag/p....");
			}
			break;
		case DW_FORM_indirect:
	/* We should not ever get here, since the true form was
			determined and direct_form has the DW_FORM_indirect if it is
			used here in this attr. */
			esb_append(esbp, get_FORM_name(dbg, theform));
			break;
		default:
			fprintf(stderr,"dwarf_whatform unexpected value");
	}
}

void get_location_list(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Attribute attr,
		  struct esb_s *esbp)
{
	Dwarf_Locdesc *llbuf = 0;
	Dwarf_Locdesc **llbufarray = 0;
	Dwarf_Signed no_of_elements;
	Dwarf_Error err;
	int i;
	int lres = 0;
	int llent = 0;

	lres = dwarf_loclist_n(attr, &llbufarray, &no_of_elements, &err);
	if (lres == DW_DLV_ERROR)
		fprintf(stderr,"dwarf_loclist");
	if (lres == DW_DLV_NO_ENTRY)
		return;

	for (llent = 0; llent < no_of_elements; ++llent) {
		char small_buf[100];

		llbuf = llbufarray[llent];

		if (!dense && llbuf->ld_from_loclist) {
			if (llent == 0) {
				snprintf(small_buf, sizeof(small_buf),
					 "<loclist with %ld entries follows>",
					 (long) no_of_elements);
				esb_append(esbp, small_buf);
			}
			esb_append(esbp, "\n\t\t\t");
			snprintf(small_buf, sizeof(small_buf), "[%2d]", llent);
			esb_append(esbp, small_buf);
		}
		lres = _dwarf_print_one_locdesc(dbg, llbuf, esbp);
		if (lres == DW_DLV_ERROR) {
			return;
		} else {
	    /* DW_DLV_OK so we add follow-on at end, else is
			DW_DLV_NO_ENTRY (which is impossible, treat like
			DW_DLV_OK). */
		}
	}
	for (i = 0; i < no_of_elements; ++i) {
		dwarf_dealloc(dbg, llbufarray[i]->ld_s, DW_DLA_LOC_BLOCK);
		dwarf_dealloc(dbg, llbufarray[i], DW_DLA_LOCDESC);
	}
	dwarf_dealloc(dbg, llbufarray, DW_DLA_LIST);
}

static void print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr,
				Dwarf_Attribute attr_in,
				int print_information,
				char **srcfiles, Dwarf_Signed cnt)
{
	Dwarf_Attribute attrib = 0;
	Dwarf_Unsigned uval = 0;
	char * atname = 0;
	char * valname = 0;
	int tres = 0;
	Dwarf_Half tag = 0;

	atname = get_AT_name(dbg, attr);

    /* the following gets the real attribute, even in the ** face of an 
	incorrect doubling, or worse, of attributes */
	attrib = attr_in;
    /* do not get attr via dwarf_attr: if there are (erroneously) **
	multiple of an attr in a DIE, dwarf_attr will ** not get the
	second, erroneous one and dwarfdump ** will print the first one
	multiple times. Oops. */

	tres = dwarf_tag(die, &tag, &err);
	if (tres == DW_DLV_ERROR) {
		tag = 0;
	} else if (tres == DW_DLV_NO_ENTRY) {
		tag = 0;
	} else {
		/* ok */
	}
	
	switch (attr) {
		case DW_AT_language:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_language", &valname,
					get_LANG_name, &err);
			break;
		case DW_AT_accessibility:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_accessibility",
					&valname, get_ACCESS_name,
					&err);
			break;
		case DW_AT_visibility:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_visibility",
					&valname, get_VIS_name,
					&err);
			break;
		case DW_AT_virtuality:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_virtuality",
					&valname,
					get_VIRTUALITY_name, &err);
			break;
		case DW_AT_identifier_case:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_identifier",
					&valname, get_ID_name,
					&err);
			break;
		case DW_AT_inline:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_inline", &valname,
					get_INL_name, &err);
			break;
		case DW_AT_encoding:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_encoding", &valname,
					get_ATE_name, &err);
			break;
		case DW_AT_ordering:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_ordering", &valname,
					get_ORD_name, &err);
			break;
		case DW_AT_calling_convention:
			get_small_encoding_integer_and_name(dbg, attrib, &uval,
					"DW_AT_calling_convention",
					&valname, get_CC_name,
					&err);
			break;
			case DW_AT_discr_list:	/* DWARF3 */
				get_small_encoding_integer_and_name(dbg, attrib, &uval,
						"DW_AT_discr_list",
						&valname, get_DSC_name,
						&err);
				break;
		case DW_AT_location:
		case DW_AT_data_member_location:
		case DW_AT_vtable_elem_location:
		case DW_AT_string_length:
		case DW_AT_return_addr:
		case DW_AT_use_location:
		case DW_AT_static_link:
		case DW_AT_frame_base:
			/* value is a location description or location list */
			esb_empty_string(&esb_base);
			get_location_list(dbg, die, attrib, &esb_base);
			valname = esb_get_string(&esb_base);
			break;
		default:
			esb_empty_string(&esb_base);
			get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
			valname = esb_get_string(&esb_base);
			break;
	}
	if (print_information) {
		if (dense)
			printf(" %s<%s>", atname, valname);
		else
			printf("\t\t%-28s%s\n", atname, valname);
	}
}

void print_one_die(Dwarf_Debug dbg, Dwarf_Die die, int print_information,
			      char **srcfiles, Dwarf_Signed cnt)
{
	Dwarf_Signed i;
	Dwarf_Off offset, overall_offset;
	char * tagname;
	Dwarf_Half tag;
	Dwarf_Signed atcnt;
	Dwarf_Attribute *atlist;
	int tres;
	int ores;
	int atres;

	tres = dwarf_tag(die, &tag, &err);
	if (tres != DW_DLV_OK) {
		fprintf(stderr, "accessing tag of die!");
	}
	tagname = get_TAG_name(dbg, tag);
	ores = dwarf_dieoffset(die, &overall_offset, &err);
	if (ores != DW_DLV_OK) {
		fprintf(stderr, "dwarf_dieoffset");
	}
	ores = dwarf_die_CU_offset(die, &offset, &err);
	if (ores != DW_DLV_OK) {
		fprintf(stderr, "dwarf_die_CU_offset");
	}

	if (!dst_format && print_information) {
		if (indent_level == 0) {
			if (dense)
				printf("\n");
			else {
				printf
						("\nCOMPILE_UNIT<header overall offset = %llu>:\n",
						overall_offset - offset);
			}
		} else if (local_symbols_already_began == 0 &&
					 indent_level == 1 && !dense) {
						 printf("\nLOCAL_SYMBOLS:\n");
						 local_symbols_already_began = 1;
					 }
					 if (dense) {
						 SPACE(2 * indent_level);
						 if (indent_level == 0) {
							 printf("<%d><%llu+%llu><%s>", indent_level,
									 overall_offset - offset, offset, tagname);
						 } else {
							 printf("<%d><%llu><%s>", indent_level, offset, tagname);
						 }
					 } else {
						 printf("<%d><%5llu>\t%s\n", indent_level, offset, tagname);
					 }
	}

	atres = dwarf_attrlist(die, &atlist, &atcnt, &err);
	if (atres == DW_DLV_ERROR) {
		fprintf(stderr, "dwarf_attrlist");
	} else if (atres == DW_DLV_NO_ENTRY) {
		/* indicates there are no attrs.  It is not an error. */
		atcnt = 0;
	}


	for (i = 0; i < atcnt; i++) {
		Dwarf_Half attr;
		int ares;

		ares = dwarf_whatattr(atlist[i], &attr, &err);
		if (ares == DW_DLV_OK) {
			print_attribute(dbg, die, attr,
					atlist[i],
					print_information, srcfiles, cnt);
		} else {
			fprintf(stderr, "dwarf_whatattr entry missing");
		}
	}

	for (i = 0; i < atcnt; i++) {
		dwarf_dealloc(dbg, atlist[i], DW_DLA_ATTR);
	}
	if (atres == DW_DLV_OK) {
		dwarf_dealloc(dbg, atlist, DW_DLA_LIST);
	}

	if (dense && print_information) {
		printf("\n\n");
	}
	return;
}

void print_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,
				       char **srcfiles, Dwarf_Signed cnt)
{
	Dwarf_Die child;
	Dwarf_Die sibling;
	Dwarf_Error err;
	int tres;
	int cdres;
	Dwarf_Die in_die = in_die_in;

	for (;;) {
		PUSH_DIE_STACK(in_die);

		/* here to pre-descent processing of the die */
		print_one_die(dbg, in_die, 1, srcfiles, cnt);

		cdres = dwarf_child(in_die, &child, &err);
		/* child first: we are doing depth-first walk */
		if (cdres == DW_DLV_OK) {
			indent_level++;
			//print_die_and_children(dbg, child, srcfiles, cnt);
			indent_level--;
			if (indent_level == 0)
				local_symbols_already_began = 0;
			dwarf_dealloc(dbg, child, DW_DLA_DIE);
		} else if (cdres == DW_DLV_ERROR) {
			fprintf(stderr, "dwarf_child");
		}

		cdres = dwarf_siblingof(dbg, in_die, &sibling, &err);
		if (cdres == DW_DLV_OK) {
			/*print_die_and_children(dbg, sibling, srcfiles, cnt); /*We
			loop around to actually print this, rather than
			recursing. Recursing is horribly wasteful of stack
			space. */
		} else if (cdres == DW_DLV_ERROR) {
			fprintf(stderr, "dwarf_siblingof");
		}

		/* Here do any post-descent (ie post-dwarf_child) processing of 
		the in_die. */

		POP_DIE_STACK;
		if (in_die != in_die_in) {
		/* Dealloc our in_die, but not the argument die, it belongs 
				to our caller. Whether the siblingof call worked or not. 
		*/
			dwarf_dealloc(dbg, in_die, DW_DLA_DIE);
		}
		if (cdres == DW_DLV_OK) {
			/* Set to process the sibling, loop again. */
			in_die = sibling;
		} else {
			/* We are done, no more siblings at this level. */

			break;
		}
	}/* end for loop on siblings */
	return;
}

static void print_infos(Dwarf_Debug dbg)
{
	Dwarf_Unsigned cu_header_length = 0;
	Dwarf_Unsigned abbrev_offset = 0;
	Dwarf_Half version_stamp = 0;
	Dwarf_Half address_size = 0;
	Dwarf_Die cu_die = 0;
	Dwarf_Unsigned next_cu_offset = 0;
	int nres = DW_DLV_OK;
	int sres;

	while ((nres = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,
					    &abbrev_offset, &address_size,
					    &next_cu_offset, &err)) == DW_DLV_OK) 
	{
		/* process a single compilation unit in .debug_info. */
		sres = dwarf_siblingof(dbg, NULL, &cu_die, &err);
		if (sres == DW_DLV_OK) {
			Dwarf_Signed cnt = 0;
			char **srcfiles = 0;
			int srcf = dwarf_srcfiles(cu_die, &srcfiles, &cnt, &err);

			if (srcf != DW_DLV_OK) {
				srcfiles = 0;
				cnt = 0;
			}

			print_die_and_children(dbg, cu_die, srcfiles, cnt);

			if (srcf == DW_DLV_OK) {
				int si;

				for (si = 0; si < cnt; ++si) {
					dwarf_dealloc(dbg, srcfiles[si], DW_DLA_STRING);
				}
				dwarf_dealloc(dbg, srcfiles, DW_DLA_LIST);
			}
			//print_line_numbers_this_cu(dbg, cu_die);
			dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
		} else if (sres == DW_DLV_NO_ENTRY) {
			// do nothing I guess. 
		} else {
			printf("Regetting cu_die\n");
		}
		cu_offset = next_cu_offset;
	}
}

static int process_one_file(Elf * elf, char *file_name, int archive,
				 struct dwconf_s *config_file_data)
{
	Dwarf_Debug dbg;
	int dres;

	dres = dwarf_elf_init(elf, DW_DLC_READ, NULL, NULL, &dbg, &err);
	if (dres == DW_DLV_NO_ENTRY) 
	{
		printf("No DWARF information present in %s\n", file_name);
		return 0;
	}
	if (dres != DW_DLV_OK) 
	{
		fprintf(stderr, "dwarf_elf_init");
	}

	if (archive) 
	{
		Elf_Arhdr *mem_header = elf_getarhdr(elf);
	
		printf("\narchive member \t%s\n",
		       mem_header ? mem_header->ar_name : "");
	}
	
	dwarf_set_frame_rule_inital_value(dbg, config_file_data->cf_initial_rule_value);
	dwarf_set_frame_rule_table_size(dbg, config_file_data->cf_regs_table_size);
	
	print_infos(dbg);
	
	dres = dwarf_finish(dbg, &err);
	
	return 0;
}

int main(int argc, char **argv)
{
	int f;
	Elf_Cmd cmd;
	Elf *arf, *elf;
	int archive = 0;
	
	elf_version(EV_NONE);
	if (elf_version(EV_CURRENT) == EV_NONE)
	{
		fprintf(stderr, "libelf.a out of date.\n");
		exit(1);
	}
	
	if (argc != 2)
	{
		fprintf(stderr, "Invalid args no %d.\n", argc);
		exit(1);
	}
	
	f = open(argv[1], O_RDONLY);
	if (f == -1)
	{
		fprintf(stderr, "Could not open file %s.\n", argv[1]);
		exit(1);
	}
	
	cmd = ELF_C_READ;
	arf = elf_begin(f, cmd, (Elf *) 0);
	if (elf_kind(arf) == ELF_K_AR) 
	{
		archive = 1;
	}
	
	while ((elf = elf_begin(f, cmd, arf)) != 0) 
	{
		Elf32_Ehdr *eh32;

#ifdef HAVE_ELF64_GETEHDR
		Elf64_Ehdr *eh64;
#endif /* HAVE_ELF64_GETEHDR */
		eh32 = elf32_getehdr(elf);
		if (!eh32) {
#ifdef HAVE_ELF64_GETEHDR
			/* not a 32-bit obj */
			eh64 = elf64_getehdr(elf);
			if (!eh64) {
				/* not a 64-bit obj either! */
				/* dwarfdump is quiet when not an object */
			} else {
				process_one_file(elf, argv[1], archive,
						&config_file_data);
			}
#endif /* HAVE_ELF64_GETEHDR */
		} else {
			process_one_file(elf, argv[1], archive,
					 &config_file_data);
		}
		cmd = elf_next(elf);
		elf_end(elf);
	}
	elf_end(arf);
	
	return 1;
}
