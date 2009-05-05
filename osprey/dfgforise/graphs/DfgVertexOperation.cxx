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
////    Definition of DfgVertexOperation functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertexOperation.cxx
 */


#include "DfgVertexOperation.h"

#include "DfgForIse_OpCode.h"

#include "DfgExternalTypes.h"
#include "XmlUtils.h"
#include "OpClass.h"
#include "OperandResult.h"
#include "Operation.h"

#include "DfgEdge.h"
#include "DfgGraph.h"
#include "DfgVertex.h"
#include "DfgVertexConstant.h"

#include <string>
#include <sstream>
#include <iostream>
   
 

namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////
  
  DfgVertexOperation::DfgVertexOperation(const Operation& rOperation,
					 const size_t id)
    : DfgVertex(id)
    , mrOperation(rOperation) {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  VertexKindType DfgVertexOperation::getKind() const {
    return VERTEX_OPERATION;
  };


  const Operation& DfgVertexOperation::getOperation() const {
    return mrOperation;
  };


  OpCode DfgVertexOperation::getOpCode() const {
    return mrOperation.getOpCode();
  };


  OpClass DfgVertexOperation::getOpClass() const {
    return mrOperation.getOpClass();
  };


  std::string DfgVertexOperation::getOpCodeName() const {
    return mrOperation.getOpCodeName();
  };


  size_t DfgVertexOperation::getBitSize() const {
    size_t result = 0;
    std::list<DfgEdge*> list_in_edges = getListInEdges();
    std::list<DfgEdge*>::const_iterator it_edge;
    for (it_edge = list_in_edges.begin();
	 it_edge != list_in_edges.end();
	 it_edge++) {
      assert(*it_edge);
      OperandResult* p_operand_result = (*it_edge)->getOperandResult();
      if (p_operand_result) {
	result = std::max(result, p_operand_result->getBitSize());
      }
    }
    return result;
  };


  bool DfgVertexOperation::isVar() const {
    bool result = true;
    std::list<DfgEdge*> list_in_edges = getListInEdges();
    std::list<DfgEdge*>::const_iterator it_edge;
    for (it_edge = list_in_edges.begin();
	 it_edge != list_in_edges.end();
	 it_edge++) {
      assert(*it_edge);
      OperandResult* p_operand_result = (*it_edge)->getOperandResult();
      if (p_operand_result) {
	result &= p_operand_result->isVariable();
	DfgVertex* p_dfg_vertex = (*it_edge)->getSourceVertex();
	if (p_dfg_vertex->getKind() == VERTEX_OPERATION) {
	  result &= (static_cast<DfgVertexOperation*>(p_dfg_vertex)->getOpClass() != OPCLASS_MAKE );
	}
      }
    }
    return result;
  };


  size_t DfgVertexOperation::getSoftwareLatencyCycles() const {
    return mrOperation.getSoftwareLatencyCycles();
  };


  size_t DfgVertexOperation::getHardwareLatencyTime() const {
    return mrOperation.getHardwareLatencyTime(isVar(), getBitSize());
  };


  bool DfgVertexOperation::isForbiddenInSelection() const {
    if (mpEnabledSubgraph || mrOperation.isForbiddenInSelection(getBitSize())) {
      return true;
    }
    std::list<DfgEdge*> list_in_edges = getListInEdges();
    std::list<DfgEdge*>::iterator it_edge;
    for (it_edge = list_in_edges.begin();
	 it_edge != list_in_edges.end();
	 it_edge++) {
      assert(*it_edge);
      if (!((*it_edge)->getOperandResult())) {
	return true;
      }
    }
    std::list<DfgEdge*> list_out_edges = getListOutEdges();
    for (it_edge = list_out_edges.begin();
	 it_edge != list_out_edges.end();
	 it_edge++) {
      assert(*it_edge);
      if (!((*it_edge)->getOperandResult())) {
	return true;
      }
    }
    return false;
  };


  size_t DfgVertexOperation::getSourceFileLine() const {
    return mrOperation.getSourceFileLine();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Other functions.                                                       ////
  ///////////////////////////////////////////////////////////////////////////////////

  void DfgVertexOperation::duplicateMakeOperations() {
    if (getOpClass() == OPCLASS_MAKE) {
      std::list<DfgEdge*> list_in_edges = getListInEdges();

      if (list_in_edges.size() == 1) {
	std::list<DfgEdge*>::iterator it_in_edge = list_in_edges.begin();
	assert(it_in_edge != list_in_edges.end());
	assert(*it_in_edge);
	DfgVertex* previous_vertex = (*it_in_edge)->getSourceVertex();
	assert(previous_vertex);

	if (previous_vertex->getKind() == VERTEX_CONSTANT) {
	  std::list<DfgEdge*> list_out_edges = getListOutEdges();
	  std::list<DfgEdge*>::iterator it_out_edge;
	  it_out_edge = list_out_edges.begin();
	  if (it_out_edge == list_out_edges.end()) {
	    return;
	  }
	  it_out_edge++;
	  while (it_out_edge != list_out_edges.end()) {
	    assert(*it_out_edge);
	  
	    DfgVertexConstant* new_dfg_constant_vertex;
	    DfgVertexOperation* new_dfg_make_vertex;
	    DfgEdge* new_dfg_in_edge;
	    DfgEdge* new_dfg_out_edge;

	    new_dfg_constant_vertex = new DfgVertexConstant
	      (static_cast<DfgVertexConstant*>(previous_vertex)->getConstant());
	    new_dfg_make_vertex = new DfgVertexOperation(this->mrOperation);
	    new_dfg_in_edge = new DfgEdge(*it_in_edge);
	    new_dfg_out_edge = new DfgEdge(*it_out_edge);

	    mpGraph->addVertex(new_dfg_constant_vertex);
	    mpGraph->addVertex(new_dfg_make_vertex);
	    mpGraph->addEdge(new_dfg_constant_vertex,
			     new_dfg_make_vertex,
			     new_dfg_in_edge);
	    mpGraph->addEdge(new_dfg_make_vertex,
			     (*it_out_edge)->getTargetVertex(),
			     new_dfg_out_edge);

	    mpGraph->removeEdge(*it_out_edge);
	    it_out_edge++;
	  }
	}
      }
    }
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertexOperation::getGraphmlLabel() const {
    std::ostringstream result;
    result << text2xml(mrOperation.getOpCodeDump());
    return result.str();
  };


  std::string DfgVertexOperation::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Operation :</b></u>"
	   << "<br>";
    result << "OpCode : " << getOpCodeName();
    size_t source_file_line = getSourceFileLine();
    if (source_file_line) {
      result << "<br>";
      result << "Line : " << source_file_line;
    }
    size_t software_latency = getSoftwareLatencyCycles();
    if (software_latency) {
      result << "<br>";
      result << "Software latency : " << getSoftwareLatencyCycles() << " cycles";
    }
    size_t hardware_latency = getHardwareLatencyTime();
    if (hardware_latency) {
      result << "<br>";
      result << "Hardware latency : " << getHardwareLatencyTime() << " ns.10-2";
    }
    return result.str();
  };


  std::string DfgVertexOperation::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Id>" << mId << "</Id>" << std::endl;
    result << "          <Kind>" << VERTEX_KIND_TYPE_NAME[VERTEX_OPERATION] << "</Kind>" << std::endl;
    result << getOperation().getXmlDescription();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertexOperation::~DfgVertexOperation() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertexOperation::getGraphmlShape() const {
    return "ellipse";
  };


  std::string DfgVertexOperation::getGraphmlFillColor() const {
    std::ostringstream result;
    result << "#00CC00";
    if (isForbiddenInSelection() && !mpEnabledSubgraph) {
      result << "22";
    }
    return result.str();
  };



};
