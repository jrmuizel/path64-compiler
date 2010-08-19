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
////    Definition of Constant class and functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Constant.cxx
 */


#include "Constant.h"

#include "OperandResult.h"

#include <string>
#include <sstream>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  Constant::Constant(const long long value, const size_t bitSize) {
    mBitSize = bitSize;
    mValue = value;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool Constant::isVariable() const {
    return false;
  };


  long long Constant::getValue() const {
    return mValue;
  };


  std::string Constant::getName() const {
    std::ostringstream result;
    result << mValue;
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////
  

  std::string Constant::getHtmlDescription() const {
    std::ostringstream result;
    result << "<u><b>Constant :</b></u>"
	   << "<br>";
    result << "Value : " << getValue()
	   << "<br>";
    result << "Size : " << getBitSize() << " bits";
    return result.str();
  };


  std::string Constant::getXmlDescription() const {
    std::ostringstream result;
    result << getXmlHeader();
    result << "            <ConstantValue>" << getValue()
	   << "</ConstantValue>" << std::endl;
    result << getXmlFooter();
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Operators.                                                             ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool operator==(const Constant& rConstant1, const Constant& rConstant2) {
    return rConstant1.mValue == rConstant2.mValue;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  Constant::~Constant() {
  };



};
