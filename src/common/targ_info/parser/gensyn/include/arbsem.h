/** \file arbsem.h
    \brief Semantic tree definitions.

*/

#ifndef __ARBRE_SEM_H__
#define __ARBRE_SEM_H__

#include <stdio.h>
#include <time.h>

#include "unilex.h"
#include "gensyn_defs.h"
#include "strind.h"

/** \typedef noeudSEM
    Semantic node.
*/

/** \struct noeudSEM
    This structure is used to store semantic nodes.
 */
typedef struct noeudSEM {
  ATOME action;          /**< Action associated to semantic node */
  int semIndex;          /**< Index in semantics */
  int encIndex ;         /**< Index in encoding stack */
  int popSemIndex ;      /**< */
  s_StrInd *token;       /**< Token associated to semantic node */
  struct noeudSEM *fg;   /**< Left child of semantic node */
  struct noeudSEM *fd;   /**< Right child of semantic node */
} noeudSEM;

/** Unique number used to identify assembler/linker. */
extern time_t current_time;
/** String used to display generation date of automatically generated files */
extern char   datestr[64];

/** set_current_time */
extern void set_current_time( void );
/** storeSemTree */
extern void	    storeSemTree(noeudSEM *ST) ;
/** GenCode */
extern void     GenCode (FILE *cFileHeader,char *cname);
/** InitPile */
extern void     InitPile (void);
/** InitTab */
extern void     InitTab (void);
/** EmpilerSEM */
extern void     EmpilerSEM(noeudSEM *Elem);
/** DepilerSEM */
extern noeudSEM *DepilerSEM(void);
/** GenSEM */
extern noeudSEM *GenSEM (ATOME lexeme, s_StrInd *val); 
/** SetGauche */
extern void     SetGauche (noeudSEM *Racine, noeudSEM *fg);
/** SetPopSemIndex */
extern void     SetPopSemIndex (noeudSEM *Racine, int index);
/** SetDroite */
extern void     SetDroite (noeudSEM *Racine, noeudSEM *fd);
/** RechAct */
extern noeudSEM *RechAct (char *nom, int *ind);
/** AjoutAct */
extern void     AjoutAct (char *nom, noeudSEM *Action);
/** GetAxiome */
extern noeudSEM *GetAxiome (void);
/** SetEncInfo */
extern void     SetEncInfo (noeudSEM *T, int encIndex) ;

/** \typedef OutputT
    Type of output required.
*/

/** \enum OutputT
    This enumaration type defines command line requirements.
 */
typedef enum {
   OUT_FOR_NONE=0,      /**< Output not specified on command line  */
   OUT_FOR_CORE=1,      /**< Output required for core of assembler */
   OUT_FOR_EXTENSION=2  /**< Output required for an extension      */
} OutputT;

extern OutputT OutputType; /**< Variable defining output type required */

#endif
