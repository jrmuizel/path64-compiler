/** \file groups.h
    \brief management of SLIW groupings. ST100 dedicated

*/
#ifndef	__GROUPS_H__
#define	__GROUPS_H__

#include <stdio.h>
#include "gensyn_defs.h"

/** AddInstTypeToCurrSlot */
extern void AddInstTypeToCurrSlot(char *typeName) ;
/** SetNewCurrSlot */
extern void SetNewCurrSlot(uint32 index) ;
/** AddCurrSlotToCurrGrouping */
extern void AddCurrSlotToCurrGrouping() ;
/** IsGroupingDefined */
extern int IsGroupingDefined(char *name) ;
/** SetNewCurrGrouping */
extern void SetNewCurrGrouping(char *name) ;
/** InitGroupings */
extern void InitGroupings() ;
/** AddCurrGroupingToList */
extern void AddCurrGroupingToList() ;
/** PrintGroupings */
extern void PrintGroupings (FILE*cfd) ;
/** PrintGroupingTable */
extern void PrintGroupingTable (FILE*cfd) ;

#endif /* __GROUPS_H__ */
