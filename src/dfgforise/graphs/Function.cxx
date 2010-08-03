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
////    Definition of Function class and functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Function.cxx
 */


#include "Function.h"

#include "GraphmlTypes.h"
#include "Variable.h"
#include "XmlUtils.h"

#include "DumpToGraphml.h"

#include "BasicBlock.h"

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <set>


namespace DfgForIse {


  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  Function::Function(const size_t id) {
    mId = id;
    msNextId = std::max(msNextId, mId+1);
    msTableId[mId] = this;
    {
      std::ostringstream name;
      name << "Function " << mId;
      mName = name.str();
    }
    mMaximumFrequency = 0;
    mSourceFileName = "";
    mSourceFileLine = 0;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void Function::addBasicBlock(BasicBlock* pBasicBlock) {
    assert(pBasicBlock);
    mMaximumFrequency = std::max(mMaximumFrequency, pBasicBlock->getFrequency());
    pBasicBlock->mpFunction = this;
    mMapBasicBlocks[pBasicBlock->getId()] = pBasicBlock;
  };


  void Function::removeBasicBlock(BasicBlock* pBasicBlock) {
    assert(pBasicBlock);
    mMapBasicBlocks.erase(pBasicBlock->getId());
  };


  void Function::addVariable(Variable* pVariable) {
    assert(pVariable);
    mMapVariables[pVariable->getId()] = pVariable;
  };


  void Function::addEdge(size_t sourceId, size_t targetId) {
    mSetEdges.insert(std::pair<size_t,size_t>(sourceId,targetId));
  };


  void Function::removeEdge(size_t sourceId, size_t targetId) {
    mSetEdges.erase(std::pair<size_t,size_t>(sourceId,targetId));
  };


  void Function::duplicateMakeOperations() {
    std::map<size_t, BasicBlock*>::const_iterator it_basic_block;
    for (it_basic_block = mMapBasicBlocks.begin();
	 it_basic_block != mMapBasicBlocks.end();
	 it_basic_block++) {
      assert(it_basic_block->second);
      it_basic_block->second->duplicateMakeOperations();
    }
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t Function::getId() const {
    return mId;
  };


  size_t Function::getNextId() {
    return msNextId;
  };


  Function* Function::getFunctionById(const size_t id) {
    std::map<size_t, Function*>::iterator it_function
      = msTableId.find(id);
    if (it_function == msTableId.end()) {
      return 0;
    }
    assert(it_function->second);
    return (it_function->second);
  };


  BasicBlock* Function::getBasicBlockById(const size_t id) const{
    std::map<size_t, BasicBlock*>::const_iterator it_basic_block
      = mMapBasicBlocks.find(id);
    if (it_basic_block == mMapBasicBlocks.end()) {
      return 0;
    }
    assert(it_basic_block->second);
    return (it_basic_block->second);
  };


  Variable* Function::getOrCreateVariable(const size_t id,
					  const std::string name,
					  const size_t bitSize) {
    Variable* p_dfg_variable = mMapVariables[id];
    if (!p_dfg_variable) {
      p_dfg_variable = new Variable(id, name, bitSize);
      mMapVariables[id] = p_dfg_variable;
    }
    assert(p_dfg_variable);
    return p_dfg_variable;
  };

 
  void Function::setName(const std::string name) {
    mName = name;
  };


  std::string Function::getName() const {
    return mName;
  };


  void Function::setMaximumFrequency(const size_t maximumFrequency) {
    mMaximumFrequency = maximumFrequency;
  };


  size_t Function::getMaximumFrequency() const {
    return mMaximumFrequency;
  };


  void Function::setSourceFileName(const std::string sourceFileName) {
    mSourceFileName = sourceFileName;
  };


  std::string Function::getSourceFileName() const {
    return mSourceFileName;
  };


  void Function::setSourceFileLine(const size_t sourceFileLine) {
    mSourceFileLine = sourceFileLine;
  };


  size_t Function::getSourceFileLine() const {
    return mSourceFileLine;
  };
    

  std::list<BasicBlock*> Function::getListBasicBlocks() {
    std::list<BasicBlock*> result;
    std::map<size_t, BasicBlock*>::iterator it_basic_block;
    for (it_basic_block =  mMapBasicBlocks.begin();
	 it_basic_block !=  mMapBasicBlocks.end();
	 it_basic_block++) {
      assert(it_basic_block->second);
      result.push_back(it_basic_block->second);
    }
    return result;
  };


  bool Function::empty() {
    return mMapBasicBlocks.empty();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string Function::getGraphmlId() const {
    std::ostringstream result;
    result << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_FUNCTION] << mId;
    return result.str();
  };

  
  std::string Function::getGraphmlLabel() const {
    std::ostringstream result;
    if (!mSourceFileName.empty()) {
      result << mSourceFileName << " : ";
    }
    result << mName;
    return result.str();
  };


  std::string Function::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>" << mName << "</b></u>"
	   << "<br>";
    if (!mSourceFileName.empty()) {
      result << "Source File : " << mSourceFileName;
    }
    if (mSourceFileLine) {
      result << " (Line : " << mSourceFileLine << ")";
    }
    return result.str();
  };


  std::string Function::getXmlDescription() const {
    std::ostringstream result;
    result << "      <Id>" << mId << "</Id>" << std::endl;
    result << "      <Name>" << mName << "</Name>" << std::endl;
    result << "      <MaximumFrequency>" << mMaximumFrequency
	   << "</MaximumFrequency>" << std::endl;
    result << "      <SourceFileName>" << mSourceFileName
	   << "</SourceFileName>" << std::endl;
    result << "      <SourceFileLine>" << mSourceFileLine
	   << "</SourceFileLine>" << std::endl;
    return result.str();
  };

    
  void Function::generateDumpGraphml(DumpToGraphml& rOs) const {

    // Prints the GraphML description of the set of graph (generally a function).
    rOs << "  <node id=\"" << getGraphmlId() << "\""
	<< " yfiles.foldertype=\"group\">"
	<< std::endl;


    if (rOs.isEnabledPrintFormating()) {
      rOs << "    <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_FUNCTION] << "\">"
	  << std::endl;
      rOs << "      <y:ProxyAutoBoundsNode>"
	  << std::endl;
      rOs << "        <y:Realizers active=\"0\">"
	  << std::endl;
      rOs << "          <y:GroupNode>"
	  << std::endl;
      rOs << "            <y:Fill color=\"#FEFEFE07\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "            <y:BorderStyle color=\"#000000\""
	  << " type=\"line\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "            <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"#FEFEFE\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"14\""
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
      rOs << "            <y:Fill color=\"#FEFEFE07\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "            <y:BorderStyle color=\"#000000\""
	  << " type=\"line\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "            <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"#FEFEFE\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"14\""
	  << " fontStyle=\"plain\""
	  << " lineColor=\"#999999\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"t\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << mName
	  << "</y:NodeLabel>"
	  << std::endl;
      rOs << "            <y:Shape type=\"rectangle\"/>"
	  << std::endl;
      rOs << "            <y:State closed=\"true\""
	  << " innerGraphDisplayEnabled=\"false\"/>"
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
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_FUNCTION] << "\">"
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
    rOs << "  <graph edgedefault=\"directed\">"
	<< std::endl;

    // Prints the graphs.
    std::map<size_t, BasicBlock*>::const_iterator it_basic_block;
    for (it_basic_block = mMapBasicBlocks.begin();
	 it_basic_block != mMapBasicBlocks.end();
	 it_basic_block++) {
      assert(it_basic_block->second);
      it_basic_block->second->generateDumpGraphml(rOs);
    }

    // Prints the edges between the graphs (CFG dependencies).
    std::set< std::pair<size_t,size_t> >::const_iterator it_edge;
    for (it_edge = mSetEdges.begin();
	 it_edge != mSetEdges.end();
	 it_edge++) {
      BasicBlock* p_basic_block_source = getBasicBlockById(it_edge->first);
      BasicBlock* p_basic_block_target = getBasicBlockById(it_edge->second);
      if (p_basic_block_source && p_basic_block_target) {
	rOs << "      <edge source=\"" << p_basic_block_source->getGraphmlId() << "\""
	    << " target=\"" << p_basic_block_target->getGraphmlId() << "\">"
	    << std::endl;


	if (rOs.isEnabledPrintFormating()) {
	  rOs << "        <data key=\""
	      << GRAPHML_EDGE_TYPE_NAME[EDGE_TYPE_CFG] << "\">"
	      << std::endl;
	  rOs << "          <y:BezierEdge>"
	      << std::endl;
	  rOs << "          <y:LineStyle color=\"#3333337F\""
	      << " type=\"line\""
	      << " width=\"2.0\"/>"
	      << std::endl;
	  rOs << "            <y:Arrows source=\"none\""
	      << " target=\"white_delta\"/>" << std::endl;
	  rOs << "            <y:BendStyle smoothed=\"false\"/>"
	      << std::endl;
	  rOs << "          </y:BezierEdge>"
	      << std::endl;
	  rOs << "        </data>"
	      << std::endl;
	  rOs << "        <data key=\"edge_data\"/>"
	      << std::endl;
	}// end dumpFormating
	else {
	  rOs << "        <data key=\""
	      << GRAPHML_EDGE_TYPE_NAME[EDGE_TYPE_CFG] << "\">"
	      << std::endl;
	  rOs << "        </data>"
	      << std::endl;
	}


	rOs << "        <DFGforISE>"
	    << std::endl;
	rOs << "          <IdSourceGraph>"
	    << it_edge->first
	    << "</IdSourceGraph>"
	    << std::endl;
	rOs << "          <IdTargetGraph>"
	    << it_edge->second
	    << "</IdTargetGraph>"
	    << std::endl;
	rOs << "        </DFGforISE>"
	    << std::endl;
	rOs << "      </edge>"
	    << std::endl;
      }
    }

    rOs << "  </graph>" << std::endl;
    rOs << "  </node>" << std::endl;

  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  /** Destructor.
   */
  Function::~Function(){
    mMapBasicBlocks.clear();
    mSetEdges.clear();
    mMapVariables.clear();
    msTableId.erase(mId);
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private fields.                                                        ////
  ///////////////////////////////////////////////////////////////////////////////////
 
  std::map<size_t, Function*> Function::msTableId;


  size_t Function::msNextId = 0;
 


};
