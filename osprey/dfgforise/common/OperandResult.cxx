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
