#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <bfd.h>
#include <string.h>


int log_error_message(enum error_source_t errsource, int errn, char * fmt, ...)
{
	FILE * logfile = stderr;
	va_list ap;
	va_start(ap, fmt);
	
	switch(errsource)
	{
		case SYS_ERROR:
			fprintf(logfile, "SYS_ERROR %s", strerror(errn));
			break;
		case BFD_ERROR:
			fprintf(logfile, "BFD_ERROR %s", bfd_errmsg(errn));
			break;
		case DWARF_ERROR:
			break;
		default:
			fprintf(logfile, "unknown error_source_t variable [%d]. Error code: [%d] Error message:", errsource, errn);
	}
	vfprintf(logfile, fmt, ap);
	va_end(ap);
	
	fprintf(logfile, "\n");
	return 0;
}

int log_message(char * fmt, ...)
{
	FILE * logfile = stderr;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(logfile, fmt, ap);
	va_end(ap);
	fprintf(logfile, "\n");
	return 0;
}