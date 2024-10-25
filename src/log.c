#include "log.h"
#include <stdlib.h>
#include <stdarg.h>

FILE *log_file = NULL;

void _log(LogLevel level, const char *fmt, ...)
{
	if(log_file == NULL)
		log_file = stdout;

	StringBuilder b = log_prefix(level, fmt);
	String fmt_str = builder_to_string(&b);

	char *buffer = malloc(MB(1));
	va_list list;
	va_start(list, fmt);
	int size = vsprintf_s(buffer, MB(1), fmt_str.data, list);
	buffer[size++] = '\n';

	fwrite(buffer, 1, size, log_file);

	free(buffer);
	free_builder(&b);
}

void set_log_handle(FILE *f)
{
	log_file = f;
}

StringBuilder log_prefix(LogLevel level, const char *fmt)
{
	String level_prefixes[] = {
		[LOG_ERROR] = STR_LIT("[ERROR]"),
		[LOG_WARN]  = STR_LIT("[WARN]"),
		[LOG_INFO]  = STR_LIT("[INFO]"),
		[LOG_DEBUG] = STR_LIT("[DEBUG]"),
	};
	StringBuilder b = {};
	push_builder(&b, level_prefixes[level]);
	push_builder(&b, STR_LIT(" "));
	push_builder(&b, make_string(fmt, strlen(fmt)));

	return b;
}

