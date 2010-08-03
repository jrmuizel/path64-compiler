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
////    Definition of Subgraph functions.                                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Subgraph.cxx
 */


#include "Subgraph.h"

#include "GraphmlTypes.h"
#include "XmlUtils.h"

#include "DumpToGraphml.h"

#include "BasicBlock.h"
#include "DfgEdge.h"
#include "DfgInternalTypes.h"
#include "DfgVertex.h"
#include "SubgraphMeritData.h"

#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include <vector>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  Subgraph::Subgraph(BasicBlock& rBasicBlock,
		     const size_t id)
    : mrBasicBlock(rBasicBlock) {
    mId = id;
    msNextId = std::max(msNextId, mId+1);
    msTableId[mId] = this;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void Subgraph::addVertex(DfgVertex* pDfgVertex) {
    assert(pDfgVertex);
    assert(pDfgVertex->mpGraph);
    assert(pDfgVertex->mpGraph == &mrBasicBlock);
    std::set<DfgVertex*>::iterator it = mSetVertices.find(pDfgVertex);
    if (it != mSetVertices.end()) {
      return;
    }
    bool subgraph_enabled = false;
    mSetVertices.insert(pDfgVertex);
    if ((mrBasicBlock.mSetEnabledSubgraphs.find(this))
	!= mrBasicBlock.mSetEnabledSubgraphs.end()) {
      if (pDfgVertex->mpEnabledSubgraph) {
	assert(pDfgVertex->mpEnabledSubgraph == this);
      }
      subgraph_enabled = true;
      pDfgVertex->mpEnabledSubgraph = this;
    }
    std::list<DfgEdge*> list_edges = pDfgVertex->getListInEdges();
    std::list<DfgEdge*>::iterator it_edge;
    for (it_edge = list_edges.begin();
	 it_edge != list_edges.end();
	 it_edge++) {
      if ((mSetVertices.find((*it_edge)->getSourceVertex()))
	  != mSetVertices.end()) {
	assert(*it_edge);
	mSetEdges.insert(*it_edge);
	if (subgraph_enabled) {
	  (*it_edge)->mpEnabledSubgraph = this;
	}
      }
    }
    list_edges = pDfgVertex->getListOutEdges();
    for (it_edge = list_edges.begin();
	 it_edge != list_edges.end();
	 it_edge++) {
      if ((mSetVertices.find((*it_edge)->getTargetVertex()))
	  != mSetVertices.end()) {
	assert(*it_edge);
	mSetEdges.insert(*it_edge);
	if (subgraph_enabled) {
	  (*it_edge)->mpEnabledSubgraph = this;
	}
      }
    }
  };


  void Subgraph::addVertex(const size_t idVertex) {
    DfgVertex* p_dfg_vertex = DfgVertex::getVertexById(idVertex);
    addVertex(p_dfg_vertex);
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t Subgraph::getId() const {
    return mId;
  };


  size_t Subgraph::getNextId() {
    return msNextId;
  };


  Subgraph* Subgraph::getSubgraphById(const size_t id) {
    std::map<size_t, Subgraph*>::iterator it_subgraph
      = msTableId.find(id);
    if (it_subgraph == msTableId.end()) {
      return 0;
    }
    assert(it_subgraph->second);
    return (it_subgraph->second);
  };


  std::set<DfgVertex*> Subgraph::getSetVertices() const {
    return mSetVertices;
  };


  size_t Subgraph::getNumVertices() const {
    return mSetVertices.size();
  };


  BasicBlock* Subgraph::getBasicBlock() const {
    return &mrBasicBlock;
  };


  void Subgraph::setMeritData(SubgraphMeritData* pSubgraphMeritData) {
    mpSubgraphMeritData = pSubgraphMeritData;
  };


  SubgraphMeritData* Subgraph::getMeritData() const {
    return mpSubgraphMeritData;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string Subgraph::getGraphmlId() const {
    std::ostringstream result;
    result << mrBasicBlock.getGraphmlId() << "::";
    result << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_SUBGRAPH] << getId();
    return result.str();
  };


  std::string Subgraph::getGraphmlLabel() const {
    std::ostringstream result;
    result << "Subgraph " << getId();
    return result.str();
  };


  std::string Subgraph::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Subgraph " << getId() << "</b></u>"
	   << "<br>";
    if (mpSubgraphMeritData) {
      result << mpSubgraphMeritData->getHtmlDescription();
    }
    return result.str();
  };


  std::string Subgraph::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Id>" << mId << "</Id>" << std::endl;
    return result.str();
  };


  void Subgraph::generateDumpGraphml(DumpToGraphml& rOs) const {
    std::set<DfgVertex*>::const_iterator it_vertex;
    std::set<DfgEdge*>::const_iterator it_edge;
    std::string fill_color = getAndIncrGraphmlFillColor();

    rOs << "      <node id=\"" << getGraphmlId() << "\""
	<< " yfiles.foldertype=\"group\">"
	<< std::endl;


    if (rOs.isEnabledPrintFormating()) {
      rOs << "        <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_SUBGRAPH] << "\">"
	  << std::endl;
      rOs << "          <y:ProxyAutoBoundsNode>"
	  << std::endl;
      rOs << "            <y:Realizers active=\"0\">"
	  << std::endl;
      rOs << "              <y:GroupNode>"
	  << std::endl;
      rOs << "                <y:Fill color=\"" << fill_color << "30\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "                <y:BorderStyle color=\"#666699\""
	  << " type=\"dotted\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "                <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"" << fill_color << "\""
	  << " configuration=\"CroppingLabel\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"16\""
	  << " fontStyle=\"plain\""
	  << " hasLineColor=\"false\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"t\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << getGraphmlLabel() << "</y:NodeLabel>"
	  << std::endl;
      rOs << "                <y:Shape type=\"roundrectangle\"/>"
	  << std::endl;
      rOs << "                <y:State closed=\"false\""
	  << " innerGraphDisplayEnabled=\"false\"/>"
	  << std::endl;
      rOs << "                <y:NodeBounds considerNodeLabelSize=\"true\"/>"
	  << std::endl;
      rOs << "                <y:Insets bottom=\"15\""
	  << " left=\"15\""
	  << " right=\"15\""
	  << " top=\"15\"/>"
	  << std::endl;
      rOs << "                <y:BorderInsets bottom=\"0\""
	  << " left=\"1\"" 
	  << " right=\"1\""
	  << " top=\"0\"/>"
	  << std::endl;
      rOs << "              </y:GroupNode>"
	  << std::endl;
      rOs << "              <y:GroupNode>"
	  << std::endl;
      rOs << "                <y:Fill color=\"" << fill_color << "1F\""
	  << " transparent=\"false\"/>"
	  << std::endl;
      rOs << "                <y:BorderStyle color=\"#666699\""
	  << " type=\"dotted\""
	  << " width=\"1.0\"/>"
	  << std::endl;
      rOs << "                <y:NodeLabel alignment=\"right\""
	  << " autoSizePolicy=\"node_width\""
	  << " backgroundColor=\"" << fill_color << "\""
	  << " configuration=\"CroppingLabel\""
	  << " fontFamily=\"Dialog\""
	  << " fontSize=\"16\""
	  << " fontStyle=\"plain\""
	  << " hasLineColor=\"false\""
	  << " modelName=\"internal\""
	  << " modelPosition=\"t\""
	  << " textColor=\"#000000\""
	  << " visible=\"true\">"
	  << getGraphmlLabel() << "</y:NodeLabel>"
	  << std::endl;
      rOs << "                <y:Shape type=\"roundrectangle\"/>"
	  << std::endl;
      rOs << "                <y:State closed=\"true\""
	  << " innerGraphDisplayEnabled=\"true\"/>"
	  << std::endl;
      rOs << "                <y:NodeBounds considerNodeLabelSize=\"true\"/>"
	  << std::endl;
      rOs << "                <y:Insets bottom=\"15\""
	  << " left=\"15\""
	  << " right=\"15\""
	  << " top=\"15\"/>"
	  << std::endl;
      rOs << "                <y:BorderInsets bottom=\"0\""
	  << " left=\"0\""
	  << " right=\"0\""
	  << " top=\"0\"/>"
	  << std::endl;
      rOs << "              </y:GroupNode>"
	  << std::endl;
      rOs << "            </y:Realizers>"
	  << std::endl;
      rOs << "          </y:ProxyAutoBoundsNode>"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
      rOs << "        <data key=\"node_data\">"
	  << text2xml(getHtmlDescription())
	  << "</data>"
	  << std::endl;
    }// end dumpFormating
    else {
      rOs << "        <data key=\""
	  << GRAPHML_NODE_TYPE_NAME[NODE_TYPE_SUBGRAPH] << "\">"
	  << std::endl;
      rOs << "        </data>"
	  << std::endl;
    }


    rOs << "        <DFGforISE>"
	<< std::endl;
    rOs << getXmlDescription();
    rOs << "        </DFGforISE>"
	<< std::endl;
    rOs << "      <graph edgedefault=\"directed\""
	<< " id=\"" << getGraphmlId() << ":\">"
	<< std::endl;

    // Prints the GraphML description of the vertices present in the subgraph.
    for (it_vertex = mSetVertices.begin();
	 it_vertex != mSetVertices.end();
	 it_vertex++) {
      assert(*it_vertex);
      (*it_vertex)->generateDumpGraphml(rOs);
    }

    // Prints the GraphML description of the edges present in the subgraph
    // and which have their two edges in the subgraph.
    for (it_edge = mSetEdges.begin();
	 it_edge != mSetEdges.end();
	 it_edge++) {
      assert(*it_edge);
      (*it_edge)->generateDumpGraphml(rOs);
    }

    rOs << "      </graph>" << std::endl;
    rOs << "      </node>" << std::endl;

  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////
    
  Subgraph::~Subgraph() {
    mSetVertices.clear();
    mSetEdges.clear();
    msTableId.erase(mId);
  };

    

  ///////////////////////////////////////////////////////////////////////////////////
  ////    Protected functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string Subgraph::getAndIncrGraphmlFillColor() const {
    std::ostringstream result;
    int tmp_fill_color = msCurrentFillColor;
    if (!(mrBasicBlock.mSetEnabledSubgraphs.size())) {
      result << "#" << std::hex;
      result.width(6);
      result.fill('0');
      result << tmp_fill_color << std::dec;
      return result.str();
    }
    int value_incr = (128*6)/(mrBasicBlock.mSetEnabledSubgraphs.size());
    // Changes the hue of the color in hsb format.
    while (value_incr > 0) {
      switch (msIncrFillRGB) {
      case 1:
	if (value_incr < (255 - msCurrentFillColor/(256*256))) {
	  msCurrentFillColor += value_incr*256*256;
	  value_incr = 0;
	}
	else {
	  value_incr -= 255 - msCurrentFillColor/(256*256);
	  msCurrentFillColor = 255*256*256 + msCurrentFillColor%(256*256);
	  msIncrFillRGB = -2;
	}
	break;
      case 2:
	if (value_incr < (255 - (msCurrentFillColor/256)%256)) {
	  msCurrentFillColor += value_incr*256;
	  value_incr = 0;
	}
	else {
	  value_incr -= 255 - (msCurrentFillColor/256)%256;
	  msCurrentFillColor = (msCurrentFillColor/(256*256))*256*256
	    + 255*256 + msCurrentFillColor%256;
	  msIncrFillRGB = -3;
	}
	break;
      case 3:
	if (value_incr < (255 - msCurrentFillColor%256)) {
	  msCurrentFillColor += value_incr;
	  value_incr = 0;
	}
	else {
	  value_incr -= 255 - msCurrentFillColor%256;
	  msCurrentFillColor = (msCurrentFillColor/256)*256 + 255;
	  msIncrFillRGB = -1;
	}
	break;
      case -1:
	if (value_incr < (msCurrentFillColor/(256*256) - 127)) {
	  msCurrentFillColor -= value_incr*256*256;
	  value_incr = 0;
	}
	else {
	  value_incr -= msCurrentFillColor/(256*256) - 127;
	  msCurrentFillColor = 127*256*256 + msCurrentFillColor%(256*256);
	  msIncrFillRGB = 2;
	}
	break;
      case -2:
	if (value_incr < ((msCurrentFillColor/256)%256 - 127)) {
	  msCurrentFillColor -= value_incr*256;
	  value_incr = 0;
	}
	else {
	  value_incr -= (msCurrentFillColor/256)%256 - 127;
	  msCurrentFillColor = (msCurrentFillColor/(256*256))*256*256 + 127*256
	    + msCurrentFillColor%256;
	  msIncrFillRGB = 3;
	}
	break;
      case -3:
	if (value_incr < (msCurrentFillColor%256 - 127)) {
	  msCurrentFillColor -= value_incr;
	  value_incr = 0;
	}
	else {
	  value_incr -= msCurrentFillColor%256 - 127;
	  msCurrentFillColor = (msCurrentFillColor/256)*256 + 127;
	  msIncrFillRGB = 1;
	}
	break;
      default:
	msIncrFillRGB = 2;
	break;
      }
    }
    result << "#" << std::hex;
    result.width(6);
    result.fill('0');
    result << tmp_fill_color << std::dec;
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Protected fields.                                                      ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t Subgraph::msNextId = 0;
  

  std::map<size_t, Subgraph*> Subgraph::msTableId;
  

  int Subgraph::msCurrentFillColor = (127*256 + 191)*256 + 255;
  

  short Subgraph::msIncrFillRGB = 2;



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Definition of SubgraphComparison class and functions.                  ////
  ///////////////////////////////////////////////////////////////////////////////////

  SubgraphComparison::SubgraphComparison() {
  };


  bool SubgraphComparison::operator() (const Subgraph* lhs,
				       const Subgraph* rhs) const {
    assert(lhs);
    assert(rhs);
    SubgraphMeritData& lhs_merit_data = *(lhs->getMeritData());
    SubgraphMeritData& rhs_merit_data = *(rhs->getMeritData());
    if (lhs_merit_data == rhs_merit_data) {
      return (lhs->getId() > rhs->getId());
    }
    else {
      return (lhs_merit_data < rhs_merit_data);
    }
  };



};
