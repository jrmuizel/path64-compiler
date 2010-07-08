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
////    Declaration of DfgVertexVariable class and functions.                    ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertexVariable.h
 */


#ifndef DfgVertexVariable_h
#define DfgVertexVariable_h

#include "DfgExternalTypes.h"
#include "DfgVertex.h"

#include <string>
#include <iostream>
   
 

namespace DfgForIse {



  class Variable;



  /** An input/output variable DFG Vertex.
   * Inherited class of DfgVertex representing an input/output variable vertex
   * in a DFG graph.
   */
  class DfgVertexVariable : public DfgVertex {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor with the variable object.
     * @param rVariable A reference to the Variable Object associated
     *                  with the vertex.
     * @param id The size_t representing the id of the vertex.
     */
    DfgVertexVariable(const Variable& rVariable, const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the kind of the vertex.
     * @return The VertexKindType enum : VERTEX_VARIABLE.
     */
    VertexKindType getKind() const;


    /** Gets the variable associated with the vertex.
     * @return A reference to the Variable object associated with the vertex.
     */
    const Variable& getVariable() const;


    /** Gets the id of the variable associated with the vertex.
     * @return A size_t representing the id of the variable associated
     *         with the vertex.
     */
    size_t getVariableId() const;


    /** Gets the name of the variable associated with the vertex.
     * @return A string representing the name of the variable associated
     *         with the vertex.
     */
    std::string getVariableName() const;


    /** Indicates if the vertex is forbidden in a subgraph.
     * @return A bool indicating if the vertex is forbidden in a subgraph.
     */
    bool isForbiddenInSelection() const;



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
    ~DfgVertexVariable();

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
 
    /** Indicates the Variable object associated with the vertex.
     */
    const Variable& mrVariable;



  };



};

#endif // DfgVertexVariable_h
