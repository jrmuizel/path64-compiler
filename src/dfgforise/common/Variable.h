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
////    Declaration of Variable class and functions.                             ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Variable.h
 */


#ifndef Variable_h
#define Variable_h

#include "common_config.h"

#include "OperandResult.h"

#include <string>



namespace DfgForIse {



  /** A variable.
   * Used to view reuse of certain variables.
   */
  class Variable : public OperandResult {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor with the id and the name of the variable.
     * @param id A size_t argument for the id of the variable. 
     * @param name A string argument representing the name of the variable.
     * @param bitSize A size_t indicating the size of the variable.
     */
    Variable(const size_t id,
	     const std::string name,
	     const size_t bitSize = DEFAULT_BIT_SIZE);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets if the operand/result corresponds to a variable.
     * @return The bool indicating if the operand/result is a variable.
     */
    bool isVariable() const;


    /** Gets the id of the variable.
     * @return The size_t representing the id of the variable.
     */
    size_t getId() const;


    /** Gets the name of the variable.
     * @return The string representing the name of the variable.
     */
    std::string getName() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the html description of the variable.
     * @return The string giving the html description of the variable.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the operand/result.
     * @return The string giving the xml description of the operand/result.
     */
    std::string getXmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Operators.                                                           ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Operator checking equality between variables.
     * @param rVariable1 A reference to the first Variable object to check.
     * @param rVariable2 A reference to the second Variable object to check.
     * @return A bool indicating if the two variables have the same id.
     */
    friend bool operator==(const Variable& rVariable1, const Variable& rVariable2);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~Variable();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Indicates the id of the variable.
     */
    size_t mId; 


    /** Indicates the name of the variable.
     */
    std::string mName;



  };



};

#endif // Variable_h
