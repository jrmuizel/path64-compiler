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
////    Definition of OperandResult class and functions.                         ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file OperandResult.cxx
 */


#include "OperandResult.h"

#include "Constant.h"
#include "Variable.h"

#include <string>
#include <sstream>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  size_t OperandResult::getBitSize() const {
    return mBitSize;
  };



  /////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                   ////
  /////////////////////////////////////////////////////////////////////////////////

  std::string OperandResult::getXmlHeader() const {
    std::ostringstream result;
    result << "          <OperandResult>" << std::endl;
    result  << "          <IsVariable>" << isVariable()
	   << "</IsVariable>" << std::endl;
    result << "          <BitSize>" << getBitSize()
	   << "</BitSize>" << std::endl;
    return result.str();
  };


  std::string OperandResult::getXmlFooter() const {
    std::ostringstream result;
    result << "          </OperandResult>" << std::endl;
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Operators.                                                             ////
  ///////////////////////////////////////////////////////////////////////////////////

  bool operator<(const OperandResult& rOpndRes1, const OperandResult& rOpndRes2) {
    if (rOpndRes1.isVariable() && rOpndRes2.isVariable()) {
      const Variable* p_variable_1 = dynamic_cast<const Variable*>(&rOpndRes1);
      const Variable* p_variable_2 = dynamic_cast<const Variable*>(&rOpndRes2);
      assert(p_variable_1);
      assert(p_variable_2);
      return p_variable_1->getId() < p_variable_2->getId();
    }
    else if (!(rOpndRes1.isVariable()) && !(rOpndRes2.isVariable())) {
      const Constant* p_constant_1 = dynamic_cast<const Constant*>(&rOpndRes1);
      const Constant* p_constant_2 = dynamic_cast<const Constant*>(&rOpndRes2);
      assert(p_constant_1);
      assert(p_constant_2);
      return p_constant_1->getValue() < p_constant_2->getValue();
    }
    else {
      return (!(rOpndRes1.isVariable()) && rOpndRes2.isVariable());
    }
  };
    


  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  OperandResult::~OperandResult() {
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Definition of OperandResultComparison class and functions.             ////
  ///////////////////////////////////////////////////////////////////////////////////

  OperandResultComparison::OperandResultComparison() {

  };


  bool OperandResultComparison::operator() (const OperandResult* lhs,
					    const OperandResult* rhs) const {
    assert(lhs);
    assert(rhs);
    return *lhs < *rhs;
  };



};
