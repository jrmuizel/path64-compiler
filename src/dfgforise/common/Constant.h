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
////    Declaration of Constant class and functions.                             ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Constant.h
 */


#ifndef Constant_h
#define Constant_h

#include "common_config.h"
#include "OperandResult.h"

#include <string>



namespace DfgForIse {



  /** A constant.
   */
  class Constant : public OperandResult {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor with the value of the constant.
     * @param value A long long representing the value of the constant.
     * @param bitSize A size_t indicating the size of the constant.
     */
    Constant(const long long value, const size_t bitSize = DEFAULT_BIT_SIZE);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets if the operand/result corresponds to a variable.
     * @return The bool indicating if the operand/result is a variable.
     */
    bool isVariable() const;


    /** Gets the value of the constant.
     * @return The long long representing the value of the constant.
     */
    long long getValue() const;


    /** Gets the name of the constant.
     * @return The string representing the name of the constant.
     */
    std::string getName() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the html description of the constant.
     * @return The string giving the html description of the constant.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the constant.
     * @return The string giving the xml description of the constant.
     */
    std::string getXmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Operators.                                                           ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Operator checking equality between constants.
     * @param rConstant1 A reference to the first Constant object to check.
     * @param rConstant2 A reference to the second Constant object to check.
     * @return A bool indicating if the two constants have the same value.
     */
    friend bool operator==(const Constant& rConstant1, const Constant& rConstant2);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~Constant();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Indicates the value of the constant.
     */
    long long mValue;



  };



};

#endif // Constant_h
