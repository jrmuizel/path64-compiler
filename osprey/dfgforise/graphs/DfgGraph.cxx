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
////    Definition of DfgGraph functions.                                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgGraph.cxx
 */


#include "DfgGraph.h"

#include "DfgEdge.h"
#include "DfgInternalTypes.h"
#include "DfgVertex.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>

namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void DfgGraph::addVertex(DfgVertex* pDfgVertex) {
    assert(pDfgVertex);
    if (pDfgVertex->mpGraph) {
      assert(pDfgVertex->mpGraph == this);
      return;
    }
    pDfgVertex->mpGraph = this;
    BoostVertexDescriptor new_vertex = boost::add_vertex(pDfgVertex, mBoostDfg);
    pDfgVertex->mBoostVertex = new_vertex;
  };


  void DfgGraph::addVertex(const size_t idVertex) {
    DfgVertex* p_dfg_vertex = DfgVertex::getVertexById(idVertex);
    addVertex(p_dfg_vertex);
  };


  void DfgGraph::removeVertex(DfgVertex* pDfgVertex) {
    assert(pDfgVertex);
    assert(pDfgVertex->mpGraph == this);
    boost::clear_vertex(pDfgVertex->mBoostVertex, mBoostDfg);
    boost::remove_vertex(pDfgVertex->mBoostVertex, mBoostDfg);
  };


  void DfgGraph::addEdge(DfgVertex* pSourceVertex, DfgVertex* pTargetVertex,
			 DfgEdge* pDfgEdge) {
    assert(pSourceVertex);
    assert(pSourceVertex->mpGraph == this);
    assert(pTargetVertex);
    assert(pTargetVertex->mpGraph == this);
    BoostEdgeDescriptor new_edge;
    bool inserted;
    boost::tie(new_edge, inserted) = boost::add_edge(pSourceVertex->mBoostVertex,
						     pTargetVertex->mBoostVertex,
						     pDfgEdge,
						     mBoostDfg);
    pDfgEdge->mpGraph = this;
    pDfgEdge->mBoostEdge = new_edge;
  };


  void DfgGraph::addEdge(const size_t idSource, const size_t idTarget,
			 DfgEdge* pDfgEdge) {
    DfgVertex* p_source_vertex = DfgVertex::getVertexById(idSource);
    DfgVertex* p_target_vertex = DfgVertex::getVertexById(idTarget);
    addEdge(p_source_vertex, p_target_vertex, pDfgEdge);
  };


  void DfgGraph::removeEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    assert(pDfgEdge->mpGraph == this);
    boost::remove_edge(pDfgEdge->mBoostEdge, mBoostDfg);
  };


  void DfgGraph::clear() {

    // Deletes the corresponding edges.
    BoostEdgeDfgMapT edge_dfg_map = boost::get(boost::edge_dfg,
					       mBoostDfg);
    BoostEdgeIterator first_edge, last_edge;
    for(boost::tie(first_edge, last_edge) = boost::edges(mBoostDfg);
	first_edge != last_edge;
	++first_edge) {
      delete edge_dfg_map[*first_edge];
    }

    // Deletes the corresponding vertices.
    BoostVertexDfgMapT vertex_dfg_map = boost::get(boost::vertex_dfg,
						   mBoostDfg);
    BoostVertexIterator first_vertex, last_vertex;
    for(boost::tie(first_vertex, last_vertex) = boost::vertices(mBoostDfg);
	first_vertex != last_vertex;
	++first_vertex) {
      delete vertex_dfg_map[*first_vertex];
    }

    // Deletes the map members.
    mBoostDfg.clear();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t DfgGraph::getId() const {
    return mId;
  };


  void DfgGraph::setName(const std::string name) {
    mName = name;
  };


  std::string DfgGraph::getName() const {
    return mName;
  };
  

  size_t DfgGraph::getNumVertices() const {
    return boost::num_vertices(mBoostDfg);
  };


  std::vector<DfgVertex*> DfgGraph::getVectorVerticesTopologicalOrder() const {
    BoostVertexDfgMapConstT vertex_dfg_map = boost::get(boost::vertex_dfg,
							mBoostDfg);
    DfgVertex* p_dfg_vertex;
    typedef std::vector< BoostVertexDescriptor > container;
    container c;
    size_t index_topo = 0;
    std::vector<DfgVertex*> result;
    boost::topological_sort(mBoostDfg,
			    std::back_inserter(c));
    for (container::iterator ii = c.begin();
	 ii != c.end();
	 ++ii) {
      p_dfg_vertex = vertex_dfg_map[*ii];
      result.push_back(p_dfg_vertex);
      p_dfg_vertex->mIdTopologicalOrder = index_topo;
      index_topo++;
    }
    return result;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgGraph::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>" << getName() << "</b></u>";
    return result.str();
  };


  std::string DfgGraph::getXmlDescription() const {
    std::ostringstream result;
    result << "      <Id>" << getId() << "</Id>" << std::endl;
    result << "      <Name>" << getName() << "</Name>" << std::endl;
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgGraph::~DfgGraph() {
    clear();
  };
    

  ///////////////////////////////////////////////////////////////////////////////////
  ////    Protected functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgGraph::getGraphmlFillColor() const {
    return "#CCCCCC";
  };



};
