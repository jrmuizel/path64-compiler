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
////    Declaration of DfgVertexOperation class and functions.                   ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertexOperation.h
 */


#ifndef DfgVertexOperation_h
#define DfgVertexOperation_h

#include "DfgExternalTypes.h"
#include "OpClass.h"
#include "DfgForIse_OpCode.h"
#include "DfgVertex.h"

#include <string>
#include <iostream>
   
 

namespace DfgForIse {



  class Operation;



  /** An operation DFG Vertex.
   * Inherited class of DfgVertex representing an operation vertex in a DFG graph.
   */
  class DfgVertexOperation : public DfgVertex {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Constructor with the operation.
     * @param rOperation An Operation object.
     * @param id The size_t representing the id of the vertex.
     */
    DfgVertexOperation(const Operation& rOperation, const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the kind of the vertex.
     * @return The VertexKindType enum : VERTEX_OPERATION.
     */
    VertexKindType getKind() const;


    /** Gets the operation linked with the vertex.
     * @return A reference to the operation associated with the vertex.
     */
    const Operation& getOperation() const;


    /** Gets the opcode of the operation linked with the vertex.
     * @return The OpCode associated with the vertex.
     */
    OpCode getOpCode() const;


    /** Gets the name of the opcode of the operation linked with the vertex.
     * @return The string representing the name of the OpCode.
     */
    std::string getOpCodeName() const;


    /** Gets the opclass of the operation linked with the vertex.
     * @return The OpClass associated with the vertex.
     */
    OpClass getOpClass() const;


    /** Gets the size of the operation in bits.
     * @return The size_t indicating the size of the operation.
     */
    size_t getBitSize() const;


    /** Gets if the operation is a variable or an immediate operation.
     * @return The bool indicating if the operation is a variable or an immediate
     *         operation.
     */
    bool isVar() const;


    /** Gets the software cost of the vertex.
     * @return The int representing the software cost of the operation
     *         associated with the vertex.
     */
    size_t getSoftwareLatencyCycles() const;


    /** Gets the hardware cost of the vertex.
     * @return The int representing the hardware cost of the operation
     *         associated with the vertex.
     */
    size_t getHardwareLatencyTime() const;


    /** Indicates if the vertex is forbidden in a subgraph.
     * @return A bool indicating if the vertex is forbidden in a subgraph.
     */
    bool isForbiddenInSelection() const;


    /** Gets the line of the code in source file.
     * @return The size_t representing the line of the code in source file.
     */
    size_t getSourceFileLine() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Other functions.                                                     ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Duplicates MAKE operations in one vertex for each out dependency.
     */
    virtual void duplicateMakeOperations();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the label used to represent the vertex when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    std::string getGraphmlLabel() const;


    /** Gets the html description of the vertex.
     * @return The string giving the html description of the vertex.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the vertex.
     * @return The string giving the xml description of the vertex.
     */
    std::string getXmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~DfgVertexOperation();



  private:




    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the shape used to represent the vertex when dumping in GraphML.
     * @return The string indicating which shape to use when dumping in GraphML.
     */
    std::string getGraphmlShape() const;


    /** Gets the fill color used to represent the vertex when dumping in GraphML.
     * @return The string indicating which fill color to use when dumping in GraphML.
     */
    std::string getGraphmlFillColor() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Represents the operation associated with the vertex.
     */
    const Operation& mrOperation;
  


  };



};

#endif // DfgVertexOperation_h
