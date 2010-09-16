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
////    Declaration of DFG Vertices types.                                       ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgExternalTypes.h
 */


#ifndef DfgExternalTypes_h
#define DfgExternalTypes_h



namespace DfgForIse {



  /** Enumeration of vertex kinds in an extended DFG.
   * Note : I/O Variable vertices are part of extended DFGs.
   */
  enum VertexKindType {
    VERTEX_UNDEFINED,     /**< Undefined vertex kind. */
    VERTEX_CONSTANT,      /**< Input constant vertex kind. */
    VERTEX_OPERATION,     /**< Operation vertex kind. */
    VERTEX_VARIABLE,      /**< Input/Output Variable vertex kind. */
    VERTEX_INVALID        /**< Invalid, number of enumerations. */
  };


  /** Name of vertex kinds. 
   */
  const char* const VERTEX_KIND_TYPE_NAME[VERTEX_INVALID] = {
    "UNDEFINED",
    "CONSTANT",
    "OPERATION",
    "VARIABLE"
  };



  /** Enumeration of edge kinds in an extended DFG.
   */
  enum EdgeKindType {

    EDGE_DEP_REGIN,      /**<  Standard dependency between operations. */
    EDGE_DEP_REGOUT,
    EDGE_DEP_REGANTI,
    EDGE_DEP_MEMIN,
    EDGE_DEP_MEMOUT,
    EDGE_DEP_MEMANTI,
    EDGE_DEP_MEMVOL,
    EDGE_DEP_MEMREAD,
    EDGE_DEP_SPILLIN,
    EDGE_DEP_PREFIN,
    EDGE_DEP_PREFOUT,
    EDGE_DEP_PREBR,
    EDGE_DEP_POSTBR,
    EDGE_DEP_SCC,
    EDGE_DEP_MISC,

    EDGE_IN_UNDEFINED,   /**< Input undefined edge kind. */
    EDGE_IN_CONSTANT,    /**< Input constant edge kind. */
    EDGE_IN_VARIABLE,    /**< Input variable edge kind. */
    EDGE_OUT_VARIABLE,   /**< Output variable edge kind. */

    EDGE_INVALID         /**< Invalid, number of enumerations. */
  };


  /** Name of edge kinds. 
   */
  const char* const EDGE_KIND_TYPE_NAME[EDGE_INVALID] = {
    "DEP_REGIN",
    "DEP_REGOUT",
    "DEP_REGANTI",
    "DEP_MEMIN",
    "DEP_MEMOUT",
    "DEP_MEMANTI",
    "DEP_MEMVOL",
    "DEP_MEMREAD",
    "DEP_SPILLIN",
    "DEP_PREFIN",
    "DEP_PREFOUT",
    "DEP_PREBR",
    "DEP_POSTBR",
    "DEP_SCC",
    "DEP_MISC",

    "IN_UNDEFINED",
    "IN_CONSTANT",
    "IN_VARIABLE",
    "OUT_VARIABLE"
  };



};

#endif // DfgExternalTypes_h
