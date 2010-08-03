/** \file astype.h
    \brief Specific types used by assembler.

*/
#ifndef __ASTYPE_H__
#define __ASTYPE_H__

#include "gensyn_defs.h"

/** \typedef s_String
    String storage.
*/

/** \struct s_String
    This structure is used to store strings.
 */
typedef struct {
	unsigned char *buf ; /**< Pointer to the string itself */
	uint16 length ;      /**< Length of the string */
} s_String ;

#endif /* __ASTYPE_H__ */

