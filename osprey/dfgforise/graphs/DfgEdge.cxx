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
////    Definition of DfgEdge functions.                                         ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgEdge.cxx
 */


#include "DfgEdge.h"

#include "OperandResult.h"
#include "GraphmlTypes.h"
#include "XmlUtils.h"

#include "DumpToGraphml.h"

#include "DfgGraph.h"
#include "DfgInternalTypes.h"
#include "DfgVertex.h"
#include "Subgraph.h"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
   
 

namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgEdge::DfgEdge(EdgeKindType edgeKind) {
    mKind = edgeKind;
    mOperandRankInTargetOperation = -1;
    mOperandIsFalsePredication = false;
    mResultRankInSourceOperation = -1;
    mpOperandResult = 0;
    mpGraph = 0;
    mpEnabledSubgraph = 0;
  };


  DfgEdge::DfgEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    mKind = pDfgEdge->mKind;
    mOperandRankInTargetOperation = pDfgEdge->mOperandRankInTargetOperation;
    mOperandIsFalsePredication = pDfgEdge->mOperandIsFalsePredication;
    mResultRankInSourceOperation = pDfgEdge->mResultRankInSourceOperation;
    mpOperandResult = pDfgEdge->mpOperandResult;
    mpGraph = 0;
    mpEnabledSubgraph = 0;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  EdgeKindType DfgEdge::getKind() const {
    return mKind;
  };


  DfgVertex* DfgEdge::getSourceVertex() const {
    assert(mpGraph);
    BoostVertexDescriptor source_vertex = boost::source(mBoostEdge,
							mpGraph->mBoostDfg);
    DfgVertex* p_dfg_vertex = boost::get(boost::vertex_dfg,
					 mpGraph->mBoostDfg,
					 source_vertex);
    assert(p_dfg_vertex);
    return p_dfg_vertex;
  };


  DfgVertex* DfgEdge::getTargetVertex() const {
    assert(mpGraph);
    BoostVertexDescriptor target_vertex = boost::target(mBoostEdge,
							mpGraph->mBoostDfg);
    DfgVertex* p_dfg_vertex = boost::get(boost::vertex_dfg,
					 mpGraph->mBoostDfg,
					 target_vertex);
    assert(p_dfg_vertex);
    return p_dfg_vertex;
  };


  void DfgEdge::setOperandRankInTargetOperation(int operandRankInTargetOperation) {
    mOperandRankInTargetOperation = operandRankInTargetOperation;
  };


  int DfgEdge::getOperandRankInTargetOperation() const {
    return mOperandRankInTargetOperation;
  };


  void DfgEdge::setOperandIsFalsePredication(const bool operandIsFalsePredication) {
    mOperandIsFalsePredication = operandIsFalsePredication;
  };


  bool DfgEdge::getOperandIsFalsePredication() const {
    return mOperandIsFalsePredication;
  };


  void DfgEdge::setResultRankInSourceOperation(int resultRankInSourceOperation) {
    mResultRankInSourceOperation = resultRankInSourceOperation;
  };


  int DfgEdge::getResultRankInSourceOperation() const {
    return mResultRankInSourceOperation;
  };


  void DfgEdge::setOperandResult(OperandResult* pOperandResult) {
    mpOperandResult = pOperandResult;
  };


  OperandResult* DfgEdge::getOperandResult() const {
    return mpOperandResult;
  };


  bool DfgEdge::isRealDependency() const {
    return ((mKind == EDGE_DEP_REGIN)
	    || (mKind == EDGE_DEP_MEMIN)
	    || ((mKind == EDGE_DEP_MISC) && mpOperandResult)
	    || (mKind == EDGE_IN_UNDEFINED)
	    || (mKind == EDGE_IN_CONSTANT)
	    || (mKind == EDGE_IN_VARIABLE)
	    || (mKind == EDGE_OUT_VARIABLE));
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgEdge::getGraphmlLabel() const {
    std::ostringstream result;
    if (mOperandIsFalsePredication) {
      result << "! ";
    }
    result << getOperandRankInTargetOperation();
    return result.str();
  };


  std::string DfgEdge::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Dependency :</b></u>"
	   << "<br>";
    result << "Kind : " << EDGE_KIND_TYPE_NAME[mKind]
	   << "<br>";
    result << "Source : " << (getSourceVertex())->getGraphmlLabel();
    if (mResultRankInSourceOperation >= 0) {
      result << "    (res : " << mResultRankInSourceOperation << ")";
    }
    result << "<br>";
    result << "Target : " << (getTargetVertex())->getGraphmlLabel();
    if (mOperandRankInTargetOperation >= 0) {
      result << "    (opnd : ";
      if (mOperandIsFalsePredication) {
	result << "! ";
      }
      result << mOperandRankInTargetOperation << ")";
    }
    if (mpOperandResult) {
      result << "<br>";
      result << getOperandResult()->getHtmlDescription();
    }
    return result.str();
  };


  std::string DfgEdge::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Kind>" << EDGE_KIND_TYPE_NAME[mKind] << "</Kind>" << std::endl;
    result << "          <IdSourceVertex>" << (getSourceVertex())->getId()
	   << "</IdSourceVertex>" << std::endl;
    result << "          <IdTargetVertex>" << (getTargetVertex())->getId()
	   << "</IdTargetVertex>" << std::endl;
    result << "          <OperandRankInTargetOperation>"
	   << mOperandRankInTargetOperation
	   << "</OperandRankInTargetOperation>" << std::endl;
    result << "          <OperandIsFalsePredication>"
	   << mOperandIsFalsePredication
	   << "</OperandIsFalsePredication>" << std::endl;
    result << "          <ResultRankInSourceOperation>"
	   << mResultRankInSourceOperation
	   << "</ResultRankInSourceOperation>" << std::endl;
    if (mpOperandResult) {
      result << mpOperandResult->getXmlDescription();
    }
    return result.str();
  };


  void DfgEdge::generateDumpGraphml(DumpToGraphml& rOs) const {
    if ((!isRealDependency()) && (!rOs.isEnabledPrintNonRealEdges())) {
      return;
    }
    if (!rOs.isEnabledPrintEdgeKind(mKind)) {
      return;
    }

    rOs << "      <edge source=" << '"' << (getSourceVertex())->getGraphmlId() << '"'
	<< " target=" << '"' << (getTargetVertex())->getGraphmlId() << '"' << ">"
	<< std::endl;


    if (rOs.isEnabledPrintFormating()) {
      rOs << "        <data key=" << '"'
	  << GRAPHML_EDGE_TYPE_NAME[EDGE_TYPE_DFG] << '"' << ">"
	  << std::endl;
      rOs << "          <y:" << getGraphmlLineType() << ">"
	  << std::endl;
      rOs << "            <y:Path/>"
	  << std::endl;
      rOs << "            <y:LineStyle color=" << '"' << getGraphmlLineColor() << "BF" << '"'
	  << " type=" << '"' << "line" << '"'
	  << " width=" << '"' << "1.0" << '"' << "/>"
	  << std::endl;
      rOs << "            <y:Arrows source=" << '"' << "none" << '"'
	  << " target=" << '"' << "standard" << '"' << "/>"
	  << std::endl;
      if (mOperandRankInTargetOperation >= 0) {
	rOs << "            <y:EdgeLabel alignment=" << '"' << "center" << '"'
	    << " backgroundColor=" << '"' << "#FFFFFFBF" << '"'
	    << " distance=" << '"' << "2.0" << '"'
	    << " fontFamily=" << '"' << "Dialog" << '"'
	    << " fontSize=" << '"' << "12" << '"'
	    << " fontStyle=" << '"' << "plain" << '"'
	    << " lineColor=" << '"' << "#9999CC" << '"'
	    << " modelName=" << '"' << "three_center" << '"'
	    << " modelPosition=" << '"' << "tcentr" << '"'
	    << " preferredPlacement=" << '"' << "target" << '"'
	    << " ratio=" << '"' << "0.5" << '"'
	    << " textColor=" << '"' << "#000000" << '"'
	    << " visible=" << '"' << "true" << '"' << ">"
	    << getGraphmlLabel()
	    << "</y:EdgeLabel>"
	    << std::endl;
      }
      rOs << "            <y:BendStyle smoothed=" << '"' << "true" << '"' << "/>"
	  << std::endl;
      rOs << "          </y:" << getGraphmlLineType() << ">"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
      rOs << "        <data key=" << '"' << "edge_data" << '"' << ">"
	  << text2xml(getHtmlDescription())
	  << "</data>"
	  << std::endl;
    }// end dumpFormating
    else {
      rOs << "        <data key=" << '"'
	  << GRAPHML_EDGE_TYPE_NAME[EDGE_TYPE_DFG] << '"' << ">"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
    }


    rOs << "        <DFGforISE>"
	<< std::endl;
    rOs << getXmlDescription();
    rOs << "        </DFGforISE>"
	<< std::endl;
    rOs << "      </edge>"
	<< std::endl;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgEdge::~DfgEdge() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgEdge::getGraphmlLineColor() const {
    if ((mKind == EDGE_DEP_REGIN)
	|| (mKind == EDGE_DEP_MEMIN)
	|| (mKind == EDGE_IN_UNDEFINED)
	|| (mKind == EDGE_IN_CONSTANT)
	|| (mKind == EDGE_IN_VARIABLE)
	|| (mKind == EDGE_OUT_VARIABLE)) {
      return "#000000";
    }
    else if ((mKind == EDGE_DEP_REGOUT)
	     || (mKind == EDGE_DEP_MEMOUT)) {
      return "#33CC33";
    }
    else if ((mKind == EDGE_DEP_REGANTI)
	     || (mKind == EDGE_DEP_MEMANTI)) {
      return "#3333CC";
    }
    else {
      return "#CC3333";
    }
  };


  std::string DfgEdge::getGraphmlLineType() const {
    return "PolyLineEdge";
  };


  EdgeKindType GetEdgeKindFromName(const std::string name) {
    // Uses a static map initializated once : links an opcode name to its enumeration
    static std::map<std::string, EdgeKindType> s_map_name_edge_kind;
    if (s_map_name_edge_kind.empty()) {
      for (size_t i = 0;
	   i < EDGE_INVALID;
	   i++) {
	s_map_name_edge_kind[EDGE_KIND_TYPE_NAME[i]] = EdgeKindType(i);
      };
    }
    std::map<std::string, EdgeKindType>::const_iterator it_edge_kind = s_map_name_edge_kind.find(name);
    if (it_edge_kind == s_map_name_edge_kind.end()) {
      return EDGE_INVALID;
    }
    return it_edge_kind->second;
  };



};
