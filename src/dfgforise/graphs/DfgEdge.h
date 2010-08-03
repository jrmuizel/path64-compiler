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
////    Declaration of DfgEdge class and functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DfgEdge.h
 */


#ifndef DfgEdge_h
#define DfgEdge_h

#include "DumpToGraphml.h"

#include "DfgExternalTypes.h"
#include "DfgInternalTypes.h"

#include <string>
#include <iostream>
#include <vector>
   


namespace DfgForIse {



  class BasicBlock;
  class DfgGraph;
  class Subgraph;
  class DfgVertex;
  class OperandResult;



  /** A DFG Edge.
   * Abstract class representing an edge in a DFG graph.
   */
  class DfgEdge {
  


  public:



    friend class BasicBlock;
    friend class DfgGraph;
    friend class Subgraph;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor with the place of the operand in the target operation.
     * @param edgeKind The kind of the edge.
     */
    DfgEdge(EdgeKindType edgeKind);

    /** Copy Constructor.
     * @param pDfgEdge An edge to copy the datas from.
     */
    DfgEdge(DfgEdge* pDfgEdge);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the kind of the edge.
     * @return The EdgeKindType enum which indicates the kind of edge.
     */
    EdgeKindType getKind() const;


    /** Gets the source vertex of the edge.
     * @return A pointer on the DfgVertex representing the source vertex of the edge.
     */
    DfgVertex* getSourceVertex() const;


    /** Gets the target vertex of the edge.
     * @return A pointer on the DfgVertex representing the target vertex of the edge.
     */
    DfgVertex* getTargetVertex() const;


    /** Sets the place of the operand in the target operation.
     * @param operandRankInTargetOperation An int representing the place of the operand.
     */
    void setOperandRankInTargetOperation(int operandRankInTargetOperation);


    /** Gets the place of the operand in the target operation.
     * @return The int indicating the place of the operand.
     */
    int getOperandRankInTargetOperation() const;


    /** Sets if the operand in the target operation is a false predication.
     * @param operandIsFalsePredication A bool indicating if the operand is
     *                                  a false predication.
     */
    void setOperandIsFalsePredication(const bool operandIsFalsePredication);


    /** Gets if the operand in the target operation is a false predication.
     * @return The bool indicating if the operand is a false predication.
     */
    bool getOperandIsFalsePredication() const;


    /** Sets the place of the result in the source operation.
     * @param resultRankInSourceOperation An int representing the place of the result.
     */
    void setResultRankInSourceOperation(int resultRankInSourceOperation);


    /** Gets the place of the result in the source operation.
     * @return The int indicating the place of the result.
     */
    int getResultRankInSourceOperation() const;


    /** Sets the operand/result associated with the edge.
     * @param pOperandResult A pointer to an OperandResult object
     *                       associated with the edge.
     */
    void setOperandResult(OperandResult* pOperandResult);


    /** Gets the operand/result associated with the edge.
     * @return A pointer to the OperandResult object associated with the edge.
     */
    OperandResult* getOperandResult() const;


    /** Gets if the edge corresponds to a real dependency.
     * A real dependency is a 'define->use' dependency.
     * @return A bool indicating if the edge is a real dependency.
     */
    bool isRealDependency() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the label used to represent the edge when dumping in GraphML.
     * @return The string indicating which name to use when dumping in GraphML.
     */
    std::string getGraphmlLabel() const;


    /** Gets the html description of the edge.
     * @return The string giving the html description of the edge.
     */
    std::string getHtmlDescription() const;


    /** Gets the xml description of the edge.
     * @return The string giving the xml description of the edge.
     */
    std::string getXmlDescription() const;


    /** Generates the GraphML description of an edge of the graph.
     * Concatenates to rOs the GraphML description of the edge of the graph.
     * @param rOs An output stream.
     */
    void generateDumpGraphml(DumpToGraphml& rOs) const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Destructor.
     */
    ~DfgEdge();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the string representing the color to use for dumping
     *  the edge in GraphML.
     */
    std::string getGraphmlLineColor() const;


    /** Gets the string representing the line type to use for dumping
     *  the edge in GraphML.
     */
    std::string getGraphmlLineType() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Indicates the kind of the edge.
     */
    EdgeKindType mKind;


    /** Indicates the number of the operand in the target operation.
     */
    int mOperandRankInTargetOperation;


    /** Indicates if the operand in the target operation has a false predication.
     *  Default value = false.
     */
    bool mOperandIsFalsePredication;


    /** Indicates the number of the result in the source operation.
     */
    int mResultRankInSourceOperation;


    /** Indicates the OperandResult object associated with the edge.
     */
    OperandResult* mpOperandResult;


    /** The descriptor of the edge used in a Boost graph.
     */
    BoostEdgeDescriptor mBoostEdge;


    /** Indicates which graph the edge has been assigned to.
     * Value if not assigned : NULL.
     */
    DfgGraph* mpGraph;
    

    /** Indicates which activated subgraph the edge is assigned to.
     * Value if not assigned : NULL.
     */
    Subgraph* mpEnabledSubgraph;



  };



  /** Gets the edge kind corresponding to the name given in parameter.
   * @param name A string representing the name of the edge kind.
   * @return The EdgeKindType which have the specified name.
   */
  EdgeKindType GetEdgeKindFromName(const std::string name);



};

#endif // DfgEdge_h
