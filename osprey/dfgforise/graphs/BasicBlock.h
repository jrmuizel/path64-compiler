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
////    Declaration of BasicBlock class and functions.                           ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file BasicBlock.h
 */


#ifndef BasicBlock_h
#define BasicBlock_h

#include "common_config.h"

#include "DumpToGraphml.h"

#include "DfgGraph.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>



namespace DfgForIse {



  class Function;
  class DfgGraph;
  class DfgVertex;
  class DfgEdge;
  class Subgraph;



  /** A basic block.
   * Class representing a basic block, inherited from DfgGraph.
   */
  class BasicBlock : public DfgGraph {



    friend class Function;
    friend class Subgraph;



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Default constructor.
     * @param id The size_t representing the id of the graph.
     */
    BasicBlock(const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Adds a subgraph to the graph.
     * Note : Every vertex of a subgraph must belong to the same graph than itself ;
     *        Side effect on field mpEnabledSubgraph of each DfgVertex object
     *          belonging to the subgraph.
     * @param pSubgraph A pointer to a Subgraph object.
     */
    void enableSubgraph(Subgraph* pSubgraph);
    

    /** Adds a subgraph to the graph, identified by its unique id.
     * Note : Every vertex of a subgraph must belong to the same graph than itself ;
     *        The id must exist in Subgraph's id table ;
     *        Side effect on field mpEnabledSubgraph of each DfgVertex object
     *          belonging to the subgraph.
     * @param idSubgraph A size_t representing a unique id on Subgraph.
     */
    void enableSubgraph(const size_t idSubgraph);


    /** Clears the activation of all subgraphs in this graph ;
     * Note : Side effect on field mpEnabledSubgraph of each DfgVertex object
     *          belonging to the subgraph.
     */
    void disableAllSubgraphs();


    /** Duplicates MAKE operations in one vertex for each out dependency.
     */
    void duplicateMakeOperations();


    /** Clears the vertices and edges belonging to the graph ;
     * Note : Delete every object belonging to the graph.
     */
    void clear();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the next graph if automatically indexed.
     * @return The size_t id of the next graph if automatically indexed.
     */
    static size_t getNextId();


    /** Sets the frequency the portion of code associated with the DFG is executed.
     * @param frequency The size_t representing the frequency this code is executed.
     */
    void setFrequency(const size_t frequency);


    /** Gets the frequency the portion of code associated with the DFG is executed.
     * @return The size_t representing the frequency this code is executed.
     */
    size_t getFrequency() const;


    /** Gets the maximum frequency to compare the frequency to (generally, it is
     *  the maximum frequency of the application).
     * @return The size_t representing the maximum frequency.
     */
    size_t getMaximumFrequency() const;


    /** Gets the function the basic block is belonging to.
     * @return A pointer to the Function object.
     */
    Function* getFunction() const;


    /** Gets the function name the basic block is belonging to.
     * @return A string indicating the function name.
     */
    std::string getFunctionName() const;


    /** Gets the name of the source file the basic block is belonging to.
     * @return The string indicating the source file name.
     */
    std::string getSourceFileName() const;


    /** Sets the list of labels associated to the bb.
     * @param labels A string representing the list of labels 
     *               associated to the bb.
     */
    void BasicBlock::setLabels(const std::string labels);


    /** Gets the list of labels associated to the bb.
     * @return The string representing the list of labels 
     *               associated to the bb.
     */
    std::string BasicBlock::getLabels() const ;

    
    /** Sets the line of the code in source file.
     * @param sourceFileLine A size_t representing the line of the code in
     *                       source file.
     */
    void setSourceFileLine(const size_t sourceFileLine);


    /** Gets the line of the code in source file.
     * @return The size_t representing the line of the code in source file.
     */
    size_t getSourceFileLine() const;


    /** Gets the set of enabled subgraphs of the basic block.
     * @return The set of enabled Subgraph objects.
     */
    std::set<Subgraph*> getSetEnabledSubgraphs() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the basic block when dumping in GraphML.
     * @return The string GraphML id of the basic block.
     */
    std::string getGraphmlId() const;


    /** Gets the label used to identify the basic block when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    std::string getGraphmlLabel() const;


    /** Gets the html description of the basic block.
     * @return The string giving the html description of the basic block.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the basic block.
     * @return The string giving the xml description of the basic block.
     */
    std::string getXmlDescription() const;


    /** Generates the GraphML description of the graph.
     * Concatenates to rOs the GraphML description of the graph.
     * @param rOs An output stream.
     */
    void generateDumpGraphml(DumpToGraphml& rOs) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~BasicBlock();



  private:
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the string representing the fill color to use for dumping
     *  the basic block in GraphML.
     * @return The string giving the fill color of the graph.
     */
    std::string getGraphmlFillColor() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
 
    /** Indicates the next id to use when automatically generate the id.
     */
    static size_t msNextId;


    /** Indicates the estimated frequency the basic block is executed.
     */
    size_t mFrequency;


    /** Indicates the function the basic block is belonging to.
     */
    Function* mpFunction;


    /** Indicates the list of labels attached to the basic block
     */
    std::string mLabels;

    /** Indicates the line in the source file from which the basic block has been
     *  computed.
     */
    size_t mSourceFileLine;


    /** Lists the enabled subgraphs of this graph.
     */
    std::set<Subgraph*> mSetEnabledSubgraphs;



  };



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Declaration of BasicBlockComparison class and functions.               ////
  ///////////////////////////////////////////////////////////////////////////////////

  /** Comparison class used for the priority queue on BasicBlock objects.
   */
  class BasicBlockComparison {



  public:



    /** Default constructor.
     */
    BasicBlockComparison();


    /** Comparison operator on BasicBlock objects.
     * Note : The two arguments are supposed to point to existing BasicBlock
     *        objects.
     * @param lhs A pointer to the left hand BasicBlock.
     * @param rhs A pointer to the right hand BasicBlock.
     * @return true iif the frequency of the right basic block is higher than the
     *         left ones,
     *         and if they are equal, the index of the right basic block is lower
     *         than the left ones.
     */
    bool operator() (const BasicBlock* lhs,
		     const BasicBlock* rhs) const;



  };



};

#endif // BasicBlock_h
