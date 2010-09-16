
#include "utils_version_id.h"

int utilsVersionInit( utilsVersion * init ) {
	if ( init == NULL ) { return FALSE; }
	utils_print_version = init;
	return TRUE;
}

int utilsVersion_fprintf( FILE * output ) {
	return fprintf( output, "%s %s V%d.%d-%d.%d - %s\n%s\nLast Build: %s %s\n",
			utils_print_version->name,
			utils_print_version->status,
			utils_print_version->major,
			utils_print_version->minor,
			utils_print_version->micro,
			utils_print_version->tag,
			utils_print_version->description,
			utils_print_version->legal,
			utils_print_version->date,
			utils_print_version->time
		);
}

int utilsVersion_snprintf( char * buffer, int size ) {
	return snprintf( buffer, size, "%s %s V%d.%d-%d.%d - %s\n%s\nLast Build: %s %s\n",
			utils_print_version->name,
			utils_print_version->status,
			utils_print_version->major,
			utils_print_version->minor,
			utils_print_version->micro,
			utils_print_version->tag,
			utils_print_version->description,
			utils_print_version->legal,
			utils_print_version->date,
			utils_print_version->time
		);
}

