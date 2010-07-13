
#include "utils_option.h"

/** \defgroup utilsOptionValue
* @{
*/

	static utilsOptionValue * utilsOptionValueNew( utilsOptionValue * next, char * argument, char * stop ) {
		utilsOptionValue * more;
		UTILS_MEMORY_NEW( more, 1, utilsOptionValue );
		if ( more != NULL ) {
			more->next = next;
			if ( argument != NULL ) {
				if ( stop != NULL ) {
					assert( argument <= stop );
					UTILS_MEMORY_STRNDUP( more->argument, argument, ( stop - argument ) );
				}
				else {
					UTILS_MEMORY_STRDUP( more->argument, argument );
				}
			}
			else {
				more->argument = NULL;
			}
		}
		return more;
	}

	static void utilsOptionValueDelete( utilsOptionValue * value ) {
		if ( value != NULL ) {
			if ( value->argument != NULL ) {
				free( value->argument );
			}
			utilsOptionValueDelete( value->next );
			free( value );
		}
	}

/** @}
*/

/** \defgroup utilsOption get / set
* @{
*/

static utilsOption * utils_options = NULL;

	static int utilsOptionSetTag( utilsOption * option, char * value ) {
		assert( option->type == UTILS_OPTION_TAG );
		utilsOptionValueDelete( option->value );
		option->value = NULL;
		if ( value != NULL ) {
			option->value = utilsOptionValueNew( NULL, value, NULL );
			if ( option->callback != NULL ) { return ( option->callback )( option, value ); }
		}
		return ( option->value != NULL );
	}

	static int utilsOptionSetToggle( utilsOption * option, char * value ) {
		assert( option->type == UTILS_OPTION_TOGGLE );
		utilsOptionValueDelete( option->value );
		option->value = NULL;
		if ( ( value != NULL ) && ( ( strcmp( value, "yes" ) == 0 ) || ( strcmp( value, "no" ) == 0 ) ) ) {
			option->value = utilsOptionValueNew( NULL, value, NULL );
			if ( option->callback != NULL ) { return ( option->callback )( option, value ); }
		}
		return ( option->value != NULL );
	}

	static int utilsOptionSetScalar( utilsOption * option, char * value ) {
		assert( option->type == UTILS_OPTION_SCALAR );
		utilsOptionValueDelete( option->value );
		option->value = NULL;
		if ( value != NULL ) {
			option->value = utilsOptionValueNew( NULL, value, NULL );
			if ( option->callback != NULL ) { return ( option->callback )( option, value ); }
		}
		return ( option->value != NULL );
	}

	static int utilsOptionSetList( utilsOption * option, char * value ) {
		char * scoot;
		assert( option->type == UTILS_OPTION_LIST );
		do {
			utilsOptionValue * more;
			scoot = value;
			while ( ( * scoot != '\0' ) && ( * scoot != ',' ) ) { scoot ++; }
			more = utilsOptionValueNew( option->value, value, scoot );
			if ( more != NULL ) { option->value = more; }
			if ( option->callback != NULL ) {
				if ( ! ( option->callback )( option, more->argument ) ) {
					return FALSE;
				}
			}
			value = scoot + 1;
		} while ( * scoot != '\0' );
		return TRUE;
	}

	int utilsOptionSet( utilsOption * option, char * value ) {
		switch ( option->type ) {
			case UTILS_OPTION_TAG:    return utilsOptionSetTag( option, value );
			case UTILS_OPTION_TOGGLE: return utilsOptionSetToggle( option, value );
			case UTILS_OPTION_SCALAR: return utilsOptionSetScalar( option, value );
			case UTILS_OPTION_LIST:   return utilsOptionSetList( option, value );
			default: return FALSE;
		}
		// return FALSE;
	}

	int utilsOptionSetByName( utilsOption * options, char * name, char * value ) {
		utilsOption * option = utilsOptionGetByName( options, name );
		if ( option == NULL ) { return FALSE; }
		return utilsOptionSet( option, value );
	}

	utilsOption * utilsOptionGetByName( utilsOption * options, char * name ) {
		utilsOption * walker = options ? options : utils_options;
		if ( name == NULL ) { return NULL; }
		while ( walker != NULL ) {
			if ( strcmp( walker->name, name ) == 0 ) {
				return walker;
			}
			walker = walker->next;
		}
		return NULL;
	}

	int utilsOptionIsDefined( utilsOption * option ) {
		return ( ( option != NULL ) && ( option->value != NULL ) && ( option->value->argument != NULL ) );
	}

	int utilsOptionValueCount( utilsOption * option ) {
		int count = 0;
		utilsOptionValue * walker = option->value;
		while ( walker != NULL ) {
			if ( walker->argument != NULL ) {
				count += 1;
			}
			walker = walker->next;
		}
		return count;
	}

	int utilsOptionGetTag( utilsOption * option ) {
		assert( option->type == UTILS_OPTION_TAG );
		return ( utilsOptionIsDefined( option ) );
	}

	int utilsOptionGetToggle( utilsOption * option ) {
		assert( option->type == UTILS_OPTION_TOGGLE );
		return ( utilsOptionIsDefined( option ) && ( * option->value->argument == 'y' ) );
	}

	char * utilsOptionGetScalar( utilsOption * option ) {
		assert( option->type == UTILS_OPTION_SCALAR );
		if ( ! utilsOptionIsDefined( option ) ) { return NULL; }
		return option->value->argument;
	}

	utilsOptionValue * utilsOptionGetList( utilsOption * option ) {
		assert( option->type == UTILS_OPTION_LIST );
		if ( ! utilsOptionIsDefined( option ) ) { return NULL; }
		return option->value;
	}

	utilsOptionValue * utilsOptionGetValue( utilsOption * option, char * value ) {
		utilsOptionValue * walker = NULL;
		assert( ( option->type == UTILS_OPTION_SCALAR ) || ( option->type == UTILS_OPTION_LIST ) );
		if ( value != NULL ) {
			walker = option->value;
			while ( ( walker != NULL ) && ( strcmp( walker->argument, value ) != 0 ) ) {
				walker = walker->next;
			}
		}
		return walker;
	}

