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
////    Definition of BasicBlock functions.                                      ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file BasicBlock.cxx
 */


#include "BasicBlock.h"

#include "GraphmlTypes.h"
#include "XmlUtils.h"

#include "DumpToGraphml.h"

#include "DfgEdge.h"
#include "DfgGraph.h"
#include "DfgVertex.h"
#include "Function.h"
#include "Subgraph.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  BasicBlock::BasicBlock(const size_t id) {
    mId = id;
    msNextId = std::max(msNextId, mId+1);
    {
      std::ostringstream name;
      name << "BB " << mId;
      mName = name.str();
    }
    mFrequency = DEFAULT_BASIC_BLOCK_FREQUENCY;
    mpFunction = 0;
    mLabels = "";
    mSourceFileLine = 0;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void BasicBlock::enableSubgraph(Subgraph* pSubgraph) {
    assert(pSubgraph);
    std::set<DfgVertex*>::iterator it_vertex;
    DfgVertex* p_dfg_vertex;
    assert(&(pSubgraph->mrBasicBlock) == this);
    for (it_vertex = (pSubgraph->mSetVertices).begin();
	 it_vertex != (pSubgraph->mSetVertices).end();
	 it_vertex++) {
      p_dfg_vertex = *it_vertex;
      assert(p_dfg_vertex);
      if (p_dfg_vertex->mpEnabledSubgraph) {
	assert(p_dfg_vertex->mpEnabledSubgraph == pSubgraph);
	return;
      }
    }
    for (it_vertex = (pSubgraph->mSetVertices).begin();
	 it_vertex != (pSubgraph->mSetVertices).end();
	 it_vertex++) {
      p_dfg_vertex = *it_vertex;
      assert(p_dfg_vertex);
      p_dfg_vertex->mpEnabledSubgraph = pSubgraph;
    }
    std::set<DfgEdge*>::iterator it_edge;
    for (it_edge = (pSubgraph->mSetEdges).begin();
	 it_edge != (pSubgraph->mSetEdges).end();
	 it_edge++) {
      assert(*it_edge);
      (*it_edge)->mpEnabledSubgraph = pSubgraph;
    }
    mSetEnabledSubgraphs.insert(pSubgraph);
  };


  void BasicBlock::enableSubgraph(const size_t idSubgraph) {
    Subgraph* p_subgraph = Subgraph::getSubgraphById(idSubgraph);
    enableSubgraph(p_subgraph);
  };


  void BasicBlock::disableAllSubgraphs() {
    BoostVertexDfgMapT vertex_dfg_map = boost::get(boost::vertex_dfg, mBoostDfg);
    BoostVertexIterator first_vertex, last_vertex;
    for(boost::tie(first_vertex, last_vertex) = boost::vertices(mBoostDfg);
	first_vertex != last_vertex;
	++first_vertex) {
      DfgVertex* p_dfg_vertex = vertex_dfg_map[*first_vertex];
      assert(p_dfg_vertex);
      p_dfg_vertex->mpEnabledSubgraph = 0;
    }
    mSetEnabledSubgraphs.clear();
  };


  void BasicBlock::duplicateMakeOperations() {
    BoostVertexDfgMapT vertex_dfg_map = boost::get(boost::vertex_dfg,
							mBoostDfg);
    BoostVertexIterator first_vertex, last_vertex;
    for(boost::tie(first_vertex, last_vertex) = boost::vertices(mBoostDfg);
	first_vertex != last_vertex;
	++first_vertex) {
      DfgVertex* p_dfg_vertex = vertex_dfg_map[*first_vertex];
      assert(p_dfg_vertex);
      p_dfg_vertex->duplicateMakeOperations();
    }
  };


  void BasicBlock::clear() {

    // Deletes the corresponding subgraphs.
    std::set<Subgraph*>::const_iterator it_subgraph;
    for (it_subgraph = mSetEnabledSubgraphs.begin();
	 it_subgraph != mSetEnabledSubgraphs.end();
	 it_subgraph++) {
      delete (*it_subgraph);
    }

    // Deletes the map members.
    mSetEnabledSubgraphs.clear();

    // Deletes the other datas.
    DfgGraph::clear();

  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t BasicBlock::getNextId() {
    return msNextId;
  };


  void BasicBlock::setFrequency(const size_t frequency) {
    mFrequency = frequency;
  };


  size_t BasicBlock::getFrequency() const {
    return mFrequency;
  };


  size_t BasicBlock::getMaximumFrequency() const {
    if (!mpFunction) {
      return 0;
    }
    return mpFunction->getMaximumFrequency();
  };


  Function* BasicBlock::getFunction() const {
    return mpFunction;
  };


  std::string BasicBlock::getFunctionName() const {
    if (!mpFunction) {
      return "";
    }
    return mpFunction->getName();
  };


  std::string BasicBlock::getSourceFileName() const {
    if (!mpFunction) {
      return "";
    }
    return mpFunction->getSourceFileName();
  };


  void BasicBlock::setLabels(const std::string labels) {
    mLabels = labels;
  };


  std::string BasicBlock::getLabels() const {
    return mLabels;
  };


  void BasicBlock::setSourceFileLine(const size_t sourceFileLine) {
    mSourceFileLine = sourceFileLine;
  };


  size_t BasicBlock::getSourceFileLine() const {
    return mSourceFileLine;
  };


  std::set<Subgraph*> BasicBlock::getSetEnabledSubgraphs() const {
    return mSetEnabledSubgraphs;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string BasicBlock::getGraphmlId() const {
    std::ostringstream result;
    if (mpFunction) {
      result << mpFunction->getGraphmlId() << "::";
    }
    result << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_BASICBLOCK] << mId;
    return result.str();
  };


  std::string BasicBlock::getGraphmlLabel() const {
    std::ostringstream result;
    result << getFunctionName() << " : " << mName;
    return result.str();
  };


  std::string BasicBlock::getHtmlDescription() const {
    std::ostringstream result;
    std::string tmp_str = "";
    result << DfgGraph::getHtmlDescription() << "<br>";
    result << "Frequency : " << getFrequency();
    tmp_str = getFunctionName();
    if (!(tmp_str.empty())) {
      result << "<br>";
      result << "Function : " << tmp_str;
    }
    tmp_str = getSourceFileName();
    if (!(tmp_str.empty())) {
      result << "<br>";
      result << "Source File : " << tmp_str;
    }
    if (mSourceFileLine) {
      result << "<br>";
      result << "Line in source : " << mSourceFileLine;
    }
    tmp_str = getLabels();
    if (!(tmp_str.empty())) {
      result << "<br>";
      result << "Labeled with : " << tmp_str;      
    }
    return result.str();
  };


  std::string BasicBlock::getXmlDescription() const {
    std::ostringstream result;
    result << DfgGraph::getXmlDescription() << std::endl;
    result << "      <Frequency>" << mFrequency << "</Frequency>" << std::endl;
    result << "      <SourceFileLine>" << mSourceFileLine
	   << "</SourceFileLine>" << std::endl;
    result << "      <Labels>" << mLabels << "</Labels>" << std::endl;
    return result.str();
  };


  void BasicBlock::generateDumpGraphml(DumpToGraphml& rOs) const {

    std::set<Subgraph *>::const_iterator it_subgraph;
    std::string fill_color = getGraphmlFillColor();

    // Prints the GraphML description of the graph (basic block).
    rOs << "  <node id=\"" << getGraphmlId() << "\""
	<< " yfiles.foldertype=\"group\">"
	<< std::endl;


    if (rOs.isEnabledPrintFormating()) {
      rOs << "    <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_BASICBLOCK] << "\">"
	  << std::endl;
      rOs << "      <y:ProxyAutoBoundsNode>"
	  << std::endl;
      rOs << "        <y:Realizers active=\"0\">"
	  << std::endl;
      rOs << "          <y:GroupNode>"
	  << std::endl;
      rOs << "            <y:Fill color=\"" << fill_color << "07\""
	  << " color2=\"" << fill_color << "20\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "            <y:BorderStyle color=\"#000000\""
	  << " type=\"line\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "            <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"" << fill_color << "\""
	  << " configuration=\"CroppingLabel\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"18\""
	  << " fontStyle=\"plain\""
	  << " lineColor=\"#999999\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"t\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << getGraphmlLabel()
	  << "</y:NodeLabel>"
	  << std::endl;
      rOs << "            <y:Shape type=\"rectangle\"/>"
	  << std::endl;
      rOs << "            <y:State closed=\"false\""
	  << " innerGraphDisplayEnabled=\"false\"/>"
	  << std::endl;
      rOs << "            <y:NodeBounds considerNodeLabelSize=\"true\"/>"
	  << std::endl;
      rOs << "            <y:Insets bottom=\"15\""
	  << " left=\"15\""
	  << " right=\"15\""
	  << " top=\"15\"/>"
	  << std::endl;
      rOs << "            <y:BorderInsets bottom=\"0\""
	  << " left=\"1\""
	  << " right=\"1\""
	  << " top=\"0\"/>"
	  << std::endl;
      rOs << "          </y:GroupNode>"
	  << std::endl;
      rOs << "          <y:GroupNode>"
	  << std::endl;
      rOs << "            <y:Fill color=\"" << fill_color << "07\""
	  << " color2=\"" << fill_color << "0F\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "            <y:BorderStyle color=\"#000000\""
	  << " type=\"line\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "            <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"" << fill_color << "\""
	  << " configuration=\"CroppingLabel\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"18\""
	  << " fontStyle=\"plain\""
	  << " lineColor=\"#999999\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"t\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << getGraphmlLabel()
	  << "</y:NodeLabel>"
	  << std::endl;
      rOs << "            <y:Shape type=\"rectangle\"/>"
	  << std::endl;
      rOs << "            <y:State closed=\"true\""
	  << " innerGraphDisplayEnabled=\"false\"/>"
	  << std::endl;
      rOs << "            <y:NodeBounds considerNodeLabelSize=\"true\"/>"
	  << std::endl;
      rOs << "            <y:Insets bottom=\"15\""
	  << " left=\"15\""
	  << " right=\"15\""
	  << " top=\"15\"/>"
	  << std::endl;
      rOs << "            <y:BorderInsets bottom=\"0\""
	  << " left=\"0\""
	  << " right=\"0\""
	  << " top=\"0\"/>"
	  << std::endl;
      rOs << "          </y:GroupNode>"
	  << std::endl;
      rOs << "        </y:Realizers>"
	  << std::endl;
      rOs << "      </y:ProxyAutoBoundsNode>"
	  << std::endl;
      rOs << "    </data>"
	  << std::endl;
      rOs << "    <data key=\"node_data\">"
	  << text2xml(getHtmlDescription())
	  << "</data>"
	  << std::endl;
    }// end dumpFormating
    else {
      rOs << "        <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_BASICBLOCK] << "\">"
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


    rOs << "    <DFGforISE>"
	<< std::endl;
    rOs << getXmlDescription();
    rOs << "    </DFGforISE>"
	<< std::endl;
    rOs << "    <graph edgedefault=\"directed\""
	<< " id=\"" << getGraphmlId() << ":\">"
	<< std::endl;

    
    // Prints the GraphML description of subgraphs and their vertices and 
    // internal edges.
    for (it_subgraph = mSetEnabledSubgraphs.begin();
	 it_subgraph != mSetEnabledSubgraphs.end();
	 it_subgraph++) {
      assert(*it_subgraph);
      (*it_subgraph)->generateDumpGraphml(rOs);
    }

    // Prints the GraphML description of vertices (which do not belong to an enabled
    // subgraph).
    BoostVertexDfgMapConstT vertex_dfg_map = boost::get(boost::vertex_dfg,
							mBoostDfg);
    BoostVertexIterator first_vertex, last_vertex;
    for(boost::tie(first_vertex, last_vertex) = boost::vertices(mBoostDfg);
	first_vertex != last_vertex;
	++first_vertex) {
      DfgVertex* p_dfg_vertex = vertex_dfg_map[*first_vertex];
      assert(p_dfg_vertex);
      if (!(p_dfg_vertex->mpEnabledSubgraph)) {
	p_dfg_vertex->generateDumpGraphml(rOs);
      }
    }

    // Prints the GraphML description of edges (which do not belong to an enabled
    // subgraph).
    BoostEdgeDfgMapConstT edge_dfg_map = boost::get(boost::edge_dfg, mBoostDfg);
    BoostEdgeIterator first_edge, last_edge;
    for(boost::tie(first_edge, last_edge) = boost::edges(mBoostDfg);
	first_edge != last_edge;
	++first_edge) {
      DfgEdge* p_dfg_edge = edge_dfg_map[*first_edge];
      assert(p_dfg_edge);
      if (!(p_dfg_edge->mpEnabledSubgraph)) {
	p_dfg_edge->generateDumpGraphml(rOs);
      }
    }

    rOs << "    </graph>" << std::endl;
    rOs << "  </node>" << std::endl;

  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  BasicBlock::~BasicBlock() {
  };
    

  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string BasicBlock::getGraphmlFillColor() const {
    size_t max_frequency = getMaximumFrequency();
    if ( (mFrequency <= 0) || (mFrequency > max_frequency) ) {
      return "#CCCCCC";
    }
    else {
      std::ostringstream result;
      short green_intensity = (short)(255 * (max_frequency - mFrequency)
				      / max_frequency);
      result << "#FF" << std::hex;
      result.width(2);
      result.fill('0');
      result << green_intensity << std::dec;
      result << "00";
      return result.str();
    }
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private fields.                                                        ////
  ///////////////////////////////////////////////////////////////////////////////////
 
  size_t BasicBlock::msNextId = 0;



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Definition of BasicBlockComparison class and functions.                  ////
  ///////////////////////////////////////////////////////////////////////////////////

  BasicBlockComparison::BasicBlockComparison() {
  };


  bool BasicBlockComparison::operator() (const BasicBlock* lhs,
					 const BasicBlock* rhs) const {
    assert(lhs);
    assert(rhs);
    size_t lhs_frequency = lhs->getFrequency();
    size_t rhs_frequency = rhs->getFrequency();
    if (lhs_frequency == rhs_frequency) {
      Function* lhs_function = lhs->getFunction();
      Function* rhs_function = rhs->getFunction();
      if (lhs_function && rhs_function) {
	if (lhs_function->getId() != rhs_function->getId()) {
	  return (lhs_function->getId() > rhs_function->getId());
	}
      }
      return (lhs->getId() > rhs->getId());
    }
    else {
      return (lhs_frequency < rhs_frequency);
    }
  };



};
