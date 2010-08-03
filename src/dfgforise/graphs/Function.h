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
////    Declaration of Function class and functions.                             ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file Function.h
 */


#ifndef Function_h
#define Function_h

#include "DumpToGraphml.h"

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <set>



namespace DfgForIse {



  class BasicBlock;
  class Variable;



  /** A function containing a list of basic blocks.
   * Class representing a function in an application.
   */
  class Function {



    friend class BasicBlock;



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Default constructor.
     * @param id The size_t representing the id of the function.
     */
    Function(const size_t id = msNextId);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Adds a basic block to the function.
     * Note : A basic block with an existing index will be overwriten.
     *        Side effect on BasicBlock field mpFunction.
     * @param pBasicBlock A pointer to a BasicBlock object.
     */
    void addBasicBlock(BasicBlock* pBasicBlock);


    /** Removes a basic block from the function.
     * @param pBasicBlock A pointer to a BasicBlock object.
     *        Side effect on BasicBlock field mpFunction.
     */
    void removeBasicBlock(BasicBlock* pBasicBlock);


    /** Adds a variable to the function.
     * Note : A variable with an existing index will be overwriten.
     * @param pVariable A pointer to a variable object.
     */
    void addVariable(Variable* pVariable);


    /** Adds an edge between the graphs.
     * Note : adds the edge even if the indexes does not correspond to any basic
     *        block in the set.
     * @param sourceId A size_t indicating the id of the source basic block.
     * @param targetId A size_t indicating the id of the target basic block.
     */
    void addEdge(size_t sourceId, size_t targetId);


    /** Removes an edge between two graphs.
     * @param sourceId A size_t indicating the id of the source basic block.
     * @param targetId A size_t indicating the id of the target basic block.
     */
    void removeEdge(size_t sourceId, size_t targetId);


    /** Duplicates MAKE operations in one vertex for each out dependency.
     */
    void duplicateMakeOperations();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Set and get functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the set of graphs.
     * @return The size_t id of the set of graphs.
     */
    size_t getId() const;


    /** Gets the id of the next set of graphs if automatically indexed.
     * @return The size_t id of the next set of graphs if automatically indexed.
     */
    static size_t getNextId();


    /** Gets a pointer to the Function object identified by its unique id.
     * @param id A size_t representing a unique id on Function.
     * @return A pointer to the Function object which has the given id.
     *         Returns NULL if the id has no corresponding set of graphs.
     */
    static Function* getFunctionById(const size_t id);


    /** Gets a pointer to the BasicBlock object identified by its unique id.
     * @param id A size_t representing a unique id on BasicBlock.
     * @return A pointer to the BasicBlock object which has the given id.
     *         Returns NULL if the id has no corresponding basic block.
     */
    BasicBlock* getBasicBlockById(const size_t id) const;


    /** Gets and/or creates the Variable object with the corresponding id.
     * If it already exists, sets the name of the variable,
     * if not, creates a new object with the values in parameter.
     * @param id A size_t argument for the id of the variable. 
     * @param name A string argument representing the name of the variable.
     * @param bitSize A size_t indicating the size of the variable.
     * @return A pointer to a Variable object with the given id and name.
     */
    Variable* getOrCreateVariable(const size_t id,
				  const std::string name,
				  const size_t bitSize = 32);


    /** Sets the name of the set of graphs.
     * @param name A string indicating the name of the set of graphs.
     */
    void setName(const std::string name);


    /** Gets the name of the set of graphs.
     * @return The string indicating the name of the set of graphs.
     */
    std::string getName() const;


    /** Sets the maximum frequency to compare the frequency to (generally, it is
     *  the maximum frequency of the application).
     * @param maximumFrequency The size_t representing the maximum frequency.
     */
    void setMaximumFrequency(const size_t maximumFrequency);


    /** Gets the maximum frequency to compare the frequency to (generally, it is
     *  the maximum frequency of the application).
     * @return The size_t representing the maximum frequency.
     */
    size_t getMaximumFrequency() const;


    /** Sets the name of the corresponding source file.
     * @param sourceFileName A string indicating the name of the source file.
     */
    void setSourceFileName(const std::string sourceFileName);


    /** Gets the name of the corresponding source file.
     * @return The string indicating the name of the source file.
     */
    std::string getSourceFileName() const;


    /** Sets the line in the source file.
     * @param sourceFileLine A size_t representing the line of the code in
     *                       source file.
     */
    void setSourceFileLine(const size_t sourceFileLine);


    /** Gets the line in the source file.
     * @return The size_t representing the line of the code in source file.
     */
    size_t getSourceFileLine() const;


    /** Gets the list of basic blocks belonging to the function.
     * @return The list of pointers to BasicBlock objects.
     */
    std::list<BasicBlock*> getListBasicBlocks();


    /** Checks if the function contains no basic block.
     * @return A bool indicating if no basic block is belonging to the list.
     */
    bool empty();



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the id of the function when dumping in GraphML.
     * @return The string GraphML id of the function.
     */
    std::string getGraphmlId() const;


    /** Gets the label used to identify the function when dumping in Graphml.
     * @return The string indicating which name to use when dumping in Graphml.
     */
    std::string getGraphmlLabel() const;


    /** Gets the xml description of the set of graphs.
     * @return The string giving the xml description of the set of graphs.
     */
    std::string getXmlDescription() const;


    /** Generates the GraphML description of the graphs in the set.
     * Concatenates to rOs the GraphML description of the graphs in the set.
     * @param rOs An output stream.
     */
    void generateDumpGraphml(DumpToGraphml& rOs) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~Function();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the html description of the function.
     * @return The string giving the html description of the function.
     */
    std::string getHtmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////
  
    /** Indicates the id of the set of graphs.
     */
    size_t mId;


    /** Indicates the next id to use when automatically generate the id.
     */
    static size_t msNextId;


    /** Links each set of graphs to its id.
     * Allows to find a set of graphs with its id.
     */
    static std::map<size_t, Function*> msTableId;


    /** Indicates the name of the graph set (which can correspond to a function).
     */
    std::string mName;


    /** Indicates the maximum frequency to compare basic block frequencies to.
     */
    size_t mMaximumFrequency;


    /** Indicates the name of the source file from which the graphset has been
     *  computed.
     */
    std::string mSourceFileName;


    /** Indicates the line in the source file from which the graphset has been
     *  computed.
     */
    size_t mSourceFileLine;


    /** Lists the BasicBlock objects of the function.
     */
    std::map<size_t, BasicBlock*> mMapBasicBlocks;


    /** Lists the edges between the basic blocks.
     */
    std::set< std::pair<size_t,size_t> > mSetEdges;


    /** Lists the variables used in the function.
     */
    std::map<size_t, Variable*> mMapVariables;



  };



};


#endif // Function_h