/** @}
*/

/** \defgroup utilsOption privates
* @{
*/

	static char * utilsOptionSkipPrefix( char * argument ) {
		while ( * argument == '-' ) { argument ++; }
		return argument;
	}

	static int utilsOptionLooksLikeAnOption( char * argument ) {
		if ( * argument != '-' ) { return FALSE; }
		argument = utilsOptionSkipPrefix( argument );
		return ( ! isdigit( ( int )( * argument ) ) );
	}

	static int utilsOptionStop( int index, int * argc, char ** argv ) {
		if ( index < * argc ) {
			char * argument = utilsOptionSkipPrefix( argv[index] );
			return ( * argument == '\0' );
		}
		return TRUE;
	}

	static int utilsOptionReset( utilsOption * options ) {
		utilsOption * walker = options ? options : utils_options;
		while ( walker != NULL ) {
			if ( walker->value != NULL ) {
				utilsOptionValueDelete( walker->value );
			}
			if ( walker->init != NULL ) {
				if ( 0 == ( walker->type & ( UTILS_OPTION_OPEN | UTILS_OPTION_CLOSE ) ) ) {
					if ( ! utilsOptionSet( walker, walker->init ) ) {
						utilsPrintInternalError( "UTILS", "Cannot reset options '%s' value '%s'", walker->name, walker->init );
						return FALSE;
					}
				}
			}
			else {
				walker->value = NULL;
			}
			walker->flags &= ~ UTILS_OPTION_DONE;
			walker = walker->next;
		}
		return TRUE;
	}

	static void utilsOptionPack( int pack, int index, int * argc, char ** argv ) {
		int from = index + pack;
		int to   = index;
		while ( from < * argc ) {
			argv[ to ] = argv[ from ];
			from += 1;
			to += 1;
		}
		while ( to < * argc ) {
			argv[ to ] = NULL;
			to += 1;
		}
		* argc -= pack;
	}

	static int utilsOptionStringMatchCaseInsensitive( char * reference, char * target ) {
		char * walker = target;
		while ( ( * walker != '\0' ) && ( * walker != '=' ) && ( * reference != '\0' ) && ( toupper( * walker ) == toupper( * reference ) ) ) {
			walker ++;
			reference ++;
		}
		if ( * walker == '\0' ) { return ( walker - target ); }
		if ( * walker == '='  ) { return ( walker - target ); }
		if ( * reference == '\0'  ) { return ( walker - target ); }
		return 0;
	}

	static int utilsOptionStringMatchCaseSensitive( char * reference, char * target ) {
		char * walker = target;
		while ( ( * walker != '\0' ) && ( * walker != '=' ) && ( * reference != '\0' ) && ( * walker == * reference ) ) {
			walker ++;
			reference ++;
		}
		if ( * walker == '\0' ) { return ( walker - target ); }
		if ( * walker == '='  ) { return ( walker - target ); }
		if ( * reference == '\0'  ) { return ( walker - target ); }
		return 0;
	}

	static int utilsOptionStringMatch( utilsOption * reference, char * target ) {
		if ( reference->flags & UTILS_OPTION_CASE ) {
			return utilsOptionStringMatchCaseInsensitive( reference->name, target );
		}
		else {
			return utilsOptionStringMatchCaseSensitive( reference->name, target );
		}
	}

	static int utilsOptionMatch( int index, int * argc, char ** argv, utilsOption * options ) {

		int toggle = 0;
		int match = 0;
		char * argument = utilsOptionSkipPrefix( argv[index] );
		utilsOption * found = NULL;
		utilsOption * walker = options ? options : utils_options;

		// search matching option
		while ( walker != NULL ) {

			if ( ( 0 == ( walker->type & ( UTILS_OPTION_OPEN | UTILS_OPTION_CLOSE ) ) ) && ! ( walker->flags & UTILS_OPTION_IGNORE ) ) {

				// search for a new match; first chance with normal match
				int new_toggle = 0;
				int new_match = utilsOptionStringMatch( walker, argument );

				// search for a new match; second chance only for TOGGLE option with "no"-prefixed match
				if ( new_match == 0 ) {
					if ( walker->type == UTILS_OPTION_TOGGLE ) {
						if ( ( argument[0] == 'n' ) && ( argument[1] == 'o' ) ) {
							new_match = utilsOptionStringMatch( walker, argument + 2 );
							new_toggle = 2;
						}
					}
				}

				// if got a new match, check if any previous match...
				if ( new_match != 0 ) {
					// but if the new match is perfect then ignore any previous match, get out.
					if ( ( ( argument[new_match+new_toggle] == '\0' ) || ( argument[new_match+new_toggle] == '=' ) ) && ( walker->name[new_match] == '\0' ) ) {
						found = walker;
						match = new_match;
						toggle = new_toggle;
						break;
					}
					// if it was a partial match on reference that is not a prefix, then it's not really a match, ignore the new match
					if ( ( ( argument[new_match+new_toggle] != '\0' ) && ( argument[new_match+new_toggle] != '=' ) ) && ! ( walker->flags & UTILS_OPTION_PREFIX ) ) {
					}
					// if it is the first match, keep it in mind
					else if ( found == NULL ) {
						found = walker;
						match = new_match;
						toggle = new_toggle;
					}
					// else if previous match was hidden then ignore the previous match
					else if ( found->flags & UTILS_OPTION_HIDDEN ) {
						found = walker;
						match = new_match;
						toggle = new_toggle;
					}
					// else the previous was not hidden, but if the new one is hidden then ignore the new match
					else if ( walker->flags & UTILS_OPTION_HIDDEN ) {
					}
					// else none are hidden, and none match is perfect
					else { // it is an "ambigous matching" error case
						utilsPrintError( "UTILS", "ambigous option '%s' ('%s' VS '%s' )", argv[index], walker->name, found->name );
						return -1;
					}
				}

			}

			// go next option to check if another one match
			walker = walker->next;
		}

		// if some match found, perform option parsing
		if ( found != NULL ) {

			int pack = 0;

			// check used ONCE if required

			if ( ( found->flags & UTILS_OPTION_ONCE ) && ( found->flags & UTILS_OPTION_DONE ) ) {
				utilsPrintError( "UTILS", "option '%s' must be set only once by command line\n", found->name );
				return -1;
			}

			// set option with value

			if ( found->type == UTILS_OPTION_TAG ) { // case of TAG
				if ( argument[match] != '\0' ) { // value is in same argv as option, but not expected
					utilsPrintError( "UTILS", "no value expected for option '%s'\n", found->name );
					return -1;
				}
				if ( ! utilsOptionSetTag( found, argument ) ) {
					return -1;
				}
				pack = 1;
			}

			else if ( found->type == UTILS_OPTION_TOGGLE ) { // case of TOGGLE
				if ( argument[match+toggle] != '\0' ) { // value is in same argv as option, but not expected
					utilsPrintError( "UTILS", "no value expected for option '%s'\n", found->name );
					return -1;
				}
				if ( ! utilsOptionSetToggle( found, toggle ? "no" : "yes" ) ) {
					return -1;
				}
				pack = 1;
			}

			else { // case of SCALAR and LIST

				char * value = NULL;

				// look for value
				if ( argument[match] == '=' ) { // value is in same argv as option
					value = argument + match + 1;
					pack = 1;
				}
				else if ( ( argument[match] != '\0' ) && ( found->flags & UTILS_OPTION_PREFIX ) ) {
					value = argument + match;
					pack = 1;
				}
				else { // value is in next argv
					if ( utilsOptionStop( index + 1, argc, argv ) ) {
						utilsPrintError( "UTILS", "argument missing for option '%s'\n", found->name );
						return -1;
					}
					value = argv[index + 1];
					pack = 2;
				}

				// set option with value
				if ( found->type == UTILS_OPTION_SCALAR ) {
					if ( found->flags & UTILS_OPTION_DONE ) {
						utilsPrintWarning( "UTILS", "Multiple setting of option '%s', was '%s' but is now '%s'", found->name, ( found->value ) ? UTILS_STRING_SAFE( found->value->argument ) : "<null>", value );
					}
					if ( ! utilsOptionSetScalar( found, value ) ) {
						return -1;
					}
				}
				else {
					if ( ! utilsOptionSetList( found, value ) ) {
						return -1;
					}
				}

			}

			found->flags |= UTILS_OPTION_DONE;

			// strip off argc/argv from option arguments if no resistance required

			if ( ! ( found->flags & UTILS_OPTION_RESIST ) ) {
				utilsOptionPack( pack, index, argc, argv );
				return 0;
			}

			else {
				return pack;
			}

		}

		return -1;
	}

