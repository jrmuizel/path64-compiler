#include <map>                 // For map usage
#include <stdio.h>
#include "defs.h"
#include "op.h" 		
#include "bb.h" 		
#include "cg_ssa.h" 		
#include "cg_affirm.h" 		
#include "pu_info.h"     /* for Current_PU_Info */
#include "erglob.h"
#include "tracing.h"

typedef struct graphMLGraph graphMLGraph;
typedef struct graphMLNode graphMLNode;
typedef struct graphMLEdge graphMLEdge;

//typedef YList<graphMLGraph*> graphMLGraphList;
//typedef graphMLGraphList::iterator graphMLGraphIterator;

typedef enum graphMLNodeType {
	graphML_BB = 0,				///< normal basic block
	graphML_BB_EXIT,			///< exit block
	graphML_BB_CALL,			///< Call block
	graphML_BB_TAIL_CALL,		///< Tail call block
	graphML_LAST				///< illegal value 
}graphMLNodeType;

typedef enum graphMLEdgeType
{
	graphML_CONTROL_DEPENDENCE=0,
	graphML_DD_EV_DDATABLE_ITER,
	graphML_DD_EV_LEVEL,
	graphML_EDGE_LAST
}graphMLEdgeType;

	
typedef struct graphMLNode
{
	int 			index_;		///< index in the graph
	char*			label_;		///< node label
	graphMLNodeType		type_;		///< node type
	char			is_folder_;	///< if the node is folder, then it has subgraphs
	graphMLGraph*		sons_;		///< list of subgraphs
	graphMLGraph*		parent_;	///< parent graph
	int				x_coord_;	///< set for constraints
	graphMLNode*		next_;
}graphMLNodeStruct;

typedef struct graphMLEdge
{
	graphMLEdgeType		type_;		///< edge type
	char*			label_;		///< edge label (if any)
	graphMLGraph*		parent_;	///< parent graph
	graphMLNode*		source_;	///< source node
	graphMLNode*		target_;	///< target node
	graphMLEdge*		next_;
}graphMLEdgeStruct;

typedef struct graphMLGraph
{
	char*			name_;	///< graph name
	graphMLNode*		nodes_;	///< list of nodes
	graphMLEdge*		edges_;	///< list of edges
	graphMLNode*		parent_;///< parent node (if any)
	graphMLGraph*		next_;
}graphMLGraphStruct;


// Graph functions
void graphML_DumpCFG(BB* bb, const char* suffix); 
