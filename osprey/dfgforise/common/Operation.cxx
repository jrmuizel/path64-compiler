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
////    Definition of Operation functions.                                       ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Operation.cxx
 */


#include "Operation.h"

#include "DfgForIse_OpCode.h"

#include "OpClass.h"
#include "XmlUtils.h"

#include <map>
#include <string>
#include <sstream>


namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Constructor.                                                           ////
  ///////////////////////////////////////////////////////////////////////////////////

  Operation::Operation(OpCode opCode) {
    mOpCode = opCode;
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Set and get functions.                                                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  void Operation::setDefaultName(const std::string defaultName) {
    mDefaultName = defaultName;
  };


  OpCode Operation::getOpCode() const {
    return mOpCode;
  };


  std::string Operation::getOpCodeName() const {
    if (mOpCode >= OPCODE_NUMBER) {
      return mDefaultName;
    }
    return OpCodeName[mOpCode];
  };


  std::string Operation::getOpCodeDump() const {
    if (mOpCode >= OPCODE_NUMBER) {
      return mDefaultName;
    }
    OpClass opclass = getOpClass();
    if (opclass == OPCLASS_CMP) {
      return CmpVariantDump[mCmpVariant];
    }
    std::string result = OpClassDump[opclass];
    if (result.empty()) {
      result = OpCodeName[mOpCode];
    }
    return result;
  };


  OpClass Operation::getOpClass() const {
    if (mOpCode >= OPCODE_NUMBER) {
      return OPCLASS_FORBIDDEN;
    }
    return OpCodeClass[mOpCode];
  };


  void Operation::setCmpVariant(const CmpVariant cmpVariant) {
    mCmpVariant = cmpVariant;
  };


  CmpVariant Operation::getCmpVariant() const {
    return mCmpVariant;
  };


  void Operation::setSourceFileLine(const size_t sourceFileLine) {
    mSourceFileLine = sourceFileLine;
  };


  size_t Operation::getSourceFileLine() const {
    return mSourceFileLine;
  };


  size_t Operation::getSoftwareLatencyCycles() const {
    if (mOpCode >= OPCODE_NUMBER) {
      return 0;
    }
    return OpCodeSoftwareLatencyCycles[mOpCode];
  };


  size_t Operation::getHardwareLatencyTime(const bool isVar,
					   const size_t bitSize) const {
    if (mOpCode >= OPCODE_NUMBER) {
      return 0;
    }
    std::map< size_t, size_t > map_size_hw
      = gMapHardwareLatencies[getOpClass()][isVar];
    std::map< size_t, size_t >::iterator it_size_hw;
    for(it_size_hw = map_size_hw.begin();
	it_size_hw != map_size_hw.end();
	it_size_hw++) {
      // If the bit size is lower than the map ones,
      if (bitSize <= it_size_hw->first) {
	// Return the corresponding hardware latency.
	return it_size_hw->second;
      }
    }
    return 0;
  };


  bool Operation::isForbiddenInSelection(const size_t bitSize) const {
    return ((bitSize > 32) || OpClassForbiddenInSelection[getOpClass()]);
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Dumping functions.                                                     ////
  ///////////////////////////////////////////////////////////////////////////////////
  
  std::string Operation::getXmlDescription() const {
    std::ostringstream result;
    result << "        <Operation>" << std::endl;
    result << "          <OpCode>" << getOpCodeName() << "</OpCode>" << std::endl;
    result << "          <DefaultName>" << text2xml(mDefaultName)
	   << "</DefaultName>" << std::endl;
    result << "          <CmpVariant>" << getCmpVariant()
	   << "</CmpVariant>" << std::endl;
    result << "          <SourceFileLine>" << mSourceFileLine
	   << "</SourceFileLine>" << std::endl;
    result << "        </Operation>" << std::endl;
    return result.str();
  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Destructor.                                                            ////
  ///////////////////////////////////////////////////////////////////////////////////

  Operation::~Operation(){
  };



  std::map< OpClass,
	    std::map< bool,
		      std::map< size_t,
				size_t > > > gMapHardwareLatencies;



  OpCode GetOpCodeFromName(const std::string opCodeName) {
    // Uses a static map initializated once : links an opcode name to its enumeration
    static std::map<std::string, OpCode> s_map_name_op_code;
    if (s_map_name_op_code.empty()) {
      for (size_t i = 0;
	   i < OPCODE_NUMBER;
	   i++) {
	s_map_name_op_code[OpCodeName[i]] = OpCode(i);
      };
    }
    std::map<std::string, OpCode>::const_iterator it_opcode = s_map_name_op_code.find(opCodeName);
    if (it_opcode == s_map_name_op_code.end()) {
      return OPCODE_NUMBER;
    }
    return it_opcode->second;
  };


  OpClass GetOpClassFromName(const std::string opClassName) {
    // Uses a static map initializated once : links an opclass name to its enumeration
    static std::map<std::string, OpClass> s_map_name_op_class;
    if (s_map_name_op_class.empty()) {
      for (size_t i = 0;
	   i < OPCLASS_NUMBER;
	   i++) {
	s_map_name_op_class[OpClassName[i]] = OpClass(i);
      };
    }
    std::map<std::string, OpClass>::const_iterator it_opclass = s_map_name_op_class.find(opClassName);
    if (it_opclass == s_map_name_op_class.end()) {
      return OPCLASS_NUMBER;
    }
    return it_opclass->second;
  };



};
