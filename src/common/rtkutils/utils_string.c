/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : utils_string.h
 *
 * Description : string facilities
............................................................................
*/

#include "utils_memory.h"
#include "utils_string.h"

/*--------------------------------------------------------------------------------------*
*
* What: some kind of clever strstr( ).
*
* Looks if a given word (sequence of non-space-class chars) is in given sequence of
* reference words (words separated by space-class char).
*
* Inputs:
*
* - char * reference: sequence of reference words
* - char * word: word to search in <all>
*
* Returns:
*
* - TRUE if <word> is a word defined in <reference> or a word ending with 'all-match'
*   wild-card.
*
* Exemple:
*
* - utilsStringIsOneOf( "reference", NULL  ) returns FALSE, whatever "reference"
* - utilsStringIsOneOf( NULL, "any" ) returns FALSE, whatever "any"
*
* - utilsStringIsOneOf( "some reference word", "any" )      returns FALSE
* - utilsStringIsOneOf( "any reference word", "any" )      returns TRUE
* - utilsStringIsOneOf( "any* reference word", "any" )      returns TRUE
* - utilsStringIsOneOf( "any* reference word", "anything" ) returns TRUE
*
*---------------------------------------------------------------------------------------*/

int utilsStringIsOneOf( char * reference, char * word ) {

  char * walk_reference;
  char * walk_word;

  if ( word == NULL ) { return FALSE; }

  if ( reference == NULL ) { return FALSE; }

  /* walk along reference */
  walk_reference = reference;
  while( * walk_reference != '\0' ) {

    /* walk along reference and word while chars are same and non-space */
    walk_word = word;
    while( ( * walk_word == * walk_reference ) && ( * walk_word != '\0' ) && ! isspace( ( int )( * walk_reference ) ) ) { walk_word ++; walk_reference ++; }

    /* if reached end of word on both sides, then return true */
    if ( ( * walk_word == '\0' ) && ( ( * walk_reference == '\0' ) || isspace( ( int )( * walk_reference ) ) ) ) { return TRUE; }

    /* if reached 'all-match' wild card in reference then return true */
    if ( * walk_reference == '*' ) { return TRUE; }

    /* skip remain of word in reference */
    while( ! isspace( ( int )( * walk_reference ) ) && ( * walk_reference != '\0' ) ) { walk_reference ++; }

    /* skip spaces after failed word in reference */
    while( isspace( ( int )( * walk_reference ) ) && ( * walk_reference != '\0' ) ) { walk_reference ++; }

  }

  /* no match */
  return FALSE;
}

/*--------------------------------------------------------------------------------------*
* simple replacement for strtol and strtoll with addition of binary prefix
*---------------------------------------------------------------------------------------*/

sint_32 utilsString_strtol( char * str, char ** trap, int base ) {
  if ( ( base == 0 ) && ( str != NULL ) && ( str[0] == '0' ) && ( str[1] == 'b' ) ) {
    return strtol( str+2, trap, 2 );
  }
  else {
    return strtol( str, trap, base );
  }
}

sint_64 utilsString_strtoll( char * str, char ** trap, int base ) {
  if ( ( base == 0 ) && ( str != NULL ) && ( str[0] == '0' ) && ( str[1] == 'b' ) ) {
    return strtoll( str+2, trap, 2 );
  }
  else {
    return strtoll( str, trap, base );
  }
}

/*--------------------------------------------------------------------------------------*
* clever replacement for strtoll with addition of binary prefix, separators and "undefined" digit
*---------------------------------------------------------------------------------------*/

