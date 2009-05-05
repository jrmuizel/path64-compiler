/* 

  Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved. 

  This program is free software; you can redistribute it and/or modify it 
  under the terms of version 2 of the GNU General Public License as 
  published by the Free Software Foundation. 
  This program is distributed in the hope that it would be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

  Further, this software is distributed without any warranty that it is 
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever. 
  You should have received a copy of the GNU General Public License along 
  with this program; if not, write the Free Software Foundation, Inc., 59 
  Temple Place - Suite 330, Boston MA 02111-1307, USA. 

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
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
