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
////    Declaration of DumpToGraphml class and functions.                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file DumpToGraphml.h
 */


#ifndef DumpToGraphml_h
#define DumpToGraphml_h

#include "DfgExternalTypes.h"

#include <iostream>
#include <iomanip>
#include <fstream>



namespace DfgForIse {



  /** A graphml files dumping class.
   * Class for dumping DFGs and CFGs to graphml files.
   */
  class DumpToGraphml : public std::ofstream {



  public:



    /////////////////////////////////////////////////////////////////////////////////
    ////    Constructor.                                                         ////
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Constructor.
     * @param maxFrequency A size_t indicating the maximum frequency to dump in the
     *                     header of the graphml files.
     * @param printFormating A bool indicating if the graphml formating for yEd has
     *                       to be printed in graphml files.
     * @param printCfg A bool indicating if the CFGs have to be printed in
     *                 graphml files.
     */
    DumpToGraphml(const size_t maxFrequency = 0,
		  const bool printFormating = true,
		  const bool printCfg = true);
    
    
    /** Constructor with file opening.
     * @param filename A char* indicating the name of the file to open.
     * @param mode Indicates the opening mode to use to open the file.
     * @param maxFrequency A size_t indicating the maximum frequency to dump in the
     *                     header of the graphml files.
     * @param printFormating A bool indicating if the graphml formating for yEd has
     *                       to be printed in graphml files.
     * @param printCfg A bool indicating if the CFGs have to be printed in
     *                 graphml files.
     */
    explicit DumpToGraphml(const char * filename,
			   ios_base::openmode mode = ios_base::out,
			   const size_t maxFrequency = 0,
			   const bool printFormating = true,
			   const bool printCfg = true);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Modifying functions.                                                 ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Sets the maximum frequency of the application to dump.
     * @param maxFrequency A size_t indicating the maximum frequency to dump in the
     *                     header of the graphml files.
     */
    void setMaximumFrequency(const size_t maxFrequency);


    /** Enables or disables the formating printing for yEd.
     * @param printFormating A bool indicating if the graphml formating for yEd has
     *                       to be printed in graphml files.
     */
    void setPrintFormating(const bool printFormating);


    /** Enables or disables the CFG printing.
     * @param printCfg A bool indicating if the graphml formating for yEd has
     *                       to be printed in graphml files.
     */
    void setPrintCfg(const bool printCfg);


    /** Enables or disables the non real edges printing.
     * @param printNonRealEdges A bool indicating if the edges representing
     *                         non real dependencies have to be printed.
     */
    void setPrintNonRealEdges(const bool printNonRealEdges);


    /** Enables or disables the print of an edge kind.
     * @param enabled A bool indicating if the printing is enabled for the given
     *                edge kind.
     * @param kind The edge kind to enable or disable.
     */
    void setPrintEdgeKind(const bool enabled, const EdgeKindType kind);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Get functions.                                                       ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Gets the maximum frequency of the application.
     * @return The size_t indicating the maximum frequency to dump in the header
     *         of the graphml files.
     */
    size_t getMaximumFrequency();


    /** Gets if the formating printing is enabled.
     * @return The bool indicating if the graphml formating for yEd has to be
     *         printed in graphml files.
     */
    bool isEnabledPrintFormating();


    /** Gets if the CFG printing is enabled.
     * @return The bool indicating if the graphml formating for yEd has to be
     *         printed in graphml files.
     */
    bool isEnabledPrintCfg();


    /** Gets if the non real edges printing is enables.
     * @return The bool indicating if the edges representing non real dependencies
     *         have to be printed.
     */
    bool isEnabledPrintNonRealEdges();


    /** Gets if the given edge kind is enabled for printing.
     * @param kind The edge kind to enable or disable.
     * @return The bool indicating if the printing is enabled for the given edge
     *         kind.
     */
    bool isEnabledPrintEdgeKind(const EdgeKindType kind);



    /////////////////////////////////////////////////////////////////////////////////
    ////    Dumping functions.                                                   ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Dumps the header of the graphml file.
     * Call it one time before printing anything in the graphml file ;
     * Be sure to Initialize all the printing parameters before.
     */
    void dumpGraphmlHeader();


    /** Dumps the header of the graphml file.
     * Call it one time after having printed your objects in the graphml file.
     */
    void dumpGraphmlFooter();



  private:
    


    /////////////////////////////////////////////////////////////////////////////////
    ////    Private fields.                                                      ////
    /////////////////////////////////////////////////////////////////////////////////

    /** Indicates the maximum frequency to dump in the header of the graphml files.
     */
    size_t mMaxFrequency;


    /** Indicates if the graphml formating for yEd has to be printed
     *  in graphml files.
     */
    bool mPrintFormating;


    /** Indicates if the graphml formating for yEd has to be printed
     *  in graphml files.
     */
    bool mPrintCfg;


    /** Indicates if the edges representing non real dependencies have to be
     * printed in graphml files.
     */
    bool mPrintNonRealEdges;


    /** Indicates for each edge kind if it has to be printed in graphml files.
     */
    bool mPrintEdgeKind[EDGE_INVALID];



  };



};

#endif // DumpToGraphml_h
