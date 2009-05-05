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
////    Definition of DumpToGraphml functions.                                   ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DumpToGraphml.cxx
 */

#include "DumpToGraphml.h"

#include "GraphmlTypes.h"

#include "DfgExternalTypes.h"

#include <iostream>
#include <iomanip>
#include <fstream>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////
    
  DumpToGraphml::DumpToGraphml(const size_t maxFrequency,
			       const bool printFormating,
			       const bool printCfg)
    : std::ofstream() {
    mMaxFrequency = maxFrequency;
    mPrintFormating = printFormating;
    mPrintCfg = printCfg;
    mPrintNonRealEdges = true;

    mPrintEdgeKind[EDGE_DEP_REGIN] = true;
    mPrintEdgeKind[EDGE_DEP_REGOUT] = true;
    mPrintEdgeKind[EDGE_DEP_REGANTI] = true;
    mPrintEdgeKind[EDGE_DEP_MEMIN] = true;
    mPrintEdgeKind[EDGE_DEP_MEMOUT] = true;
    mPrintEdgeKind[EDGE_DEP_MEMANTI] = true;
    mPrintEdgeKind[EDGE_DEP_MEMVOL] = true;
    mPrintEdgeKind[EDGE_DEP_MEMREAD] = true;
    mPrintEdgeKind[EDGE_DEP_SPILLIN] = true;
    mPrintEdgeKind[EDGE_DEP_PREFIN] = true;
    mPrintEdgeKind[EDGE_DEP_PREFOUT] = true;
    mPrintEdgeKind[EDGE_DEP_PREBR] = false;
    mPrintEdgeKind[EDGE_DEP_POSTBR] = false;
    mPrintEdgeKind[EDGE_DEP_SCC] = true;
    mPrintEdgeKind[EDGE_DEP_MISC] = true;

    mPrintEdgeKind[EDGE_IN_UNDEFINED] = true;
    mPrintEdgeKind[EDGE_IN_CONSTANT] = true;
    mPrintEdgeKind[EDGE_IN_VARIABLE] = true;
    mPrintEdgeKind[EDGE_OUT_VARIABLE] = true;
  };
    

  DumpToGraphml::DumpToGraphml(const char * filename,
			       ios_base::openmode mode,
			       const size_t maxFrequency,
			       const bool printFormating,
			       const bool printCfg)
    : std::ofstream(filename, mode) {
    mMaxFrequency = maxFrequency;
    mPrintFormating = printFormating;
    mPrintCfg = printCfg;
    mPrintNonRealEdges = true;

    mPrintEdgeKind[EDGE_DEP_REGIN] = true;
    mPrintEdgeKind[EDGE_DEP_REGOUT] = true;
    mPrintEdgeKind[EDGE_DEP_REGANTI] = true;
    mPrintEdgeKind[EDGE_DEP_MEMIN] = true;
    mPrintEdgeKind[EDGE_DEP_MEMOUT] = true;
    mPrintEdgeKind[EDGE_DEP_MEMANTI] = true;
    mPrintEdgeKind[EDGE_DEP_MEMVOL] = true;
    mPrintEdgeKind[EDGE_DEP_MEMREAD] = true;
    mPrintEdgeKind[EDGE_DEP_SPILLIN] = true;
    mPrintEdgeKind[EDGE_DEP_PREFIN] = true;
    mPrintEdgeKind[EDGE_DEP_PREFOUT] = true;
    mPrintEdgeKind[EDGE_DEP_PREBR] = false;
    mPrintEdgeKind[EDGE_DEP_POSTBR] = false;
    mPrintEdgeKind[EDGE_DEP_SCC] = true;
    mPrintEdgeKind[EDGE_DEP_MISC] = true;

    mPrintEdgeKind[EDGE_IN_UNDEFINED] = true;
    mPrintEdgeKind[EDGE_IN_CONSTANT] = true;
    mPrintEdgeKind[EDGE_IN_VARIABLE] = true;
    mPrintEdgeKind[EDGE_OUT_VARIABLE] = true;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Modifying functions.                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////

  void DumpToGraphml::setMaximumFrequency(const size_t maxFrequency) {
    mMaxFrequency = maxFrequency;
  };


  void DumpToGraphml::setPrintFormating(const bool printFormating) {
    mPrintFormating = printFormating;
  };


  void DumpToGraphml::setPrintCfg(const bool printCfg) {
    mPrintCfg = printCfg;
  };


  void DumpToGraphml::setPrintNonRealEdges(const bool printNonRealEdges) {
    mPrintNonRealEdges = printNonRealEdges;
  };


  void DumpToGraphml::setPrintEdgeKind(const bool enabled,
				       const EdgeKindType kind) {
    mPrintEdgeKind[kind] = enabled;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get functions.                                                         ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t DumpToGraphml::getMaximumFrequency() {
    return mMaxFrequency;
  };


  bool DumpToGraphml::isEnabledPrintFormating() {
    return mPrintFormating;
  };


  bool DumpToGraphml::isEnabledPrintCfg() {
    return mPrintCfg;
  };


  bool DumpToGraphml::isEnabledPrintNonRealEdges() {
    return mPrintNonRealEdges;
  };


  bool DumpToGraphml::isEnabledPrintEdgeKind(const EdgeKindType kind) {
    return mPrintEdgeKind[kind];
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  void DumpToGraphml::dumpGraphmlHeader() {
    assert(is_open());
    *this << "<?xml version=\"1.0\""
	<< " encoding=\"UTF-8\""
	<< " standalone=\"no\"?>"
	<< std::endl;
    *this << "<graphml xmlns=\""
	<< "http://graphml.graphdrawing.org/xmlns/graphml\""
	<< " xmlns:xsi=\""
	<< "http://www.w3.org/2001/XMLSchema-instance\""
	<< " xmlns:y=\"http://www.yworks.com/xml/graphml\""
	<< " xsi:schemaLocation=\""
	<< "http://graphml.graphdrawing.org/xmlns/graphml "
	<< "http://www.yworks.com/xml/schema/graphml/1.0/ygraphml.xsd\">"
	<< std::endl;
    for (size_t i = 0 ; i < NODE_TYPE_INVALID ; i++) {
      *this << "<key for=\"node\""
	  << " id=\"" << GRAPHML_NODE_TYPE_NAME[i] << "\""
	  << " yfiles.type=\"nodegraphics\"/>"
	  << std::endl;
    }
    for (size_t i = 0 ; i < EDGE_TYPE_INVALID ; i++) {
      *this << "<key for=\"edge\""
	  << " id=\"" << GRAPHML_EDGE_TYPE_NAME[i] << "\""
	  << " yfiles.type=\"edgegraphics\"/>"
	  << std::endl;
    }
    *this << "<key attr.name=\"description\""
	<< " attr.type=\"string\""
	<< " for=\"node\""
	<< " id=\"node_data\"/>"
	<< std::endl;
    *this << "<key attr.name=\"description\""
	<< " attr.type=\"string\""
	<< " for=\"edge\""
	<< " id=\"edge_data\"/>"
	<< std::endl;
    *this << "<key for=\"graph\" id=\"postprocessors\" yfiles.type=\"postprocessors\"/>"
	<< std::endl;
    *this << "<graph edgedefault=\"directed\">"
	<< std::endl;
    if (mMaxFrequency >= 0) {
      *this << "  <MaxFrequency>" << mMaxFrequency << "</MaxFrequency>" << std::endl;
    }
  };


  void DumpToGraphml::dumpGraphmlFooter() {
    assert(is_open());
    *this << "<data key=\"postprocessors\">" << std::endl;
    *this << "  <y:Postprocessors>" << std::endl;

    // Processor : yext.graphml.processor.NodeSizeAdapter
    // Obfuscated in yEd 3.1.1 java byte code by the class named : C.C.E.C
    // Automatically fit node to their label when parsing with yEd Graph Editor.
    *this << "  <y:Processor class=\"C.C.E.C\">" << std::endl;
    *this << "    <y:Option name=\"ADAPT_TO_MAXIMUM_NODE\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"HORIZON_SPACE\" value=\"5\"/>" << std::endl;
    *this << "    <y:Option name=\"IGNORE_HEIGHTS\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"IGNORE_WIDTHS\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"MIN_HEIGHT\" value=\"30\"/>" << std::endl;
    *this << "    <y:Option name=\"MIN_WIDTH\" value=\"30\"/>" << std::endl;
    *this << "    <y:Option name=\"ONLY_SELECTION\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"VERTICAL_SPACE\" value=\"5\"/>" << std::endl;
    *this << "  </y:Processor>" << std::endl;

    // Processor : y.module.IncrementalHierarchicLayoutModule
    // Obfuscated in yEd 3.1.1 java byte code by the class named : D.B.F
    // Automatically layout the graph in hierarchical order when parsing with yEd Graph Editor.
    *this << "  <y:Processor class=\"D.B.F\">" << std::endl;
    *this << "    <y:Option name=\"GENERAL.EDGE_TO_EDGE_DISTANCE\" value=\"5\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.LAYOUT_COMPONENTS_SEPARATELY\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.MINIMUM_DISTANCES\" value=\"20\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.MINIMUM_LAYER_DISTANCE\" value=\"40\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.NODE_TO_EDGE_DISTANCE\" value=\"10\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.NODE_TO_NODE_DISTANCE\" value=\"20\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.ORIENTATION\" value=\"TOP_TO_BOTTOM\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.SELECTED_ELEMENTS_INCREMENTALLY\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.SYMMETRIC_PLACEMENT\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"GENERAL.USE_DRAWING_AS_SKETCH\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.BACKLOOP_ROUTING\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.EDGE_ROUTING\" value=\"EDGE_ROUTING_POLYLINE\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.MINIMUM_EDGE_DISTANCE\" value=\"5\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.MINIMUM_EDGE_LENGTH\" value=\"0\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.MINIMUM_FIRST_SEGMENT_LENGTH\" value=\"0\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.MINIMUM_LAST_SEGMENT_LENGTH\" value=\"0\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.MINIMUM_SLOPE\" value=\"0.1\"/>" << std::endl;
    *this << "    <y:Option name=\"EDGE_SETTINGS.PC_OPTIMIZATION_ENABLED\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"LABELING.CONSIDER_NODE_LABELS\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"LABELING.EDGE_LABELING\" value=\"EDGE_LABELING_GENERIC\"/>" << std::endl;
    *this << "    <y:Option name=\"LABELING.EDGE_LABEL_MODEL\" value=\"EDGE_LABEL_MODEL_AS_IS\"/>" << std::endl;
    *this << "    <y:Option name=\"LABELING.NODE_LABEL_CONSIDERATION\" value=\"NODE_LABEL_CONSIDERATION_ROUTING\"/>" << std::endl;
    *this << "    <y:Option name=\"RANKS.RANKING_POLICY\" value=\"HIERARCHICAL_OPTIMAL\"/>" << std::endl;
    *this << "    <y:Option name=\"RANKS.LAYER_ALIGNMENT\" value=\"BOTTOM\"/>" << std::endl;
    *this << "    <y:Option name=\"GROUPING.GROUP_LAYERING_STRATEGY\" value=\"RECURSIVE_LAYERING\"/>" << std::endl;
    *this << "    <y:Option name=\"GROUPING.GROUP_ALIGNMENT\" value=\"GROUP_ALIGN_BOTTOM\"/>" << std::endl;
    *this << "    <y:Option name=\"GROUPING.GROUP_ENABLE_COMPACTION\" value=\"true\"/>" << std::endl;
    *this << "    <y:Option name=\"GROUPING.GROUP_HORIZONTAL_COMPACTION\" value=\"GROUP_HORIZONTAL_COMPACTION_MAX\"/>" << std::endl;
    *this << "    <y:Option name=\"SWIMLANES.TREAT_ROOT_GROUPS_AS_SWIMLANES\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"SWIMLANES.USE_ORDER_FROM_SKETCH\" value=\"false\"/>" << std::endl;
    *this << "    <y:Option name=\"SWIMLANES.SWIMLANE_SPACING\" value=\"0\"/>" << std::endl;
    *this << "  </y:Processor>" << std::endl;


    *this << "  </y:Postprocessors>" << std::endl;
    *this << "</data>" << std::endl;
    *this << "</graph>" << std::endl;
    *this << "</graphml>" << std::endl;
  };



};
