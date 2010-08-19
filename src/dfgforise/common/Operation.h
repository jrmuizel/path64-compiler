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
////    Declaration of Operation functions.                                      ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Operation.h
 */


#ifndef Operation_h
#define Operation_h

#include "common_config.h"

#include "DfgForIse_OpCode.h"

#include "OpClass.h"

#include <map>
#include <string>



namespace DfgForIse {



  /** An instance of an operation.
   */
  class Operation {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor.
     * @param opCode The opcode of the operation.
     */
    Operation(OpCode opCode);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Sets the default name of the operation.
     * @param defaultName The default name to use for dumping the operation.
     */
    void setDefaultName(const std::string defaultName);


    /** Gets the opcode of the operation.
     * @return The opcode of the operation.
     */
    OpCode getOpCode() const;


    /** Gets the name to use for the opcode.
     * @return The string representing the name of the opcode.
     */
    std::string getOpCodeName() const;


    /** Gets the dump to use for the opcode.
     * @return The string representing the dump of the opcode.
     */
    std::string getOpCodeDump() const;


    /** Gets the op class of the operation.
     * @return The OpClass of the operation.
     */
    OpClass getOpClass() const;


    /** Sets the CmpVariant, if the operation is a comparison.
     * @param cmpVariant A variant of the comparison operations.
     */
    void setCmpVariant(const CmpVariant cmpVariant);


    /** Gets the CmpVariant, if the operation is a comparison.
     * @result The variant of the comparison operation.
     */
    CmpVariant getCmpVariant() const;


    /** Sets the line in source file of the operation.
     * @param sourceFileLine A size_t indicating the line in the source file.
     */
    void setSourceFileLine(const size_t sourceFileLine);


    /** Gets the line in source file of the operation.
     * @return The size_t indicating the line in the source file.
     */
    size_t getSourceFileLine() const;


    /** Gets the software latency of the operation.
     * The result is a number of cycles.
     * @return The size_t representing the software latency of the opcode.
     */
    size_t getSoftwareLatencyCycles() const;


    /** Gets the hardware latency of the operation.
     * The result is an estimation of the number of hundredth of nanosecond.
     * @param isVar A bool indicating if the operation is an immediate or a variable
     *              operation.
     * @param bitSize A size_t indicating the size of the operation.
     * @return The size_t representing the hardware latency of the opcode.
     */
    size_t getHardwareLatencyTime(const bool isVar, const size_t bitSize) const;


    /** Gets if the opcode is forbidden in a subgraph for a macro-instruction.
     * @param bitSize A size_t indicating the size of the operation.
     * @return The bool indicating if the opcode is forbidden in a cut.
     */
    bool isForbiddenInSelection(const size_t bitSize = DEFAULT_BIT_SIZE) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the xml description of the operand/result.
     * @return The string giving the xml description of the operand/result.
     */
    std::string getXmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~Operation();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
     
    /** Represents the opcode of the operation associated with the vertex.
     */
    OpCode mOpCode;


    /** Represents the type of comparison when the class is a cmp.
     */
    CmpVariant mCmpVariant;


    /** Indicates the line in the source file from which the vertex has been
     *  computed.
     */
    size_t mSourceFileLine;



    /** Indicates the default name to use if the opcode is not a valid opcode.
     */
    std::string mDefaultName;


    
  };



  /** Indicates the hardware latency depending of the OpClass,
   *  immediate or variable operation, and the bit size.
   */
  extern std::map< OpClass,
		   std::map< bool,
			     std::map< size_t,
				       size_t > > > gMapHardwareLatencies;



  /** Gets the opcode corresponding to the name given in parameter.
   * @param opCodeName A string representing the name of the opcode.
   * @return The OpCode which have the specified name.
   */
  OpCode GetOpCodeFromName(const std::string opCodeName);



  /** Gets the opclass corresponding to the name given in parameter.
   * @param opClassName A string representing the name of the opclass.
   * @return The OpClass which have the specified name.
   */
  OpClass GetOpClassFromName(const std::string opClassName);



};

#endif // Operation_h
