/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////                                                                             ////
////    Declaration of Graphml types.                                            ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file GraphmlTypes.h
 */


#ifndef GraphmlTypes_h
#define GraphmlTypes_h



namespace DfgForIse {



  /** Enumeration of graphml files node types. 
   */
  enum GraphmlNodeType {
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BASICBLOCK,
    NODE_TYPE_VERTEX,
    NODE_TYPE_SUBGRAPH,
    NODE_TYPE_INSTRUCTION,
    NODE_TYPE_INVALID
  };


  /** Name of graphml files node types. 
   */
  const char* const GRAPHML_NODE_TYPE_NAME[NODE_TYPE_INVALID] = {
    "NODE_FUNCTION",
    "NODE_BASICBLOCK",
    "NODE_VERTEX",
    "NODE_SUBGRAPH",
    "NODE_INSTRUCTION"
  };


  /** Enumeration of graphml files edge types. 
   */
  enum GraphmlEdgeType {
    EDGE_TYPE_CFG,
    EDGE_TYPE_DFG,
    EDGE_TYPE_INVALID
  };


  /** Name of graphml files edge types. 
   */
  const char* const GRAPHML_EDGE_TYPE_NAME[EDGE_TYPE_INVALID] = {
    "EDGE_CFG",
    "EDGE_DFG"
  };



};

#endif // GraphmlTypes_h
