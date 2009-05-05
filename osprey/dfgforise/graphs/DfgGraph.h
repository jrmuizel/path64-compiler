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
////    Declaration of DfgGraph class and functions.                             ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgGraph.h
 */


#ifndef DfgGraph_h
#define DfgGraph_h

#include "DumpToGraphml.h"

#include "DfgInternalTypes.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>



namespace DfgForIse {



  class DfgVertex;
  class DfgEdge;



  /** A Data Flow Graph (Abstract class).
   * Main class representing a DFG extended with input-output variables.
   */
  class DfgGraph {



  public:



    friend class DfgVertex;
    friend class DfgEdge;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Adds a vertex to the graph.
     * Note : A vertex which is still belonging to a graph can't be assigned ;
     *        Side effect on DfgVertex fields mpGraph and mBoostVertex.
     * @param pDfgVertex A pointer to a DfgVertex object.
     */
    void addVertex(DfgVertex* pDfgVertex);


    /** Adds a vertex to the graph, identified by its unique id.
     * Note : A vertex which is still belonging to a graph can't be assigned ;
     *        The id must exist in DfgVertex's id table.
     *        Side effect on DfgVertex fields mpGraph and mBoostVertex.
     * @param idVertex A size_t representing a unique id on DfgVertex.
     */
    void addVertex(const size_t idVertex);


    /** Removes a vertex from the graph.
     * @param pDfgVertex A pointer to a DfgVertex object.
     */
    void removeVertex(DfgVertex* pDfgVertex);


    /** Adds an edge to the graph between two vertices.
     * Note : An edge can be added only if the two vertices are effectively present
     *        in this graph ;
     *        Side effect on DfgEdge fields mpGraph and mBoostEdge.
     * @param pSourceVertex A pointer to a DfgVertex object, the in vertex.
     * @param pTargetVertex A pointer to a DfgVertex object, the out vertex.
     * @param pDfgEdge A pointer to a DfgEdge object.
     */
    void addEdge(DfgVertex* pSourceVertex, DfgVertex* pTargetVertex,
		 DfgEdge* pDfgEdge);


    /** Adds an edge to the graph between two vertices identified by their unique
     *  ides.
     * Note : An edge can be added only if the two vertices are effectively present
     *          in this graph ;
     *        Side effect on DfgEdge fields mpGraph and mBoostEdge.
     * @param idSource A size_t representing a unique id on DfgVertex of the
     *                    in vertex.
     * @param idTarget A size_t representing a unique id on DfgVertex of the
     *                    out vertex.
     * @param pDfgEdge A pointer to a DfgEdge object.
     */
    void addEdge(const size_t idSource, const size_t idTarget,
		 DfgEdge* pDfgEdge);


    /** Removes an edge from the graph.
     * @param pDfgEdge A pointer to a DfgEdge object.
     */
    void removeEdge(DfgEdge* pDfgEdge);


    /** Clears the vertices and edges belonging to the graph ;
     * Note : Delete every object belonging to the graph.
     */
    virtual void clear();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the graph.
     * @return The size_t id of the graph.
     */
    size_t getId() const;


    /** Sets the name of the graph.
     * @param name A string indicating the name of the graph.
     */
    void setName(const std::string name);


    /** Gets the name of the graph.
     * @return The string indicating the name of the graph.
     */
    std::string getName() const;


    /** Gets the number of vertices in the graph.
     * @return A size_t indicating the number of vertices of the graph.
     */
    size_t getNumVertices() const;


    /** Gets the vector of vertices of the graph in topological order.
     * @return A vector of pointers to DfgVertex objects in topological order.
     */
    std::vector<DfgVertex*> getVectorVerticesTopologicalOrder() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the graph when dumping in GraphML.
     * @return The string GraphML id of the graph.
     */
    virtual std::string getGraphmlId() const = 0;


    /** Gets the label used to identify the graph when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    virtual std::string getGraphmlLabel() const = 0;


    /** Gets the html description of the graph.
     * @return The string giving the html description of the graph.
     */
    virtual std::string getHtmlDescription() const;


    /** Gets the xml description of the graph.
     * @return The string giving the xml description of the graph.
     */
    virtual std::string getXmlDescription() const;


    /** Generates the GraphML description of the graph.
     * Concatenates to rOs the GraphML description of the graph.
     * @param rOs An output stream.
     */
    virtual void generateDumpGraphml(DumpToGraphml& rOs) const = 0;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Virtual Destructor.
     */
    virtual ~DfgGraph();



  protected:
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Protected functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the string representing the fill color to use for dumping
     *  the graph in GraphML.
     * @return The string giving the fill color of the graph.
     */
    virtual std::string getGraphmlFillColor() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Protected fields.                                                    ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Indicates the id of the graph.
     */
    size_t mId;


    /** Indicates the name of the graph.
     */
    std::string mName;


    /** Represents the DFG as a Boost graph.
     */
    BoostDfg mBoostDfg;



  };



};

#endif // DfgGraph_h
