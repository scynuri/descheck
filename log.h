#ifndef LOG__H__
#define LOG__H__
enum error_source_t
{
	SYS_ERROR,
	BFD_ERROR,
	DWARF_ERROR
};
#ifndef __attribute__
	#define __attribute__(X) 
#endif//__attribute__

int log_error_message(enum error_source_t errsource, int errn, char * fmt, ...)  __attribute__ ((format (printf, 3, 4)));
int log_message(char * fmt, ...)   __attribute__ ((format (printf, 1, 2)));
#endif // LOG__H__

