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
////    Definition of SubgraphMeritData functions.                               ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file SubgraphMeritData.cxx
 */


#include "SubgraphMeritData.h"

#include "OperandResult.h"
#include "BasicBlock.h"
#include "DfgEdge.h"
#include "DfgVertex.h"
#include "Subgraph.h"

#include <string>
#include <map>
#include <set>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  SubgraphMeritData::SubgraphMeritData(const size_t clockPeriod) {
    mSoftwareLatencyCycles = 0;
    mHardwareLatencyTime = 0;
    mFrequency = 1;
    mClockPeriod = clockPeriod;
  };

  SubgraphMeritData::SubgraphMeritData(const SubgraphMeritData& rMeritData) {
    mSetOutputEdges = rMeritData.mSetOutputEdges;
    mSetInputEdges = rMeritData.mSetInputEdges;
    mSoftwareLatencyCycles = rMeritData.mSoftwareLatencyCycles;
    mHardwareLatencyTime = rMeritData.mHardwareLatencyTime;
    mFrequency = rMeritData.mFrequency;
    mClockPeriod = rMeritData.mClockPeriod;
  };

  SubgraphMeritData::SubgraphMeritData(const Subgraph& rSubgraph,
				       const size_t clockPeriod) {
    mSoftwareLatencyCycles = 0;
    mHardwareLatencyTime = 0;
    mFrequency = 1;

    if (!(rSubgraph.getBasicBlock())) {
      return;
    }

    std::set<DfgVertex*> set_vertices = rSubgraph.mSetVertices;
    std::set<DfgVertex*>::iterator it_vertex;
    std::list<DfgEdge*> list_edges ;
    std::list<DfgEdge*>::iterator it_edge;

    for (it_vertex = set_vertices.begin() ;
	 it_vertex != set_vertices.end() ;
	 it_vertex++) {
      assert(*it_vertex);
      list_edges = (*it_vertex)->getListOutEdges();
      for (it_edge = list_edges.begin() ;
	   it_edge != list_edges.end() ;
	   it_edge++) {
	assert(*it_edge);
	if (set_vertices.find((*it_edge)->getTargetVertex())
	    == set_vertices.end()) {
	  addOutputEdge(*it_edge);
	}
      }
      list_edges = (*it_vertex)->getListInEdges();
      for (it_edge = list_edges.begin() ;
	   it_edge != list_edges.end() ;
	   it_edge++) {
	assert(*it_edge);
	if (set_vertices.find((*it_edge)->getSourceVertex())
	    == set_vertices.end()) {
	  addInputEdge(*it_edge);
	}
      }
      mSoftwareLatencyCycles += (*it_vertex)->getSoftwareLatencyCycles();
      mHardwareLatencyTime = std::max(mHardwareLatencyTime,
				  getHardwareLatencyTimeCriticalPath(*it_vertex, rSubgraph));
    }
    mFrequency = rSubgraph.getBasicBlock()->getFrequency();
    mClockPeriod = clockPeriod;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void SubgraphMeritData::copy(const SubgraphMeritData& rMeritData) {
    mSetOutputEdges.clear();
    mSetOutputEdges = rMeritData.mSetOutputEdges;
    mSetInputEdges.clear();
    mSetInputEdges = rMeritData.mSetInputEdges;
    mSetOutputPorts.clear();
    mSetOutputPorts = rMeritData.mSetOutputPorts;
    mMapNumOutputPorts.clear();
    mMapNumOutputPorts = rMeritData.mMapNumOutputPorts;
    mSetInputPorts.clear();
    mSetInputPorts = rMeritData.mSetInputPorts;
    mMapNumInputPorts.clear();
    mMapNumInputPorts = rMeritData.mMapNumInputPorts;
    mSoftwareLatencyCycles = rMeritData.mSoftwareLatencyCycles;
    mHardwareLatencyTime = rMeritData.mHardwareLatencyTime;
    mFrequency = rMeritData.mFrequency;
    mClockPeriod = rMeritData.mClockPeriod;
  };


  void SubgraphMeritData::addOutputEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    mSetOutputEdges.insert(pDfgEdge);
    OperandResult* p_operand_result;
    p_operand_result = pDfgEdge->getOperandResult();
    if (p_operand_result) {
      mSetOutputPorts.insert(p_operand_result);
      (mMapNumOutputPorts[p_operand_result])++;
    }
  };


  void SubgraphMeritData::removeOutputEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    mSetOutputEdges.erase(pDfgEdge);
    OperandResult* p_operand_result;
    p_operand_result = pDfgEdge->getOperandResult();
    if (p_operand_result) {
      if (mMapNumOutputPorts[p_operand_result] <= 1) {
	mMapNumOutputPorts.erase(p_operand_result);
	mSetOutputPorts.erase(p_operand_result);
      }
      else {
	(mMapNumOutputPorts[p_operand_result])--;
      }
    }
  };


  void SubgraphMeritData::addInputEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    mSetInputEdges.insert(pDfgEdge);
    OperandResult* p_operand_result;
    p_operand_result = pDfgEdge->getOperandResult();
    if (p_operand_result) {
      mSetInputPorts.insert(p_operand_result);
      (mMapNumInputPorts[p_operand_result])++;
    }
  };


  void SubgraphMeritData::removeInputEdge(DfgEdge* pDfgEdge) {
    assert(pDfgEdge);
    mSetInputEdges.erase(pDfgEdge);
    OperandResult* p_operand_result;
    p_operand_result = pDfgEdge->getOperandResult();
    if (p_operand_result) {
      if (mMapNumInputPorts[p_operand_result] <= 1) {
	mMapNumInputPorts.erase(p_operand_result);
	mSetInputPorts.erase(p_operand_result);
      }
      else {
	(mMapNumInputPorts[p_operand_result])--;
      }
    }
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::set<DfgEdge*> SubgraphMeritData::getSetOutputEdges() const {
    return mSetOutputEdges;
  };


  std::set<DfgEdge*> SubgraphMeritData::getSetInputEdges() const {
    return mSetInputEdges;
  };


  std::set<OperandResult*, OperandResultComparison> SubgraphMeritData::getSetOutputPorts() const {
    return mSetOutputPorts;
  };


  std::set<OperandResult*, OperandResultComparison> SubgraphMeritData::getSetInputPorts() const {
    return mSetInputPorts;
  };


  size_t SubgraphMeritData::getNumOutputPorts() const {
    return mSetOutputPorts.size();
  };


  size_t SubgraphMeritData::getNumInputPorts() const {
    return mSetInputPorts.size();
  };


  void SubgraphMeritData::setSoftwareLatencyCycles(const size_t softwareLatency) {
    mSoftwareLatencyCycles = softwareLatency;
  };


  size_t SubgraphMeritData::getSoftwareLatencyCycles() const {
    return mSoftwareLatencyCycles;
  };


  void SubgraphMeritData::setHardwareLatencyTime(const size_t hardwareLatency) {
    mHardwareLatencyTime = hardwareLatency;
  };


  size_t SubgraphMeritData::getHardwareLatencyTime() const {
    return mHardwareLatencyTime;
  };


  size_t SubgraphMeritData::getHardwareLatencyCycles() const {
    // We consider each changement of cycle costs CYCLE_COST_CHANGEMENT_CYCLE cycle.
    float num_period = ((float)getHardwareLatencyTime())/((float)mClockPeriod);
    if (mHardwareLatencyTime <= mClockPeriod) {
      return 1;
    }
    else {
      return ( 1 +
	       (size_t)(ceil((1 / (1 - CYCLE_COST_CHANGEMENT_CYCLE)) *
			     ((float)((mHardwareLatencyTime))) /
			     ((float)mClockPeriod))));
    }
  };


  void SubgraphMeritData::setFrequency(const size_t frequency) {
    mFrequency = frequency;
  };


  size_t SubgraphMeritData::getFrequency() const {
    return mFrequency;
  };


  void SubgraphMeritData::setClockPeriod(const size_t clockPeriod) {
    mClockPeriod = clockPeriod;
  };


  size_t SubgraphMeritData::getClockPeriod() const {
    return mClockPeriod;
  };


  size_t SubgraphMeritData::getMerit() const {
    size_t software_latency_cycles = getSoftwareLatencyCycles();
    size_t hardware_latency_cycles = getHardwareLatencyCycles();
    if (software_latency_cycles<=hardware_latency_cycles) {
      return 0;
    }
    return mFrequency * (software_latency_cycles - hardware_latency_cycles);
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string SubgraphMeritData::getHtmlDescription() const {
    std::ostringstream result;
    result << "Output ports : " << getNumOutputPorts()
	   << "<br>";
    result << "Input ports : " << getNumInputPorts()
	   << "<br>";
    result << "Software latency : " << getSoftwareLatencyCycles() << " cycles"
	   << "<br>";
    result << "Hardware latency : " << getHardwareLatencyCycles() << " cycles"
	   << " (" << getHardwareLatencyTime() << " ns.10-2)"
	   << "<br>";
    result << "Merit : " << getMerit();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Operators.                                                             ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool operator<(const SubgraphMeritData& rMeritData1,
		 const SubgraphMeritData& rMeritData2) {
    size_t merit_1 = rMeritData1.getMerit();
    size_t merit_2 = rMeritData2.getMerit();
    if (merit_1 == merit_2) {
      size_t num_ports_1 = rMeritData1.getNumInputPorts()
	+ rMeritData1.getNumOutputPorts();
      size_t num_ports_2 = rMeritData2.getNumInputPorts()
	+ rMeritData2.getNumOutputPorts();
      if (num_ports_1 == num_ports_2) {
	return (rMeritData1.getFrequency() < rMeritData2.getFrequency());
      }
      else {
	return (num_ports_1 > num_ports_2);
      }
    }
    else {
      return (merit_1 < merit_2);
    }
  };


  bool operator==(const SubgraphMeritData& rMeritData1,
		  const SubgraphMeritData& rMeritData2) {
    return !((rMeritData1<rMeritData2) || (rMeritData2<rMeritData1));
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////
    
  SubgraphMeritData::~SubgraphMeritData() {
    mSetOutputEdges.clear();
    mSetInputEdges.clear();
    mSetOutputPorts.clear();
    mMapNumOutputPorts.clear();
    mSetInputPorts.clear();
    mMapNumInputPorts.clear();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////
    
  size_t SubgraphMeritData::getHardwareLatencyTimeCriticalPath (const DfgVertex* pDfgVertex,
								const Subgraph& rSubgraph) {
    assert(pDfgVertex);
    std::set<DfgVertex*> set_vertices = rSubgraph.mSetVertices;
    std::list<DfgEdge*> list_edges = pDfgVertex->getListOutEdges();
    std::list<DfgEdge*>::iterator it_edge;
    size_t hardware_latency = 0;

    for (it_edge = list_edges.begin() ;
	 it_edge != list_edges.end() ;
	 it_edge++) {
      assert(*it_edge);
      DfgVertex* p_target_dfg_vertex = (*it_edge)->getTargetVertex();
      if (set_vertices.find(p_target_dfg_vertex) != set_vertices.end()) {
	hardware_latency
	  = std::max(hardware_latency,
		     getHardwareLatencyTimeCriticalPath(p_target_dfg_vertex, rSubgraph));
      }
    }
    hardware_latency += pDfgVertex->getHardwareLatencyTime();
    return hardware_latency;
  };



};
