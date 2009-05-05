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
////    Declaration of SubgraphMeritData class and functions.                    ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file SubgraphMeritData.h
 */


#ifndef SubgraphMeritData_h
#define SubgraphMeritData_h

#include "common_config.h"
#include "OperandResult.h"

#include <string>
#include <map>
#include <set>



namespace DfgForIse {



  class DfgEdge;
  class DfgVertex;
  class Subgraph;



  /** A DFG Subgraph.
   * Class representing a subgraph (set of vertices) in a DFG graph.
   */
  class SubgraphMeritData {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Constructor.
     * @param clockPeriod A size_t indicating the clock period of the target proc.
     */
    SubgraphMeritData(const size_t clockPeriod = DEFAULT_CLOCK_PERIOD);

    /** Copy constructor.
     * @param rMeritData A reference to the SubgraphMeritData object to copy.
     */
    SubgraphMeritData(const SubgraphMeritData& rMeritData);

    /** Constructor from a subgraph.
     * @param rSubgraph A reference to the Subgraph object to compute the datas from.
     * @param clockPeriod A size_t indicating the clock period of the target proc.
     */
    SubgraphMeritData(const Subgraph& rSubgraph,
		      const size_t clockPeriod = DEFAULT_CLOCK_PERIOD);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Copy from another SubgraphMeritData object.
     * @param rMeritData A reference to the SubgraphMeritData object to copy.
     */
    void copy(const SubgraphMeritData& rMeritData);


    /** Adds an edge to the set of subgraph output edges.
     * @param pDfgEdge A DfgEdge object to add to the set of the out edges of the
     *                 corresponding subgraph.
     */
    void addOutputEdge(DfgEdge* pDfgEdge);


    /** Removes an edge from the set of subgraph output edges.
     * @param pDfgEdge A DfgEdge object to remove from the set of the out edges of
     *                 the corresponding subgraph.
     */
    void removeOutputEdge(DfgEdge* pDfgEdge);


    /** Adds an edge to the set of subgraph input edges.
     * @param pDfgEdge A DfgEdge object to add to the set of the in edges of the
     *                 corresponding subgraph.
     */
    void addInputEdge(DfgEdge* pDfgEdge);


    /** Removes an edge from the set of subgraph input edges.
     * @param pDfgEdge A DfgEdge object to remove from the set of the in edges of
     *                 the corresponding subgraph.
     */
    void removeInputEdge(DfgEdge* pDfgEdge);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get and set functions.                                               ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the set of output edges of the subgraph.
     * @return the set of the out DfgEdge objects of the corresponding subgraph.
     */
    std::set<DfgEdge*> getSetOutputEdges() const;


    /** Gets the set of input edges of the subgraph.
     * @return the set of the in DfgEdge objects of the corresponding subgraph.
     */
    std::set<DfgEdge*> getSetInputEdges() const;


    /** Gets the set of output ports of the subgraph.
     * The set of output ports is calculated from the set of out edges.
     * @return the set of the OperandResult objects, as output ports of the
     *         corresponding subgraph.
     */
    std::set<OperandResult*, OperandResultComparison> getSetOutputPorts() const;


    /** Gets the set of input ports of the subgraph.
     * The set of input ports is calculated from the set of out edges.
     * @return the set of the OperandResult objects, as input ports of the
     *         corresponding subgraph.
     */
    std::set<OperandResult*, OperandResultComparison> getSetInputPorts() const;


    /** Gets the number of output ports of the subgraph.
     * @return The size_t indicating the number of output ports.
     */
    size_t getNumOutputPorts() const;


    /** Gets the number of input ports of the subgraph.
     * @return The size_t indicating the number of input ports.
     */
    size_t getNumInputPorts() const;


    /** Sets the estimated software latency of the subgraph.
     * @param softwareLatency A size_t indicating the software latency.
     */
    void setSoftwareLatencyCycles(const size_t softwareLatency);


    /** Gets the estimated software latency of the subgraph.
     * @return The size_t indicating the software latency.
     */
    size_t getSoftwareLatencyCycles() const;


    /** Sets the estimated hardware latency of the subgraph.
     * @param hardwareLatency A size_t indicating the hardware latency.
     */
    void setHardwareLatencyTime(const size_t hardwareLatency);


