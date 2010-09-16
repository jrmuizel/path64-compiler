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
////    Definition of DfgVertexVariable functions.                               ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertexVariable.cxx
 */


#include "DfgVertexVariable.h"

#include "XmlUtils.h"
#include "Variable.h"

#include "DfgExternalTypes.h"
#include "DfgVertex.h"

#include <string>
#include <iostream>
   
 

namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertexVariable::DfgVertexVariable(const Variable& rVariable, const size_t id)
    : DfgVertex(id)
    , mrVariable(rVariable) {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Get and set functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  VertexKindType DfgVertexVariable::getKind() const {
    return VERTEX_VARIABLE;
  };


  const Variable& DfgVertexVariable::getVariable() const {
    return mrVariable;
  };


  size_t DfgVertexVariable::getVariableId() const {
    return getVariable().getId();
  };


  std::string DfgVertexVariable::getVariableName() const {
    return getVariable().getName();
  };


  bool DfgVertexVariable::isForbiddenInSelection() const {
    return true;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertexVariable::getGraphmlLabel() const {
    std::ostringstream result;
    result << text2xml(getVariableName());
    return result.str();
  };


  std::string DfgVertexVariable::getHtmlDescription() const {
    return getVariable().getHtmlDescription();
  };


  std::string DfgVertexVariable::getXmlDescription() const {
    std::ostringstream result;
    result << "          <Id>" << getId() << "</Id>" << std::endl;
    result << "          <Kind>" << VERTEX_KIND_TYPE_NAME[VERTEX_VARIABLE] << "</Kind>" << std::endl;
    result << getVariable().getXmlDescription();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  DfgVertexVariable::~DfgVertexVariable() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Private functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string DfgVertexVariable::getGraphmlShape() const {
    return "roundrectangle";
  };


  std::string DfgVertexVariable::getGraphmlFillColor() const {
    return "#FFCC00";
  };



};
