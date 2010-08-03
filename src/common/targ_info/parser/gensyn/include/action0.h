/** \file action0.h
    \brief Action routines associated to syntax description.

*/

#ifndef __ACTION0_H__
#define __ACTION0_H__

/** \defgroup GENSYN_ACTION0 Action when syntax rules are matched.
    @{
*/

#include "unilex.h"

#define _FOR_GAS

/** AddNewElf */
extern void AddNewElf (void);
/** AddNewClass */
extern void AddNewClass (void);
/** AddNewReloc */
extern void AddNewReloc (void);
/** SetRelocNum */
extern void SetRelocNum (void);
/** SetRelocStartBit */
extern void SetRelocStartBit (void);
/** SetRelocStopBit */
extern void SetRelocStopBit (void);
/** SetRelocPosBit */
extern void SetRelocPosBit (void);
/** SetRelocInsertBit */
extern void SetRelocInsertBit (void);
/** SetRelocCheck */
extern void SetRelocCheck (void);
/** SetRelocLength */
extern void SetRelocLength (void);
/** SetRelocType */
extern void SetRelocType (void);
/** SetRelocPCRel */
extern void SetRelocPCRel (void);
/** SetRelocSymbName */
extern void SetRelocSymbName (void);
/** AddInitAction */
extern void AddInitAction (char *newEncInfo) ;
/** AddNewAction */
extern void AddNewAction (char *newEncInfo) ;
/** CreateNewNTERM */
extern void CreateNewNTERM (void);
#ifdef _FOR_GAS
/** CreateNewEXPR */
extern void CreateNewEXPR (void);
#endif
/** CreateEmpty */
extern void CreateEmpty (void);
/** UnstackNTERM */
extern void UnstackNTERM (void);
/** GenDualNodeSem */
extern void GenDualNodeSem (ATOME atomRead) ;
/** CreateNewTERM */
extern void CreateNewTERM (void);
/** GenModeNodeSem */
extern void GenModeNodeSem (ATOME atomRead) ;
/** CreateNewNumVal */
extern void CreateNewNumVal (ATOME atomRead) ;
/** CreateNewLexem */
extern void CreateNewLexem (ATOME atomRead) ;
/** InitLexCount */
extern void InitLexCount (void);
/** CreateNewFormat */
extern void CreateNewFormat (void);
/** AddNewFormat */
extern void AddNewFormat (void);
/** CreateNewField */
extern void CreateNewField (void);
/** AddNumField */
extern void AddNumField (void);
/** AddNewField */
extern void AddNewField (void);
/** CreateNewGrouping */
extern void CreateNewGrouping (void);
/** CreateNewSlot */
extern void CreateNewSlot (void);
/** AddInstType */
extern void AddInstType (void);
/** AddNewSlot */
extern void AddNewSlot (void);
/** AddNewGrouping */
extern void AddNewGrouping (void);

/** \typedef CmdLineFlagT
    Command line flag structure.
*/

/** \struct CmdLineFlagT
    This structure is used to store command line flags.
 */

typedef struct {
   int dohelp;    /**< flag -h/--help set on command line */
   int doversion; /**< flag -V/--version set on command line */
   int doverbose; /**< flag -v/--verbose set on command line */
} CmdLineFlagT;

extern CmdLineFlagT flag; /**< Flags of command line */

/** @} */

#endif /* __ACTION0_H__ */
