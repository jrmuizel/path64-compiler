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
////    Declaration of DfgVertex class and functions.                            ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgVertex.h
 */


#ifndef DfgVertex_h
#define DfgVertex_h

#include "DumpToGraphml.h"

#include "DfgExternalTypes.h"
#include "DfgInternalTypes.h"

#include <string>
#include <iostream>
#include <map>
#include <list>
   
 

namespace DfgForIse {


  class BasicBlock;
  class DfgGraph;
  class DfgEdge;
  class Subgraph;



  /** A DFG Vertex.
   * Class representing a vertex in a DFG graph.
   * Undefined vertex.
   */
  class DfgVertex {
  


  public:



    friend class BasicBlock;
    friend class DfgGraph;
    friend class DfgEdge;
    friend class Subgraph;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Default constructor.
     * @param id The size_t representing the id of the vertex.
     */
    DfgVertex(const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the unique id of the vertex.
     * @return The size_t unique id of the vertex.
     */
    size_t getId() const;


    /** Gets the id of the next vertex if automatically indexed.
     * @return The size_t id of the next vertex if automatically indexed.
     */
    static size_t getNextId();


    /** Gets a pointer to the DfgVertex object identified by its unique id.
     * @param id A size_t representing a unique id on DfgVertex.
     * @return A pointer to the DfgVertex object which has the given id.
     *         Returns NULL if the id has no corresponding vertex.
     */
    static DfgVertex* getVertexById(const size_t id);


    /** Gets the kind of the vertex.
     * @return The VertexKindType enum which indicates the kind of vertex.
     */
    virtual VertexKindType getKind() const;


    /** Sets the default name of the undefined vertex.
     * @param defaultName The default name to use for dumping the vertex.
     */
    void setDefaultName(const std::string defaultName);


    /** Gets the software cost of the vertex.
     * @return The int representing the software cost of the operation
     *         associated with the vertex.
     *         Returns 0 if the vertex does not correspond to an operation.
     */
    virtual size_t getSoftwareLatencyCycles() const;


    /** Gets the hardware cost of the vertex.
     * @return The int representing the hardware cost of the operation
     *         associated with the vertex.
     *         Returns 0 if the vertex does not correspond to an operation.
     */
    virtual size_t getHardwareLatencyTime() const;


    /** Indicates if the vertex is forbidden in a subgraph.
     * @return A bool indicating if the vertex is forbidden in a subgraph.
     */
    virtual bool isForbiddenInSelection() const;
    

    /** Gets the id of the vertex in topological order in its graph.
     * Needs a previous call to its corresponding DfgGraph's function 
     * "getVectorVerticesTopologicalOrder()" to be significant.
     * @return The size_t id of the vertex in topological order.
     */
    size_t getIdTopologicalOrder() const;
    

    /** Gets the list of the in edges.
     * @return A list of pointers to the in DfgEdge objects.
     */
    std::list<DfgEdge*> getListInEdges() const;
    

    /** Gets the list of the out edges.
     * @return A list of pointers to the out DfgEdge objects.
     */
    std::list<DfgEdge*> getListOutEdges() const;
    

    /** Gets the list of the predecessor vertices.
     * Gets only the predecessors throw real dependencies.
     * @return A list of pointers to the predecessor DfgVertex objects.
     */
    std::list<DfgVertex*> getListPredecessorVertices() const;
    

    /** Gets the list of the successor vertices.
     * Gets only the successors throw real dependencies.
     * @return A list of pointers to the successor DfgVertex objects.
     */
    std::list<DfgVertex*> getListSuccessorVertices() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Other functions.                                                     ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Duplicates MAKE operations in one vertex for each out dependency.
     */
    virtual void duplicateMakeOperations();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the vertex when dumping in GraphML.
     * @return The string indicating the GraphML id of the vertex.
     */
    std::string getGraphmlId() const;


    /** Gets the label used to represent the vertex when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    virtual std::string getGraphmlLabel() const;


    /** Gets the html description of the vertex.
     * @return The string giving the html description of the vertex.
     */
    virtual std::string getHtmlDescription() const;


    /** Gets the xml description of the vertex.
     * @return The string giving the xml description of the vertex.
     */
    virtual std::string getXmlDescription() const;


    /** Generates the GraphML description of the vertex.
     * Concatenates to rOs the GraphML description of the vertex.
     * @param rOs An output stream.
     */
    void generateDumpGraphml(DumpToGraphml& rOs) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Declaration of DfgVertex destructor.                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Virtual destructor.
     * In order to properly call the destructor of the inherited class
     * ConstantDfgVertex, OperationDfgVertex and VariableDfgVertex.
     */
    virtual ~DfgVertex();



  protected:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Protected functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the shape used to represent the vertex when dumping in GraphML.
     * @return The string indicating which shape to use when dumping in GraphML.
     */
    virtual std::string getGraphmlShape() const;


    /** Gets the fill color used to represent the vertex when dumping in GraphML.
     * @return The string indicating which fill color to use when dumping in GraphML.
     */
    virtual std::string getGraphmlFillColor() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Protected fields.                                                    ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Uniquely indexes the vertex.
     */
    size_t mId;


    /** Indicates the next id to use when automatically generate the id.
     */
    static size_t msNextId;


    /** Links each vertex to its id.
     * Allows to find a vertex with its id.
     */
    static std::map<size_t, DfgVertex*> msTableId;


    /** Indicates the default name to use to identify the undefined vertex.
     */
    std::string mDefaultName;


    /** The descriptor of the vertex used in a Boost graph.
     */
    BoostVertexDescriptor mBoostVertex;


    /** Indicating which graph the vertex has been assigned to.
     * Value if not assigned : NULL.
     */
    DfgGraph* mpGraph;
    

    /** Indicates which activated subgraph the vertex is assigned to.
     * Value if not assigned : NULL.
     */
    Subgraph* mpEnabledSubgraph;
    

    /** Ides the vertex in topological order in its graph.
     * Needs a previous call to mpGraph->getVectorVerticesTopologicalOrder()
     * to be significant.
     */
    size_t mIdTopologicalOrder;



  };



  /** Gets the vertex kind corresponding to the name given in parameter.
   * @param name A string representing the name of the vertex kind.
   * @return The VertexKindType which have the specified name.
   */
  VertexKindType GetVertexKindFromName(const std::string name);


};

#endif // DfgVertex_h
