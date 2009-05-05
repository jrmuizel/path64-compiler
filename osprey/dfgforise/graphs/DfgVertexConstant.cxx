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
////    Definition of DfgVertexConstant functions.                               ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertexConstant.cxx
 */


#include "DfgVertexConstant.h"

#include "Constant.h"

#include "DfgExternalTypes.h"
#include "DfgVertex.h"

#include <string>
#include <sstream>
#include <iostream>
   

 
namespace DfgForIse {




  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertexConstant::DfgVertexConstant(const Constant& rConstant,
				       const size_t id)
    : DfgVertex(id)
    , mrConstant(rConstant) {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  VertexKindType DfgVertexConstant::getKind() const {
    return VERTEX_CONSTANT;
  };


  const Constant& DfgVertexConstant::getConstant() const {
    return mrConstant;
  };


  long long DfgVertexConstant::getConstantValue() const {
    return getConstant().getValue();
  };


  bool DfgVertexConstant::isForbiddenInSelection() const {
    return mpEnabledSubgraph;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////
  
  std::string DfgVertexConstant::getGraphmlLabel() const {
    std::ostringstream result;
    result << getConstantValue() << " (0x" << std::hex << getConstantValue() << std::dec << ")";
    return result.str();
  };

  
  std::string DfgVertexConstant::getHtmlDescription() const {
    return getConstant().getHtmlDescription();
  };


  std::string DfgVertexConstant::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Id>" << getId() << "</Id>" << std::endl;
    result << "          <Kind>" << VERTEX_KIND_TYPE_NAME[VERTEX_CONSTANT] << "</Kind>" << std::endl;
    result << getConstant().getXmlDescription();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertexConstant::~DfgVertexConstant() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.    ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertexConstant::getGraphmlShape() const {
    return "rectangle";
  };


  std::string DfgVertexConstant::getGraphmlFillColor() const {
    return "#CC99FF";
  };



};
