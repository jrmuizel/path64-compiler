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
////    Declaration of Subgraph class and functions.                             ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Subgraph.h
 */


#ifndef Subgraph_h
#define Subgraph_h

#include "DumpToGraphml.h"

#include <string>
#include <iostream>
#include <set>
#include <map>



namespace DfgForIse {



  class BasicBlock;
  class DfgVertex;
  class DfgEdge;
  class SubgraphMeritData;



  /** A DFG Subgraph.
   * Class representing a subgraph (set of vertices) in a DFG graph.
   */
  class Subgraph {



  public:



    friend class BasicBlock;
    friend class DfgVertex;
    friend class DfgEdge;
    friend class SubgraphMeritData;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor with the BasicBlock.
     * @param rBasicBlock A reference on the BasicBlock object
     *                    the subgraph is belonging to.
     * @param id The size_t representing the id of the vertex.
     */
    Subgraph(BasicBlock& rBasicBlock, const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Adds a vertex to the subgraph.
     * @param pDfgVertex A pointer to a DfgVertex object.
     * @return The bool indicating if the vertex has been added or not.
     * A vertex which is still belonging to a subgraph can't be assigned.
     * The subgraph must be linked to the same graph than the vertex.
     */
    void addVertex(DfgVertex* pDfgVertex);


    /** Adds a vertex to the subgraph identified by its unique id.
     * @param idVertex A size_t representing a unique id on DfgVertex.
     * @return The bool indicating if the vertex has been added or not.
     * A vertex which is still belonging to a subgraph can't be assigned.
     * An id must exist in Vertex table.
     * The subgraph must be linked to the same graph than the vertex.
     */
    void addVertex(const size_t idVertex);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the unique id of the subgraph.
     * @return The size_t unique id of the subgraph.
     */
    size_t getId() const;


    /** Gets the id of the next vertex if automatically indexed.
     * @return The size_t id of the next vertex if automatically indexed.
     */
    static size_t getNextId();


    /** Gets a pointer to the Subgraph object identified by its unique id.
     * @param id A size_t representing a unique id on Subgraph.
     * @return A pointer to the Subgraph object which has the given id,
     * return 0 if the id has no corresponding id.
     */
    static Subgraph* getSubgraphById(const size_t id);


    /** Gets the set of vertices the subgraph contains.
     * @return A set of the vertices of the subgraph.
     */
    std::set<DfgVertex*> getSetVertices() const;


    /** Gets the set of vertices the subgraph contains.
     * @return A size_t indicating the number of vertices in the subgraph.
     */
    size_t getNumVertices() const;


    /** Gets the BasicBlock object the subgraph is linked to.
     * @return A pointer to the BasicBlock linked to the subgraph.
     */
    BasicBlock* getBasicBlock() const;


    /** Sets the subgraph datas and merit.
     * @param pSubgraphMeritData A pointer to the SubgraphMeritData object
     *                          representing the datas of the subgraph.
     */
    void setMeritData(SubgraphMeritData* pSubgraphMeritData);


    /** Gets the subgraph datas and merit.
     * @return A pointer to the SubgraphMeritData object representing the datas
     *         of the subgraph.
     */
    SubgraphMeritData* getMeritData() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the subgraph when dumping in GraphML.
     * @return The string GraphML id of the subgraph.
     */
    std::string getGraphmlId() const;


    /** Gets the label used to identify the subgraph when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    std::string getGraphmlLabel() const;


    /** Gets the html description of the subgraph.
     * @return The string giving the html description of the subgraph.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the subgraph.
     * @return The string giving the xml description of the subgraph.
     */
    std::string getXmlDescription() const;


    /** Generates the GraphML description of the subgraph.
     * Concatenates to rOs the GraphML description of the subgraph.
     * @param rOs An output stream.
     */
    void generateDumpGraphml(DumpToGraphml& rOs) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Destructor.
     */
    ~Subgraph();



  protected:
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the string representing the fill color to use for dumping
     *  the subgraph in GraphML.
     * @return The string giving the fill color of the subgraph.
     */
    std::string getAndIncrGraphmlFillColor() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Uniquely indexes the subgraph.
     */
    size_t mId;


    /** Indicates the next id to use when automatically generate the id.
     */
    static size_t msNextId;


    /** Links each subgraph to its id.
     * Allows to find a subgraph with its id.
     */
    static std::map<size_t, Subgraph*> msTableId;


    /** Indicates the merit of the subgraph.
     * It is not automatically calculated, must be precised by hand.
     */
    SubgraphMeritData* mpSubgraphMeritData;


    /** Lists the vertices belonging to the subgraph.
     */
    std::set<DfgVertex*> mSetVertices;


    /** Lists the edges belonging to the subgraph.
     */
    std::set<DfgEdge*> mSetEdges;


    /** Indicates which graph the subgraph has been assigned to.
     */
    BasicBlock& mrBasicBlock;


    /** Indicates the current fill color to use when dumping the subgraph in GraphML.
     */
    static int msCurrentFillColor;


    /** Indicates how to increment the fill color to use when dumping
     *  the subgraph in GraphML.
     */
    static short msIncrFillRGB;



  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Declaration of SubgraphComparison class and functions.                 ////
  ///////////////////////////////////////////////////////////////////////////////////

  /** Comparison class used for the priority queue on Subgraph objects.
   */
  class SubgraphComparison {



  public:



    /** Default constructor.
     */
    SubgraphComparison();


    /** Comparison operator on Subgraph objects.
     * Note : The two arguments are supposed to point to existing Subgraph
     *        objects.
     * @param lhs A pointer to the left hand Subgraph.
     * @param rhs A pointer to the right hand Subgraph.
     * @return true iif the merit of the right subgraph is better than the left ones,
     *         and if they are equal, the index of the right subgraph is lower
     *         than the left ones.
     */
    bool operator() (const Subgraph* lhs,
		     const Subgraph* rhs) const;



  };



};

#endif // Subgraph_h
