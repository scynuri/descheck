#include "bfd.h"
#include "demangle.h"

//#include "bfdver.h"
// #include "progress.h"
		#include "bucomm.h"
// #include "dwarf.h"
// #include "budemang.h"
// #include "getopt.h"
// #include "safe-ctype.h"
// #include "dis-asm.h"
// #include "libiberty.h"
// #include "debug.h"
// #include "budbg.h"

#include "objdump.h"


int main2 (int argc, char **argv);

int main2 (int argc, char **argv)
{
// 	int c;
// 	char *target = NULL;
// 	bfd_boolean seenflag = FALSE;

	expandargv (&argc, &argv);
	bfd_init ();
	set_default_bfd_target ();

	
// 	while ((c = getopt_long (argc, argv, "pib:m:M:VvCdDlfaHhrRtTxsSI:j:wE:zgeGW",
// 		long_options, (int *) 0))
// 		       != EOF)
// 	{
// 		switch (c)
// 		{
// 			case 0:
// 				break;		/* We've been given a long option.  */
// 			case 'm':
// 				machine = optarg;
// 				break;
// 			case 'M':
// 				if (disassembler_options)
// 					/* Ignore potential memory leak for now.  */
// 					disassembler_options = concat (disassembler_options, ",",
// 						optarg, NULL);
// 				else
// 					disassembler_options = optarg;
// 				break;
// 			case 'j':
// 				if (only_used == only_size)
// 				{
// 					only_size += 8;
// 					only = xrealloc (only, only_size * sizeof (char *));
// 				}
// 				only [only_used++] = optarg;
// 				break;
// 			case 'l':
// 				with_line_numbers = TRUE;
// 				break;
// 			case 'b':
// 				target = optarg;
// 				break;
// 			case 'C':
// 				do_demangle = TRUE;
// 				if (optarg != NULL)
// 				{
// 					enum demangling_styles style;
// 
// 					style = cplus_demangle_name_to_style (optarg);
// 					if (style == unknown_demangling)
// 						fatal (_("unknown demangling style `%s'"),
// 							optarg);
// 
// 					cplus_demangle_set_style (style);
// 				}
// 				break;
// 			case 'w':
// 				wide_output = TRUE;
// 				break;
// 			case OPTION_ADJUST_VMA:
// 				adjust_section_vma = parse_vma (optarg, "--adjust-vma");
// 				break;
// 			case OPTION_START_ADDRESS:
// 				start_address = parse_vma (optarg, "--start-address");
// 				break;
// 			case OPTION_STOP_ADDRESS:
// 				stop_address = parse_vma (optarg, "--stop-address");
// 				break;
// 			case 'E':
// 				if (strcmp (optarg, "B") == 0)
// 					endian = BFD_ENDIAN_BIG;
// 				else if (strcmp (optarg, "L") == 0)
// 					endian = BFD_ENDIAN_LITTLE;
// 				else
// 				{
// 					non_fatal (_("unrecognized -E option"));
// 					usage (stderr, 1);
// 				}
// 				break;
// 			case OPTION_ENDIAN:
// 				if (strncmp (optarg, "big", strlen (optarg)) == 0)
// 					endian = BFD_ENDIAN_BIG;
// 				else if (strncmp (optarg, "little", strlen (optarg)) == 0)
// 					endian = BFD_ENDIAN_LITTLE;
// 				else
// 				{
// 					non_fatal (_("unrecognized --endian type `%s'"), optarg);
// 					usage (stderr, 1);
// 				}
// 				break;
// 
// 			case 'f':
// 				dump_file_header = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'i':
// 				formats_info = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'I':
// 				add_include_path (optarg);
// 				break;
// 			case 'p':
// 				dump_private_headers = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'x':
// 				dump_private_headers = TRUE;
// 				dump_symtab = TRUE;
// 				dump_reloc_info = TRUE;
// 				dump_file_header = TRUE;
// 				dump_ar_hdrs = TRUE;
// 				dump_section_headers = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 't':
// 				dump_symtab = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'T':
// 				dump_dynamic_symtab = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'd':
// 				disassemble = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'z':
// 				disassemble_zeroes = TRUE;
// 				break;
// 			case 'D':
// 				disassemble = TRUE;
// 				disassemble_all = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'S':
// 				disassemble = TRUE;
// 				with_source_code = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'g':
// 				dump_debugging = 1;
// 				seenflag = TRUE;
// 				break;
// 			case 'e':
// 				dump_debugging = 1;
// 				dump_debugging_tags = 1;
// 				do_demangle = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'W':
// 				dump_dwarf_section_info = TRUE;
// 				seenflag = TRUE;
// 				do_debug_info = 1;
// 				do_debug_abbrevs = 1;
// 				do_debug_lines = 1;
// 				do_debug_pubnames = 1;
// 				do_debug_aranges = 1;
// 				do_debug_ranges = 1;
// 				do_debug_frames = 1;
// 				do_debug_macinfo = 1;
// 				do_debug_str = 1;
// 				do_debug_loc = 1;
// 				break;
// 			case 'G':
// 				dump_stab_section_info = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 's':
// 				dump_section_contents = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'r':
// 				dump_reloc_info = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'R':
// 				dump_dynamic_reloc_info = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'a':
// 				dump_ar_hdrs = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'h':
// 				dump_section_headers = TRUE;
// 				seenflag = TRUE;
// 				break;
// 			case 'H':
// 				usage (stdout, 0);
// 				seenflag = TRUE;
// 			case 'v':
// 			case 'V':
// 				show_version = TRUE;
// 				seenflag = TRUE;
// 				break;
// 
// 			default:
// 				usage (stderr, 1);
// 		}
// 	}
// 
// 	if (show_version)
// 		print_version ("objdump");
// 
// 	if (!seenflag)
// 		usage (stderr, 2);
// 
// 	if (formats_info)
// 		exit_status = display_info ();
// 	else
// 	{
// 		if (optind == argc)
// 			display_file ("a.out", target);
// 		else
// 			for (; optind < argc;)
// 				display_file (argv[optind++], target);
// 	}
// 
// 	END_PROGRESS (program_name);
// */
	return argc+(int)argv+0;
}
