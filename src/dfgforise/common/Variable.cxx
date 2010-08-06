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
