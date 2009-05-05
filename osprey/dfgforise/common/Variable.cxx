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
////    Definition of Variable class and functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Variable.cxx
 */


#include "Variable.h"

#include "OperandResult.h"

#include <string>
#include <sstream>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  Variable::Variable(const size_t id, const std::string name, const size_t bitSize) {
    mBitSize = bitSize;
    mId = id;
    mName = name;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool Variable::isVariable() const {
    return true;
  };


  size_t Variable::getId() const {
    return mId;
  };
  

  std::string Variable::getName() const {
    return mName;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////
  
  std::string Variable::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Variable :</b></u>"
	   << "<br>";
    result << "Id : " << getId()
	   << "<br>";
    result << "Name : " << getName()
	   << "<br>";
    result << "Size : " << getBitSize() << " bits";
    return result.str();
  };


  std::string Variable::getXmlDescription() const {
    std::ostringstream result;
    result << getXmlHeader();
    result << "            <VariableId>" << getId()
	   << "</VariableId>" << std::endl;
    result << "            <VariableName>" << getName()
	   << "</VariableName>" << std::endl;
    result << getXmlFooter();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Operators.                                                             ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool operator==(const Variable& rVariable1, const Variable& rVariable2) {
    return rVariable1.mId == rVariable2.mId;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  Variable::~Variable() {
  };



};
