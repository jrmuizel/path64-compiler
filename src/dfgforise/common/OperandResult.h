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
////    Declaration of OperandResult class and functions.                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file OperandResult.h
 */


#ifndef OperandResult_h
#define OperandResult_h

#include "common_config.h"

#include <string>



namespace DfgForIse {



  /** An operand and/or a result of an operation.
   * It is a constant or a variable.
   */
  class OperandResult {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets if the operand/result corresponds to a variable.
     * @return The bool indicating if the operand/result is a variable.
     */
    virtual bool isVariable() const = 0;


    /** Gets the name descripting the operand/result.
     * @return The string representing the name of the operand/result.
     */
    virtual std::string getName() const = 0;


    /** Gets the size in bits of the operand/result.
     * @return The size of the operand/result.
     */
    size_t getBitSize() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the html description of the operand/result.
     * @return The string giving the html description of the operand/result.
     */
    virtual std::string getHtmlDescription() const = 0;


    /** Gets the xml description of the operand/result header.
     * @return The string giving the xml description of the operand/result header.
     */
    std::string getXmlHeader() const;


    /** Gets the xml description of the operand/result.
     * @return The string giving the xml description of the operand/result.
     */
    virtual std::string getXmlDescription() const = 0;


    /** Gets the xml description of the operand/result footer.
     * @return The string giving the xml description of the operand/result footer.
     */
    std::string getXmlFooter() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Operators.                                                           ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Operator checking if an operand/result is ordered before another
     *  operand/result.
     * @param rOpndRes1 A reference to the first OperandResult object to check.
     * @param rOpndRes2 A reference to the second OperandResult object to check.
     * @return A bool indicating if the first operand/result is strictly ordered
     *         before the second operand/result.
     */
    friend bool operator<(const OperandResult& rOpndRes1,
			  const OperandResult& rOpndRes2);
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** virtual Destructor.
     */
    virtual ~OperandResult();



  protected:
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Protected fields.                                                    ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Indicates the size of the operand/result.
     */
    size_t mBitSize;



  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Declaration of OperandResultComparison class and functions.            ////
  ///////////////////////////////////////////////////////////////////////////////////

  /** Comparison class used for OperandResult objects.
   */
  class OperandResultComparison {



  public:



    /** Default constructor.
     */
    OperandResultComparison();


    /** Comparison operator on OperandResult objects.
     * Note : The two arguments are supposed to point to existing OperandResult
     *        objects.
     * @param lhs A pointer to the left hand OperandResult.
     * @param rhs A pointer to the right hand OperandResult.
     * @return A bool indicating if the first operand/result is strictly ordered
     *         before the second operand/result .
     */
    bool operator() (const OperandResult* lhs, const OperandResult* rhs) const;



  };



};

#endif // OperandResult_h