static sint_64 utilsStringBinaryToValue( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 value = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '1' ) ) {
      value = ( value << 1 ) + ( * str - '0' );
    }
    else if ( strchr( escapes, * str ) ) {
      value = ( value << 1 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return value;
}

static sint_64 utilsStringDecimalToValue( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 value = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '9' ) ) {
      value = ( value * 10 ) + ( * str - '0' );
    }
    else if ( strchr( escapes, * str ) ) {
      value = ( value * 10 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return value;
}

static sint_64 utilsStringOctalToValue( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 value = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '7' ) ) {
      value = ( value << 3 ) + ( * str - '0' );
    }
    else if ( strchr( escapes, * str ) ) {
      value = ( value << 3 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return value;
}

static sint_64 utilsStringHexaToValue( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 value = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '9' ) ) {
      value = ( value << 4 ) + ( * str - '0' );
    }
    else if ( ( * str >= 'a' ) && ( * str <= 'f' ) ) {
      value = ( value << 4 ) + ( * str - 'a' + 10 );
    }
    else if ( ( * str >= 'A' ) && ( * str <= 'F' ) ) {
      value = ( value << 4 ) + ( * str - 'A' + 10 );
    }
    else if ( strchr( escapes, * str ) ) {
      value = ( value << 4 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return value;
}

uint_64 utilsStringToValue( char * str, char ** trap, int base, char * separators, char * escapes ) {

  if ( str == NULL ) {
    if ( trap != NULL ) { * trap = NULL; }
    return 0;
  }

  if ( base == 0 ) {
    base = utilsStringGuessBaseByPrefix( str );
    str = utilsStringSkipBasePrefix( str );
  }

  if ( separators == NULL ) { separators = ""; }
  if ( escapes == NULL ) { escapes = ""; }

  switch( base ) {
    case  2: return utilsStringBinaryToValue(  str, trap, separators, escapes );
    case  8: return utilsStringOctalToValue(   str, trap, separators, escapes );
    case 10: return utilsStringDecimalToValue( str, trap, separators, escapes );
    case 16: return utilsStringHexaToValue(    str, trap, separators, escapes );
    default: return strtoll( str, trap, base );
  }

}

/*--------------------------------------------------------------------------------------*
* some kind of strtoll but returning mask of defined bits instead of raw value
*---------------------------------------------------------------------------------------*/

static sint_64 utilsStringBinaryToMask( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 mask = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '1' ) ) {
      mask = ( mask << 1 ) + 1;
    }
    else if ( strchr( escapes, * str ) ) {
      mask = ( mask << 1 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return mask;
}

static sint_64 utilsStringOctalToMask( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 mask = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '7' ) ) {
      mask = ( mask << 3 ) + 7;
    }
    else if ( strchr( escapes, * str ) ) {
      mask = ( mask << 3 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return mask;
}

static sint_64 utilsStringDecimalToMask( char * str, char ** trap, char * separators, char * escapes ) {
  uint_64 value = ( uint_64 )utilsStringDecimalToValue( str, trap, separators, escapes );
  sint_64 mask = 0;
  do { mask = ( mask << 1 ) | 1; value >>= 1; } while( value != 0 );
  return mask;
}

static sint_64 utilsStringHexaToMask( char * str, char ** trap, char * separators, char * escapes ) {
  sint_64 mask = 0;
  while( * str != '\0' ) {
    if ( ( * str >= '0' ) && ( * str <= '9' ) ) {
      mask = ( mask << 4 ) + 15;
    }
    else if ( ( * str >= 'a' ) && ( * str <= 'f' ) ) {
      mask = ( mask << 4 ) + 15;
    }
    else if ( ( * str >= 'A' ) && ( * str <= 'F' ) ) {
      mask = ( mask << 4 ) + 15;
    }
    else if ( strchr( escapes, * str ) ) {
      mask = ( mask << 4 );
    }
    else if ( ! strchr( separators, * str ) ) {
      break;
    }
    str += 1;
  }
  if ( trap != NULL ) { * trap = str; }
  return mask;
}

uint_64 utilsStringToMask( char * str, char ** trap, int base, char * separators, char * escapes ) {

  if ( str == NULL ) {
    if ( trap != NULL ) { * trap = NULL; }
    return 0;
  }

  if ( base == 0 ) {
    base = utilsStringGuessBaseByPrefix( str );
    str = utilsStringSkipBasePrefix( str );
  }

  if ( separators == NULL ) { separators = ""; }
  if ( escapes == NULL ) { escapes = ""; }

  switch( base ) {
    case  2: return utilsStringBinaryToMask(  str, trap, separators, escapes );
    case  8: return utilsStringOctalToMask(   str, trap, separators, escapes );
    case 10: return utilsStringDecimalToMask( str, trap, separators, escapes );
    case 16: return utilsStringHexaToMask(    str, trap, separators, escapes );
    default: return strtoll( str, trap, base );
  }

}

/*--------------------------------------------------------------------------------------*
* basic numeric base management based on prefix
*---------------------------------------------------------------------------------------*/

int utilsStringGuessBaseByPrefix( char * str ) {
  if ( str != NULL ) {
    if ( ( str[0] == '0' ) && ( str[1] == 'b' ) ) { return  2; }
    if ( ( str[0] == '0' ) && ( str[1] == 'x' ) ) { return 16; }
    if ( ( str[0] == '0' ) && ( ( ( str[1] >= '0' ) && ( str[1] <= '7' ) ) || ( str[1] == '_' ) || ( str[1] == 'x' ) ) ) { return 8; }
    if ( ( str[0] == '0' ) && ( ( str[1] <  '0' ) || ( str[1] >  '9' ) ) ) { return 10; }
    if ( ( str[0] >= '1' ) && ( str[0] <= '9' ) ) { return 10; }
  }
  return 0;
}

char * utilsStringSkipBasePrefix( char * str ) {
  if ( str != NULL ) {
    if ( ( str[0] == '0' ) && ( str[1] == 'b' ) ) { return str + 2; }
    if ( ( str[0] == '0' ) && ( str[1] == 'x' ) ) { return str + 2; }
    if ( ( str[0] == '0' ) && ( ( ( str[1] >= '0' ) && ( str[1] <= '7' ) ) || ( str[1] == '_' ) || ( str[1] == 'x' ) ) ) { return str + 1; }
    if ( ( str[0] == '0' ) && ( ( str[1] <  '0' ) || ( str[1] >  '9' ) ) ) { return str; }
    if ( ( str[0] >= '1' ) && ( str[0] <= '9' ) ) { return str; }
  }
  return str;
}

char * utilsStringGetBasePrefix( char * str ) {
  if ( str != NULL ) {
    if ( ( str[0] == '0' ) && ( str[1] == 'b' ) ) { return "0b"; }
    if ( ( str[0] == '0' ) && ( str[1] == 'x' ) ) { return "0x"; }
    if ( ( str[0] == '0' ) && ( ( ( str[1] >= '0' ) && ( str[1] <= '7' ) ) || ( str[1] == '_' ) || ( str[1] == 'x' ) ) ) { return "0"; }
  }
  return "";
}

/*--------------------------------------------------------------------------------------*
* some kind of variable length strings
*---------------------------------------------------------------------------------------*/

char * utilsStringAlloc( char * format, ... ) {
	va_list va;
	char dummy[4];
	int size;
	char * buffer;
	va_start( va, format );
	size = vsnprintf( dummy, sizeof( dummy ), format, va );
	UTILS_MEMORY_NEW( buffer, size + 1, char );
	if ( buffer != NULL ) {
		size = vsnprintf( buffer, size + 1, format, va );
	}
	va_end( va );
	return buffer;
}

char * utilsStringAppend( char * base, char * format, ... ) {
	va_list va;
	char dummy[4];
	int size;
	int offset;
	char * buffer;
	va_start( va, format );
	size = vsnprintf( dummy, sizeof( dummy ), format, va );
	if ( base == NULL ) {
		offset = 0;
		UTILS_MEMORY_NEW( buffer, size + 1, char );
	}
	else {
		offset = strlen( base );
		UTILS_MEMORY_REALLOC( buffer, base, size + 1 + offset, char );
	}
	if ( buffer != NULL ) {
		size = vsnprintf( buffer + offset, size + 1, format, va );
		va_end( va );
		return buffer;
	}
	else {
		va_end( va );
		return base;
	}
}

/*--------------------------------------------------------------------------------------*
* some usefull 'skip' for simple parsing
*---------------------------------------------------------------------------------------*/

int utilsStringTrimSpaces( char * base ) {

	char * walker;
	char * scoot;

	if ( base == NULL ) { return 0; }

	walker = base;
	scoot  = base;
	while ( isspace( * scoot ) ) { scoot++; }

	do {
		if ( isspace( * scoot ) ) {
			do { scoot++; } while ( isspace( * scoot ) );
			if ( * scoot == '\0' ) {
				* walker = '\0';
				return ( scoot - walker );
			}
			* walker ++ = ' ';
		}
		else {
			* walker ++ = * scoot ++;
		}
	} while ( * scoot != '\0' );

	* walker = '\0';
	return ( scoot - walker );
}

/*--------------------------------------------------------------------------------------*
* some usefull 'skip' for simple parsing
*---------------------------------------------------------------------------------------*/

char * utilsStringSkipEscape( char * walker ) {
	if ( * walker != '\\' ) { return walker; }
	if ( * ( walker + 1 ) != '0' ) { return walker + 2; }
	do { walker ++; } while( isdigit( ( int )( * walker ) ) ); return walker;
}

char * utilsStringSkipNext( char * walker ) {
	if ( * walker != '\\' ) { return walker + 1; }
	return utilsStringSkipEscape( walker );
}

char * utilsStringSkipUntilString( char * walker, char * until ) {
	int length = strlen( until );
	while( * walker != '\0' ) {
		if ( strncmp( walker, until, length ) == 0 ) { return walker; }
		walker = utilsStringSkipNext( walker );
	}
	return NULL;
}

char * utilsStringSkipUntilChar( char * walker, char until ) {
	while( ( * walker != '\0' ) && ( * walker != until ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipUntilNonChar( char * walker, char until ) {
	while( ( * walker != '\0' ) && ( * walker == until ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipUntilMatch( char * walker, char * until ) {
	while( ( * walker != '\0' ) && ( strchr( until, * walker ) == NULL ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipUntilNonMatch( char * walker, char * until ) {
	while( ( * walker != '\0' ) && ( strchr( until, * walker ) != NULL ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipWhileChar( char * walker, char until ) {
	while( ( * walker != '\0' ) && ( * walker == until ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipWhileNonChar( char * walker, char until ) {
	while( ( * walker != '\0' ) && ( * walker != until ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipWhileMatch( char * walker, char * match ) {
	while( ( * walker != '\0' ) && ( strchr( match, * walker ) != NULL ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipWhileNonMatch( char * walker, char * match ) {
	while( ( * walker != '\0' ) && ( strchr( match, * walker ) == NULL ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipSpace( char * walker ) {
	while( ( * walker != '\0' ) && isspace( ( int )( * walker ) ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipNonSpace( char * walker ) {
	while( ( * walker != '\0' ) && ! isspace( ( int )( * walker ) ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipDigit( char * walker ) {
	while( ( * walker != '\0' ) && isdigit( ( int )( * walker ) ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipNonDigit( char * walker ) {
	while( ( * walker != '\0' ) && ! isdigit( ( int )( * walker ) ) ) { walker = utilsStringSkipNext( walker ); }
	return walker;
}

char * utilsStringSkipEmbraced( char * walker, int level ) {
  char was = 0;
  char now = * walker;
  while( now != '\0' ) {
    if ( ( now == ')' ) || ( now == ']' ) || ( now == '}' ) ) {
      return walker;
    }
    else if ( now == '(' ) {
      char * scoot = utilsStringSkipEmbraced( walker + 1, level + 1 );
      if ( * scoot != ')' ) { return walker; }
      walker = scoot + 1;
    }
    else if ( now == '[' ) {
      char * scoot = utilsStringSkipEmbraced( walker + 1, level + 1 );
      if ( * scoot != ']' ) { return walker; }
      walker = scoot + 1;
    }
    else if ( now == '{' ) {
      char * scoot = utilsStringSkipEmbraced( walker + 1, level + 1 );
      if ( * scoot != '}' ) { return walker; }
      walker = scoot + 1;
    }
    else if ( ( now == '`' ) || ( now == '"' ) || ( now == '\'' ) ) {
      char * scoot = utilsStringSkipUntilChar( walker + 1, now );
      if ( * scoot != now ) { return walker; }
      walker = scoot + 1;
    }
    else if ( now == '/' ) {
      if ( * ( walker + 1 ) == '*' ) {
        char * scoot = utilsStringSkipUntilString( walker + 2, "*/" );
        if ( scoot == NULL ) { return walker; }
        walker = scoot + 2;
      }
      else if ( * ( walker + 1 ) == '/' ) {
        char * scoot = utilsStringSkipUntilChar( walker + 2, '\n' );
        if ( * scoot != '\n' ) { return walker; }
        walker = scoot + 1;
      }
      else {
       walker ++;
      }
    }
    else if ( now == '\\') {
      walker = utilsStringSkipEscape( walker );
    }
    else {
      walker ++;
    }
    was = now;
    now = * walker;
  }
  return walker;
}