    /** Gets the estimated hardware latency of the subgraph.
     * @return The size_t indicating the hardware latency.
     */
    size_t getHardwareLatencyTime() const;


    /** Gets the estimated hardware latency of the subgraph.
     * @return The size_t indicating the hardware latency.
     */
    size_t getHardwareLatencyCycles() const;


    /** Sets the frequency.
     * @param frequency A size_t representing the frequency the corresponding
     *                  subgraph is used.
     */
    void setFrequency(const size_t frequency);


    /** Gets the frequency.
     * @return The size_t representing the frequency the subgraph is used.
     */
    size_t getFrequency() const;


    /** Sets the period of the clock in 100th nanosecond.
     * @param clockPeriod A size_t indicating the clock period of the target proc.
     */
    void setClockPeriod(const size_t clockPeriod);


    /** Gets the period of the clock in nanosecond.10-2.
     * @return A size_t indicating the clock period of the target proc.
     */
    size_t getClockPeriod() const;


    /** Gets the merit of the subgraph.
     * It is calculated from the datas.
     * @return The size_t indicating the merit.
     */
    size_t getMerit() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the html description of the subgraph merit data.
     * @return The string giving the html description of the subgraph merit data.
     */
    std::string getHtmlDescription() const;



    /////////////////////////////////////////////////////////////////////////////////
    ////    Operators.                                                           ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Operator checking if the first merit data is better than the second one.
     * @param rMeritData1 A reference to the first SubgraphMeritData object
     *                    to check.
     * @param rMeritData2 A reference to the second SubgraphMeritData object
     *                    to check.
     * @return A bool indicating if the second merit data is strictly better than
     *         the first merit data.
     */
    friend bool operator<(const SubgraphMeritData& rMeritData1,
			  const SubgraphMeritData& rMeritData2);


    /** Operator checking if the first merit data is equivalent to the second one.
     * @param rMeritData1 A reference to the first SubgraphMeritData object
     *                    to check.
     * @param rMeritData2 A reference to the second SubgraphMeritData object
     *                    to check.
     * @return A bool indicating if the second merit data is equivalent to
     *         the first merit data.
     */
    friend bool operator==(const SubgraphMeritData& rMeritData1,
			   const SubgraphMeritData& rMeritData2);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Destructor.                                                          ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Destructor.
     */
    ~SubgraphMeritData();



  private:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////
    

    /** Gets the hardware latency of the critical path from the given vertex
     *  to leave the subgraph.
     * @param pDfgVertex A pointer to a DfgVertex object from which to calculate the
     *                   hardware latency of the critical path.
     * @param rSubgraph A reference to a Subgraph object in which to search the
     *                  critical path.
     * @return A size_t indicating the hardware latency of the critical path from
     *         the given vertex to leave the subgraph.
     */
    size_t getHardwareLatencyTimeCriticalPath(const DfgVertex* pDfgVertex,
					      const Subgraph& rSubgraph);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Indicates the output edges of the subgraph.
     */
    std::set<DfgEdge*> mSetOutputEdges;


    /** Indicates the input edges of the subgraph.
     */
    std::set<DfgEdge*> mSetInputEdges;


    /** Indicates the output ports of the subgraph.
     */
    std::set<OperandResult*, OperandResultComparison> mSetOutputPorts;


    /** Indicates for each output port, the number of time it is used.
     */
    std::map<OperandResult*, size_t, OperandResultComparison> mMapNumOutputPorts;


    /** Indicates the input ports of the subgraph.
     */
    std::set<OperandResult*, OperandResultComparison> mSetInputPorts;


    /** Indicates for each input port, the number of time it is used.
     */
    std::map<OperandResult*, size_t, OperandResultComparison> mMapNumInputPorts;


    /** Indicates the estimated software latency of the subgraph.
     */
    size_t mSoftwareLatencyCycles;


    /** Indicates the estimated hardware latency of the subgraph.
     */
    size_t mHardwareLatencyTime;


    /** Indicates the frequency of the subgraph.
     */
    size_t mFrequency;


    /** Indicates the period length of the processor clock in 100th nanosecond.
     */
    size_t mClockPeriod;



  };



};

#endif // SubgraphMeritData_h
