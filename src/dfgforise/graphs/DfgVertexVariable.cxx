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
