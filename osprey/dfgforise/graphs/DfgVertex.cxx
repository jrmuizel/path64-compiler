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
////    Definition of DfgVertex functions.                                       ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertex.cxx
 */


#include "DfgVertex.h"

#include "DfgExternalTypes.h"
#include "GraphmlTypes.h"
#include "XmlUtils.h"

#include "DumpToGraphml.h"

#include "DfgEdge.h"
#include "DfgGraph.h"
#include "DfgInternalTypes.h"
#include "Subgraph.h"

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <list>
   
 

namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertex::DfgVertex(const size_t id) {
    mId = id;
    msNextId = std::max(msNextId, mId+1);
    msTableId[mId] = this;
    mDefaultName = "--";
    mpGraph = 0;
    mpEnabledSubgraph = 0;
    mIdTopologicalOrder = 0;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t DfgVertex::getId() const {
    return mId;
  };


  size_t DfgVertex::getNextId() {
    return msNextId;
  };


  DfgVertex* DfgVertex::getVertexById(const size_t id) {
    std::map<size_t, DfgVertex*>::iterator it_vertex
      = msTableId.find(id);
    if (it_vertex == msTableId.end()) {
      return 0;
    }
    assert(it_vertex->second);
    return (it_vertex->second);
  };


  VertexKindType DfgVertex::getKind() const {
    return VERTEX_UNDEFINED;
  };


  void DfgVertex::setDefaultName(const std::string defaultName) {
    mDefaultName = defaultName;
  };


  size_t DfgVertex::getSoftwareLatencyCycles() const {
    return 0;
  };


  size_t DfgVertex::getHardwareLatencyTime() const {
    return 0;
  };


  bool DfgVertex::isForbiddenInSelection() const {
    return true;
  };


  size_t DfgVertex::getIdTopologicalOrder() const {
    return mIdTopologicalOrder;
  };


  std::list<DfgEdge*> DfgVertex::getListInEdges() const {
    BoostEdgeDfgMapT edge_dfg_map = boost::get(boost::edge_dfg,
					       mpGraph->mBoostDfg);
    BoostInEdgeIterator first_in_edge, last_in_edge;
    std::list<DfgEdge*> result;
    DfgEdge* p_current_edge;
    for (boost::tie(first_in_edge, last_in_edge) = boost::in_edges(mBoostVertex,mpGraph->mBoostDfg);
	 first_in_edge != last_in_edge;
	 ++first_in_edge) {
      p_current_edge = edge_dfg_map[*first_in_edge];
      assert(p_current_edge);
      if (p_current_edge->isRealDependency()) {
	result.push_back(p_current_edge);
      }
    }
    return result;
  };

  
  std::list<DfgEdge*> DfgVertex::getListOutEdges() const {
    BoostEdgeDfgMapT edge_dfg_map = boost::get(boost::edge_dfg,
					       mpGraph->mBoostDfg);
    BoostOutEdgeIterator first_out_edge, last_out_edge;
    std::list<DfgEdge*> result;
    DfgEdge* p_current_edge;
    for (boost::tie(first_out_edge, last_out_edge) = boost::out_edges(mBoostVertex,mpGraph->mBoostDfg);
	 first_out_edge != last_out_edge;
	 ++first_out_edge) {
      p_current_edge = edge_dfg_map[*first_out_edge];
      assert(p_current_edge);
      if (p_current_edge->isRealDependency()) {
	result.push_back(p_current_edge);
      }
    }
    return result;
  };


  std::list<DfgVertex*> DfgVertex::getListPredecessorVertices() const {
    BoostEdgeDfgMapT edge_dfg_map = boost::get(boost::edge_dfg,
					       mpGraph->mBoostDfg);
    BoostInEdgeIterator first_in_edge, last_in_edge;
    std::list<DfgVertex*> result;
    DfgEdge* p_current_edge;
    for (boost::tie(first_in_edge, last_in_edge) = boost::in_edges(mBoostVertex,mpGraph->mBoostDfg);
	 first_in_edge != last_in_edge;
	 ++first_in_edge) {
      p_current_edge = edge_dfg_map[*first_in_edge];
      assert(p_current_edge);
      if (p_current_edge->isRealDependency()) {
	result.push_back(p_current_edge->getSourceVertex());
      }
    }
    return result;
  };


  std::list<DfgVertex*> DfgVertex::getListSuccessorVertices() const {
    BoostEdgeDfgMapT edge_dfg_map = boost::get(boost::edge_dfg,
					       mpGraph->mBoostDfg);
    BoostOutEdgeIterator first_out_edge, last_out_edge;
    std::list<DfgVertex*> result;
    DfgEdge* p_current_edge;
    for (boost::tie(first_out_edge, last_out_edge) = boost::out_edges(mBoostVertex,mpGraph->mBoostDfg);
	 first_out_edge != last_out_edge;
	 ++first_out_edge) {
      p_current_edge = edge_dfg_map[*first_out_edge];
      assert(p_current_edge);
      if (p_current_edge->isRealDependency()) {
	result.push_back(p_current_edge->getTargetVertex());
      }
    }
    return result;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Other functions.                                                       ////
  ///////////////////////////////////////////////////////////////////////////////////

  void DfgVertex::duplicateMakeOperations() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertex::getGraphmlId() const {
    std::ostringstream result;
    if (mpEnabledSubgraph) {
      result << mpEnabledSubgraph->getGraphmlId() << "::";
    }
    else if (mpGraph) {
      result << mpGraph->getGraphmlId() << "::";
    }
    result << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_VERTEX] << getId();
    return result.str();
  };


  std::string DfgVertex::getGraphmlLabel() const {
    return text2xml(mDefaultName);
  };


  std::string DfgVertex::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Undefined vertex</b></u>"
	   << "<br>";
    result << mDefaultName;
    return result.str();
  };


  std::string DfgVertex::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Id>" << mId << "</Id>" << std::endl;
    result << "          <Kind>" << VERTEX_KIND_TYPE_NAME[VERTEX_UNDEFINED] << "</Kind>" << std::endl;
    result << "          <DefaultName>" << text2xml(mDefaultName)
	   << "</DefaultName>" << std::endl;
    return result.str();
  };


  void DfgVertex::generateDumpGraphml(DumpToGraphml& rOs) const {
    rOs << "      <node id=\"" << getGraphmlId() << "\">"
	<< std::endl;


    if (rOs.isEnabledPrintFormating()) {
      rOs << "        <data key="
	  << "\"" << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_VERTEX] << "\">"
	  << std::endl;
      rOs << "          <y:ShapeNode>"
	  << std::endl;
      rOs << "            <y:Fill color=\"" << getGraphmlFillColor() << "\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "            <y:BorderStyle color=\"#000000\""
	  << " type=\"line\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "            <y:NodeLabel alignment=\"center\""
	  << " autoSizePolicy=\"content\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"14\""
	  << " fontStyle=\"plain\""
	  << " hasBackgroundColor=\"false\""
	  << " hasLineColor=\"false\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"c\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << getGraphmlLabel()
	  << "</y:NodeLabel>"
	  << std::endl;
      rOs << "            <y:Shape type=\"" << getGraphmlShape() << "\"/>"
	  << std::endl;
      rOs << "          </y:ShapeNode>"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
      rOs << "        <data key=\"node_data\">"
	  << text2xml(getHtmlDescription())
	  << "</data>" << std::endl;
    }// end dumpFormating
    else {
      rOs << "        <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_VERTEX] << "\">"
	  << std::endl;
      rOs << "          <y:ShapeNode>"
	  << std::endl;
      rOs << "            <y:NodeLabel>"
	  << getGraphmlLabel()
	  << "</y:NodeLabel>"
	  << std::endl;
      rOs << "          </y:ShapeNode>"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
    }


    rOs << "        <DFGforISE>"
	<< std::endl;
    rOs << getXmlDescription();
    rOs << "        </DFGforISE>"
	<< std::endl;
    rOs << "      </node>"
	<< std::endl;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertex::~DfgVertex() {
    msTableId.erase(mId);
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Protected functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertex::getGraphmlShape() const {
    return "hexagon";
  };


  std::string DfgVertex::getGraphmlFillColor() const {
    return "#DDDDDD";
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Protected fields.                                                      ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::map<size_t, DfgVertex*> DfgVertex::msTableId;


  size_t DfgVertex::msNextId = 0;


  VertexKindType GetVertexKindFromName(const std::string name) {
    // Uses a static map initializated once : links an opcode name to its enumeration
    static std::map<std::string, VertexKindType> s_map_name_vertex_kind;
    if (s_map_name_vertex_kind.empty()) {
      for (size_t i = 0;
	   i < VERTEX_INVALID;
	   i++) {
	s_map_name_vertex_kind[VERTEX_KIND_TYPE_NAME[i]] = VertexKindType(i);
      };
    }
    std::map<std::string, VertexKindType>::const_iterator it_vertex_kind = s_map_name_vertex_kind.find(name);
    if (it_vertex_kind == s_map_name_vertex_kind.end()) {
      return VERTEX_INVALID;
    }
    return it_vertex_kind->second;
  };



};
