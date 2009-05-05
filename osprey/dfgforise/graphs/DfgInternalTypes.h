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
////    Declaration of types used by Dfg classes.                                ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgInternalTypes.h
 */


#ifndef DfgInternalTypes_h
#define DfgInternalTypes_h

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>



// Adding an alias in Boost library for accessing DFG vertex and edge properties.
namespace boost {



  /** Property on boost vertices to link to DfgVertex objects.
   */
  enum vertex_dfg_t { vertex_dfg };


  /** Installing vertex property in boost.
   */
  BOOST_INSTALL_PROPERTY(vertex, dfg);


  /** Property on boost edges to link to DfgEdge objects.
   */
  enum edge_dfg_t { edge_dfg };


  /** Installing edge property in boost.
   */
  BOOST_INSTALL_PROPERTY(edge, dfg);



};



namespace DfgForIse {



  class DfgVertex;
  class DfgEdge;



  /** Type definition for the vertex property used in the Boost graph.
   * The property vertex_index_t is necessary for the topological ordering.
   */
  typedef boost::property< boost::vertex_dfg_t, DfgVertex* > BoostVertexProperty;


  /** Type definition for the edge property used in the Boost graph.
   */
  typedef boost::property< boost::edge_dfg_t, DfgEdge* > BoostEdgeProperty;


  /** Type definition for the Boost graph representing the extended DFG.
   */
  typedef boost::adjacency_list< boost::listS, boost::vecS, boost::bidirectionalS,
				 BoostVertexProperty,
				 BoostEdgeProperty > BoostDfg ;
  

  /** Type definition for vertices used in the Boost graph.
   */
  typedef BoostDfg::vertex_descriptor BoostVertexDescriptor;
    

  /** Type definition for vertex iterators used in the Boost graph.
   */
  typedef BoostDfg::vertex_iterator BoostVertexIterator;
    

  /** Type definition for adjacent vertex iterators used in the Boost graph.
   */
  typedef BoostDfg::adjacency_iterator BoostAdjacencyIterator;
    

  /** Type definition for inverse adjacent vertex iterators used in the Boost graph.
   */
  typedef BoostDfg::inv_adjacency_iterator BoostInvAdjacencyIterator;
    

  /** Type definition for vertices' dfg property maps used in the Boost graph.
   * Const Type.
   */
  typedef boost::property_map< BoostDfg, boost::vertex_dfg_t >::const_type BoostVertexDfgMapConstT;


  /** Type definition for vertices' dfg property maps used in the Boost graph.
   * Non Const Type.
   */
  typedef boost::property_map< BoostDfg, boost::vertex_dfg_t >::type BoostVertexDfgMapT;


  /** Type definition for edges used in the Boost graph.
   */
  typedef BoostDfg::edge_descriptor BoostEdgeDescriptor;
    

  /** Type definition for edge iterators used in the Boost graph.
   */
  typedef BoostDfg::edge_iterator BoostEdgeIterator;
    

  /** Type definition for out edge iterators used in the Boost graph.
   */
  typedef BoostDfg::out_edge_iterator BoostOutEdgeIterator;
    

  /** Type definition for in edge iterators used in the Boost graph.
   */
  typedef BoostDfg::in_edge_iterator BoostInEdgeIterator;
   
 
  /** Type definition for edges' index property maps used in the Boost graph.
   * Const Type.
   */
  typedef boost::property_map< BoostDfg, boost::edge_dfg_t >::const_type BoostEdgeDfgMapConstT;
  

  /** Type definition for edges' index property maps used in the Boost graph.
   * Non Const Type.
   */
  typedef boost::property_map< BoostDfg, boost::edge_dfg_t >::type BoostEdgeDfgMapT;
  


};

#endif // DfgInternalTypes_h
