/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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