/** @}
*/

/** \defgroup utilsOption publics
* @{
*/

	int utilsOptionInit( utilsOption * more, int size ) {
		if ( more != NULL ) {
			utilsOption * walker = more;
			if ( size > 1 ) {
				// more is considered as pointer on array of <size> options
				// walk through the array while linking items to the other until the last one
				while ( size -- > 1 ) {
					walker->next = ( walker + 1 );
					walker ++;
				}
			}
			else {
				// more is an option handle that may be linked to other options by 'next' list
				// walk through the list until the last one
				while ( walker->next != NULL ) { walker = walker->next; }
			}
			// link the last new option to previous knwon option list
			walker->next = utils_options;
			utils_options = more;
		}
		return TRUE;
	}

	int utilsOptionParse( int * argc, char ** argv, utilsOption * options, int last ) {

		int index = 0;

		if ( ! utilsOptionReset( options ) ) {
			return FALSE;
		}

		while ( ! utilsOptionStop( index, argc, argv ) ) { // while no argv limit nor '--' encountered...
			if ( utilsOptionLooksLikeAnOption( argv[index] ) ) {
				int next = utilsOptionMatch( index, argc, argv, options );
				if ( next < 0 ) { // error or no match found...
					if ( last ) { // and last parser, then error
						utilsPrintError( "UTILS", "Invalid option '%s'", argv[index] );
						return FALSE;
					}
					else { // not last parser invoked, then keep going
						index += 1;
					}
				}
				else { // go on next argument
					index += next;
				}
			}
			else { // it does not looks like and option, go on next argument
				index += 1;
			}
		}

		return TRUE;
	}

	int utilsOptionUsage( FILE * output, utilsOption * options, char * arguments, int all ) {
		int prefix = 1;
		utilsOption * walker = options ? options : utils_options;
		fprintf( output, "\n" );
		fprintf( output, "usage: %s [option] %s\n", UTILS_STRING_SAFE( utils_print_version->name ), arguments ? arguments : "" );
		fprintf( output, "\n" );
		fprintf( output, "  %s\n", UTILS_STRING_SAFE( utils_print_version->description ) );
		fprintf( output, "  %s\n", UTILS_STRING_SAFE( utils_print_version->what ) );
		fprintf( output, "\n" );
		fputc( '\n', output );
		while ( walker != NULL ) {
			if ( all || ! ( walker->flags & ( UTILS_OPTION_HIDDEN | UTILS_OPTION_IGNORE ) ) ) { 
				if ( walker->type == UTILS_OPTION_CLOSE ) {
					if ( prefix > 0 ) {
						prefix -= 1;
					}
				}
				utilsOptionHelp( output, walker, prefix );
				if ( walker->type == UTILS_OPTION_OPEN ) {
					prefix += 1;
				}
			}
			walker = walker->next;
		}
		fputc( '\n', output );
		return TRUE;
	}

	int utilsOptionHelp( FILE * output, utilsOption * option, int prefix ) {
		int i;
		if ( option->type != UTILS_OPTION_CLOSE ) {
			if ( option->flags & UTILS_OPTION_BLANK ) {
				fputc( '\n', output );
			}
			if ( option->flags & UTILS_OPTION_LINE ) {
				for( i=0; i<prefix; i++ ) { fputs( "  ", output ); }
				for( i=0; i< ( 80 - 2 * prefix ); i++ ) { fputc( '-', output ); }
				fputc( '\n', output );
			}
			for( i=0; i<prefix; i++ ) { fputs( "  ", output ); }
			switch ( option->type ) {
				case UTILS_OPTION_OPEN: {
					if ( option->name != NULL ) { fprintf( output, "%s )", option->name ); }
					if ( option->data != NULL ) { fprintf( output, " %s",  option->data ); }
					fputc( '\n', output );
					break;
				}
				case UTILS_OPTION_TAG: {
					fprintf( output, "-%s\t:", UTILS_STRING_SAFE( option->name ) );
					break;
				}
				case UTILS_OPTION_TOGGLE: {
					fprintf( output, "-%s / -no%s\t:", UTILS_STRING_SAFE( option->name ), UTILS_STRING_SAFE( option->name ) );
					break;
				}
				case UTILS_OPTION_SCALAR: {
					fprintf( output, "-%s <%s>\t:", UTILS_STRING_SAFE( option->name ), option->data ? option->data : "value" );
					break;
				}
				case UTILS_OPTION_LIST: {
					fprintf( output, "-%s <%s>\t:", UTILS_STRING_SAFE( option->name ), option->data ? option->data : "list" );
					break;
				}
				default: break;
			}
			if ( option->help != NULL ) {
				fprintf( output, " %s", option->help );
			}
			if ( option->init != NULL ) {
				fprintf( output, " (default=%s)", option->init );
			}
			if ( option->flags & UTILS_OPTION_ONCE ) {
				fputs( " (allowed only once)", output );
			}
		}
		fputc( '\n', output );
		return TRUE;
	}

	int utilsOptionDebug( FILE * output, utilsOption * options ) {
		int i;
		int prefix = 0;
		utilsOption * walker = options ? options : utils_options;
		fputc( '\n', output );
		while ( walker != NULL ) {
			if ( walker->type == UTILS_OPTION_CLOSE ) { prefix -= 1; }
			for( i=0; i<prefix; i++ ) { fputc( '\t', output ); }
			switch ( walker->type ) {
				case UTILS_OPTION_TAG:    { fputs( "TAG", output ); break; }
				case UTILS_OPTION_TOGGLE: { fputs( "TOGGLE", output ); break; }
				case UTILS_OPTION_SCALAR: { fputs( "SCALAR", output ); break; }
				case UTILS_OPTION_LIST:   { fputs( "LIST", output ); break; }
				case UTILS_OPTION_OPEN:   { fputs( "OPEN", output ); break; }
				case UTILS_OPTION_CLOSE:  { fputs( "CLOSE", output ); break; }
				default:                  { fputs( "?", output ); break; }
			}
			if ( walker->type == UTILS_OPTION_OPEN ) { prefix += 1; }
			if ( walker->type != UTILS_OPTION_CLOSE ) {
				fprintf( output, "\t%s", walker->name );
				if ( walker->type != UTILS_OPTION_OPEN ) {
					fputs( "\t[", output );
					fputc( ( walker->flags & UTILS_OPTION_RESIST ) ? 'R' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_HIDDEN ) ? 'H' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_CASE   ) ? 'C' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_ONCE   ) ? 'O' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_PREFIX ) ? 'P' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_IGNORE ) ? 'I' : '-', output );
					fputc( ( walker->callback != NULL     ) ? 'A' : '-', output );
					fputc( ( walker->flags & UTILS_OPTION_DONE   ) ? 'D' : '-', output );
					fputs( "] { ", output );
					if ( ! utilsOptionIsDefined( walker ) ) {
						fputs( "undefined ", output );
					}
					else {
						utilsOptionValue * value = walker->value;
						while ( value != NULL ) {
							fprintf( output, "'%s'%s", UTILS_STRING_SAFE( value->argument ), value->next ? "; " : " " );
							value = value->next;
						}
					}
					fputc( '}', output );
				}
			}
			fputc( '\n', output );
			walker = walker->next;
		}
		fputc( '\n', output );
		return TRUE;
	}

/** @}
*/



