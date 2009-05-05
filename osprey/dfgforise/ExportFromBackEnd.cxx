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
////    Definition of ExportFromBackEnd functions.                               ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file ExportFromBackEnd.cxx
 */


#include "ExportFromBackEnd.h"

#include "defs.h"
#include "glob.h"
#include "bb.h"

#ifdef BOOST_ENABLED

#include "tn.h"
#include "op.h"
#include "freq.h"
#include "cg_dep_graph.h"
#include "cg_flags.h"
#include "cg.h"
#include "label_util.h"
#include "SYS.h"
#include "file_util.h"
#include "tracing.h"

#include "XmlUtils.h"
#include "Constant.h"
#include "OperandResult.h"
#include "Variable.h"
#include "OpClass.h"
#include "Operation.h"

#include "DumpToGraphml.h"

#include "Function.h"
#include "BasicBlock.h"
#include "DfgExternalTypes.h"
#include "DfgVertex.h"
#include "DfgVertexConstant.h"
#include "DfgVertexOperation.h"
#include "DfgVertexVariable.h"
#include "DfgEdge.h"

#include "DfgForIse_OpCode.h"

#include <fstream>
#include <sstream>
#include <string>
#include <map>

#endif // BOOST_ENABLED

using namespace std;

namespace DfgForIse {


#ifdef BOOST_ENABLED

  /** Indicates the graphml file name for ISE.
   */
  string gGraphmlFileNameForIse;


  /** Indicates the graphml file name prefix for debug.
   */
  string gGraphmlFileNamePrefixForDebug;


  /** Indicates the maximum frequency found during compilation.
   */
  size_t gMaxFrequencyForIse;



  /** Lists all the functions found during compilation which contain
   *  computed DFGs.
   */
  list<Function*> gListFunctionsForIse;



  /** Lists all the list of functions found during compilation which contain
   *  computed DFGs for each phase of debug.
   * Links a file name to its computed functions.
   */
  map<string, list<Function*> > gMapListFunctionsForDebug;

  
  /** Indicates a unique id of the phase where we export DFGs for debug and code
   *  analysis.
   */
  size_t gIdPhaseForDebug;


  /** Indicates the current function being computed.
   */
  Function* gpCurrentFunction;


  /** Function which gets a BasicBlock corresponding to the basic block given in
   *  parameter.
   * @param bb The representation of the basic block in CGIR
   * @return A pointer to a BasicBlock object which represents the DFG of the basic
   *         block.
   */
  BasicBlock* Export_BasicBlock_From_BB(BB *bb);


  /** Function which gets a DfgVertex corresponding to the operation given in
   *  parameter.
   * @param op The representation of the operation in CGIR
   * @return A pointer to a DfgVertex object which represents the operation
   *         vertex in the DFG.
   */
  DfgVertex* Export_DfgVertex_From_OP(OP *op);


  /** Function which gets a Variable corresponding to the temporary name given in
   *  parameter.
   * @param tn The representation of the temporary name in CGIR
   * @return A pointer to a Variable object which represents the variable in the DFG.
   */
  Variable* Export_Variable_From_TN(TN *tn);

#endif // BOOST_ENABLED


  void ExportFromBackEnd_Initialize() {

#ifdef BOOST_ENABLED

    if ( (CG_dfg_ise_mask & 0x1)
	 || (CG_dfg_debug_mask & 0x1) ) {

      char *dfg_filename = Object_Dir
	? SYS_makePath(Object_Dir,SYS_baseptr(Src_File_Name))
	: SYS_baseptr(Src_File_Name);
      const char *filename = Remove_Extension(dfg_filename);

      if (CG_dfg_ise_mask & 0x1) {
	gGraphmlFileNameForIse = filename;
	gGraphmlFileNameForIse += ".graphml";
	gMaxFrequencyForIse = 0;
      }
      
      if (CG_dfg_debug_mask & 0x1) {
	gGraphmlFileNamePrefixForDebug = filename;
      }

    }

#endif // BOOST_ENABLED

  };


  void ExportFromBackEnd_Finalize() {

#ifdef BOOST_ENABLED


    ///////////////////////////////////
    //////// Export for ISE.   ////////
    ///////////////////////////////////
    if (CG_dfg_ise_mask & 0x1) {

      // Get infos from flags.
      bool dump_non_real_edges = (CG_dfg_ise_mask & 0x2);
      bool dump_cfg = (CG_dfg_ise_mask & 0x4);
      bool not_dump_formating = (CG_dfg_ise_mask & 0x8);

      // Set the maximum frequency to every function.
      list<Function*>::iterator it_function = gListFunctionsForIse.begin();
      while (it_function != gListFunctionsForIse.end()) {
	(*it_function)->setMaximumFrequency(gMaxFrequencyForIse);
	it_function++;
      }

      // Prepare the file.
      DumpToGraphml graphml_file;
      graphml_file.open (gGraphmlFileNameForIse.c_str(), fstream::trunc);
      graphml_file.setMaximumFrequency(gMaxFrequencyForIse);
      graphml_file.setPrintCfg(dump_cfg);
      graphml_file.setPrintFormating(!not_dump_formating);
      graphml_file.setPrintNonRealEdges(dump_non_real_edges);

      // Dump the list of functions.
      graphml_file.dumpGraphmlHeader();
      for (it_function = gListFunctionsForIse.begin();
	   it_function != gListFunctionsForIse.end();
	   it_function++) {
	// Dump the function.
	(*it_function)->generateDumpGraphml(graphml_file);
	// Clear the datas.
	delete (*it_function);
      }
      graphml_file.dumpGraphmlFooter();

      graphml_file.close();

      // Clear the datas.
      gListFunctionsForIse.clear();
    }


    ///////////////////////////////////
    //////// Export for debug. ////////
    ///////////////////////////////////
    if (CG_dfg_debug_mask & 0x1) {

      // Get infos from flags.
      bool dump_non_real_edges = (CG_dfg_debug_mask & 0x2);
      bool dump_cfg = (CG_dfg_debug_mask & 0x4);
      bool not_dump_formating = (CG_dfg_debug_mask & 0x8);

      map<string, list<Function*> >::iterator it_list_functions;
      for (it_list_functions = gMapListFunctionsForDebug.begin();
	   it_list_functions != gMapListFunctionsForDebug.end();
	   it_list_functions++) {

	// Prepare the file.
	DumpToGraphml graphml_file;
	graphml_file.open ((it_list_functions->first).c_str(), fstream::trunc);
	graphml_file.setPrintCfg(dump_cfg);
	graphml_file.setPrintFormating(!not_dump_formating);
	graphml_file.setPrintNonRealEdges(dump_non_real_edges);

	// Dump the list of functions.
	graphml_file.dumpGraphmlHeader();

	list<Function*>::iterator it_function;
	for (it_function = (it_list_functions->second).begin();
	     it_function != (it_list_functions->second).end();
	     it_function++) {
	  // Dump the function.
	  (*it_function)->generateDumpGraphml(graphml_file);
	  // Clear the datas.
	  delete (*it_function);
	}

	graphml_file.dumpGraphmlFooter();

	graphml_file.close();

	// Clear the datas.
	(it_list_functions->second).clear();
      }

      // Clear the datas.
      gMapListFunctionsForDebug.clear();

    }

#else // !BOOST_ENABLED
     DevWarn("Boost library has to be enabled in order to use options '-CG:dfg_ise_mask' and '-CG:dfg_debug_mask' for exporting DFGs.");
#endif // BOOST_ENABLED

    
  };


  void ExportFromBackEnd_PU_Initialize() {

#ifdef BOOST_ENABLED
    gIdPhaseForDebug = 0;

#endif // BOOST_ENABLED
  };


  void ExportFromBackEnd_PU_Finalize() {

  };



  void ExportFromBackEndForIse() {

#ifdef BOOST_ENABLED

    if (CG_dfg_ise_mask & 0x1) {

      // Construction of the current function.
      gpCurrentFunction = new Function();
      gpCurrentFunction->setName(Cur_PU_Name);
      gpCurrentFunction->setSourceFileName(Src_File_Name);

      for (BB *bb = REGION_First_BB;
	   bb != NULL;
	   bb = BB_next(bb)) {
	// Construction of the basic block.
	BasicBlock* p_basic_block = Export_BasicBlock_From_BB(bb);

	// Update the maximum frequency.
	gMaxFrequencyForIse = max(gMaxFrequencyForIse, p_basic_block->getFrequency());

	// Add the basic block to the current function.
	gpCurrentFunction->addBasicBlock(p_basic_block);

	// Add predecessor CFG edges of the basic block to the function.
	BBLIST* pred;
	FOR_ALL_BB_PREDS(bb,pred) {
	  gpCurrentFunction->addEdge(BB_id(BBLIST_item(pred)), BB_id(bb));
	}
      }

      // Add the function to the list of the application if the function is not empty.
      if (!gpCurrentFunction->empty()) {
	gListFunctionsForIse.push_back(gpCurrentFunction);
      }
    
    }

#endif // BOOST_ENABLED

  };



  void ExportFromBackEndForDebug(INT phase, const char *pname, BB *cur_bb) {

#ifdef BOOST_ENABLED

    if (CG_dfg_debug_mask & 0x1) {

      INT cur_bb_id = cur_bb ? BB_id(cur_bb) : 0;
      if (Get_Trace(TKIND_IR, phase)) {
	  if (cur_bb_id == 0 || Get_BB_Trace(cur_bb_id))
	    {
	      string file_name;
	      {
		ostringstream name;
		name << gGraphmlFileNamePrefixForDebug;
		name << gIdPhaseForDebug;
		name << "_" << pname;
		name << ".graphml";
		file_name = name.str();
	      }

	      // Construction of the current function.
	      gpCurrentFunction = new Function();
	      gpCurrentFunction->setName(Cur_PU_Name);
	      gpCurrentFunction->setSourceFileName(Src_File_Name);
	  
	      // If a basic block is specified in parameter, add this basic block
	      // to the current function.
	      if (cur_bb != NULL) {
		BasicBlock* p_basic_block = Export_BasicBlock_From_BB(cur_bb);
		gpCurrentFunction->addBasicBlock(p_basic_block);
	      }
	
	      // Else add every basic block to the function.
	      else {
		BB *bb;
		for (bb = REGION_First_BB; bb; bb = BB_next(bb))	{
		  if (Get_BB_Trace(BB_id(bb))) {
		    // Construction of the basic block.
		    BasicBlock* p_basic_block = Export_BasicBlock_From_BB(bb);
		    gpCurrentFunction->addBasicBlock(p_basic_block);
		  }
		}
	      }

	      BB *bb;
	      for (bb = REGION_First_BB; bb; bb = BB_next(bb))	{
		// Add predecessor CFG edges of the basic block to the function.
		BBLIST* pred;
		FOR_ALL_BB_PREDS(bb,pred) {
		  gpCurrentFunction->addEdge(BB_id(BBLIST_item(pred)), BB_id(bb));
		}
	      }

	      // Add the function to the list of the application if the function is not empty.
	      if (!gpCurrentFunction->empty()) {
		gMapListFunctionsForDebug[file_name].push_back(gpCurrentFunction);
	      }
	    }

	  gIdPhaseForDebug++;

	}
    }

#endif // BOOST_ENABLED
  };



#ifdef BOOST_ENABLED

  BasicBlock* Export_BasicBlock_From_BB(BB *bb) {

    OP *op;
    map<OP*, DfgVertex*> op_index_map;
    map<TN*, DfgVertex*> tn_index_map;

    BasicBlock *p_basic_block;
    DfgVertex *p_dfg_vertex, *p_dfg_vertex_target;
    DfgEdge* p_dfg_edge;


    // Compute the dependency graph.
    CG_DEP_Compute_Graph(bb, INCLUDE_ASSIGNED_REG_DEPS, NON_CYCLIC, 
			 INCLUDE_MEMREAD_ARCS, INCLUDE_MEMIN_ARCS,
			 INCLUDE_CONTROL_ARCS, NULL);


    // Initialize the BasicBlock object.
    op = BB_first_op(bb);
    p_basic_block = new BasicBlock(BB_id(bb));
    {
      ostringstream graph_name;
      graph_name<< "BB " << BB_id(bb);
      p_basic_block->setName(graph_name.str());
    }
    p_basic_block->setFrequency((size_t)ceil(PU_freq * ((BB_freq(bb)>=0)?BB_freq(bb):0)));
    p_basic_block->setSourceFileLine(op ? Srcpos_To_Line(OP_srcpos(op)) : 0);

    // Create the string containing all labels associated to the current bb
    if (BB_has_label(bb)) {
      std::ostringstream labels;
      ANNOTATION *ant;
      for (ant = ANNOT_First(BB_annotations(bb), ANNOT_LABEL);
	   ant != NULL;
	   ant = ANNOT_Next(ant, ANNOT_LABEL))
	{
	  LABEL_IDX label = ANNOT_label(ant);
	  labels << LABEL_name(label) << " " ;
	}
      p_basic_block->setLabels(labels.str());
    }

    // Add a DfgVertex for each operation of the basic block.
    FOR_ALL_BB_OPs_FWD(bb, op) {
      p_dfg_vertex = Export_DfgVertex_From_OP(op);
      op_index_map[op] = p_dfg_vertex;
      p_basic_block->addVertex(p_dfg_vertex);

      // Indicate the operands that still have to be added.
      vector<bool> operands_to_add(OP_opnds(op),true);

      // Eliminate the predicate operand.
      if (OP_find_opnd_use(op,OU_predicate)>=0) {
	operands_to_add[OP_find_opnd_use(op,OU_predicate)] = false;
      }
      // Eliminate the condition operand.
      if (OP_find_opnd_use(op,OU_condition)>=0) {
	operands_to_add[OP_find_opnd_use(op,OU_condition)] = false;
      }

      // Add a DfgEdge for all arcs preceding the operation in the dependency graph.
      for (ARC_LIST *arcs = OP_preds(op);
	   arcs;
	   arcs = ARC_LIST_rest(arcs)){
	ARC *arc = ARC_LIST_first(arcs);

	if (ARC_kind(arc) == CG_DEP_REGIN) {
	  // Get the REGIN dependencies
	  p_dfg_vertex = op_index_map[ARC_pred(arc)];
	  p_dfg_vertex_target = op_index_map[ARC_succ(arc)];

	  p_dfg_edge = new DfgEdge(EDGE_DEP_REGIN);
	  TN* tn = OP_opnd(ARC_succ(arc), ARC_opnd(arc));
	  Variable* p_variable = Export_Variable_From_TN(tn);
	  p_dfg_edge->setOperandResult(p_variable);
	  p_dfg_edge->setOperandRankInTargetOperation(ARC_opnd(arc));
	  p_dfg_edge->setOperandIsFalsePredication(OP_Pred_False(ARC_succ(arc), ARC_opnd(arc)));
	  for (INT16 index_result = 0;
	       index_result < OP_results(ARC_pred(arc));
	       index_result++) {
	    if (tn == OP_result(ARC_pred(arc), index_result)) {
	      p_dfg_edge->setResultRankInSourceOperation(index_result);
	      break;
	    }
	  }

	  p_basic_block->addEdge(p_dfg_vertex, p_dfg_vertex_target, p_dfg_edge);

	  operands_to_add[ARC_opnd(arc)] = false;
	}

	else {
	  // Get other dependencies
	  EdgeKindType kind = EDGE_INVALID;
	  if (ARC_kind(arc) < CG_DEP_NUM_KIND) {
	    kind = EdgeKindType(ARC_kind(arc));
	  }
	  if (kind != EDGE_INVALID) {
	    p_dfg_vertex = op_index_map[ARC_pred(arc)];
	    p_dfg_vertex_target = op_index_map[ARC_succ(arc)];

	    p_dfg_edge = new DfgEdge(kind);
	    if ( (kind == EDGE_DEP_MISC)
		 && (!p_dfg_vertex->isForbiddenInSelection())
		 && OP_call(ARC_succ(arc)) ) {
	      // Associate to the DEP_MISC arc the first result of the source operation.
	      // :WARNING: This is a supposition that the MISC dependency is real when
	      //           the previous vertex is authorized in selection.
	      TN* tn = OP_result(ARC_pred(arc), 0);
	      Variable* p_variable = Export_Variable_From_TN(tn);
	      p_dfg_edge->setOperandResult(p_variable);
	    }

	    p_basic_block->addEdge(p_dfg_vertex, p_dfg_vertex_target, p_dfg_edge);
	  }
	}

      }

      // Add a DfgVertex and a DfgEdge for the remaining operands.
      for (INT16 index_opnd = 0;
	   index_opnd < OP_opnds(op);
	   index_opnd++) {

	if (operands_to_add[index_opnd]) {
	  TN* tn = OP_opnd(op,index_opnd);
	  if (TN_is_constant(tn)) {

	    if (TN_has_value(tn)) { // Constant with value => Constant vertex
	      Constant* p_constant = new Constant(TN_value(tn), TN_bitwidth(tn));
	      p_dfg_vertex = new DfgVertexConstant(*p_constant);
	      p_dfg_edge = new DfgEdge(EDGE_IN_CONSTANT);
	      p_dfg_edge->setOperandResult(p_constant);
	      
	    }

	    else { // Other constant => Undefined vertex
	      ostringstream defaultName;
	      p_dfg_vertex = new DfgVertex();
	      if (TN_is_enum(tn)) {
		defaultName << "enum:" << ISA_ECV_Name(TN_enum(tn));
	      }
	      else if ( TN_is_label(tn) ) {
		LABEL_IDX lab = TN_label(tn);
		const char *name = LABEL_name(lab);
		INT64 offset = TN_offset(tn);
		if ( offset == 0 ) {
		  defaultName << "lab:" << name
			      << "(BB " << BB_id(Get_Label_BB(lab)) << ")" ;
		}
		else {
		  defaultName << "lab:" << name
			      << "(BB " << BB_id(Get_Label_BB(lab)) << ")"
			      << "+" << offset;
		}
	      } 
	      else if ( TN_is_tag(tn) ) {
		LABEL_IDX lab = TN_label(tn);
		const char *name = LABEL_name(lab);
		defaultName << "tag:" << name;
	      }
	      else if ( TN_is_symbol(tn) ) {
		ST *var = TN_var(tn);
		defaultName << "sym";
		defaultName << TN_RELOCS_Name(TN_relocs(tn));
		if (ST_class(var) == CLASS_CONST) {
		  defaultName << ":" << Targ_Print(NULL, ST_tcon_val(var));
		}
		else {
		  defaultName << ":" << ST_name(var) << "+" << TN_offset(tn);
		}
	      }
	      p_dfg_vertex->setDefaultName(defaultName.str());
	      p_dfg_edge = new DfgEdge(EDGE_IN_UNDEFINED);
	    }
	  }

	  else { // Variable vertex
	    map<TN*, DfgVertex*>::iterator it_tn = tn_index_map.find(tn);
	    Variable* p_variable = Export_Variable_From_TN(tn);
	    if (it_tn == tn_index_map.end()) {
	      p_dfg_vertex = new DfgVertexVariable(*p_variable);
	      p_basic_block->addVertex(p_dfg_vertex);
	      tn_index_map[tn] = p_dfg_vertex;
	    }
	    else {
	      p_dfg_vertex = it_tn->second;
	    }
	    p_dfg_edge = new DfgEdge(EDGE_IN_VARIABLE);
	    p_dfg_edge->setOperandResult(p_variable);
	  }

	  p_dfg_edge->setOperandRankInTargetOperation(index_opnd);
	  p_dfg_vertex_target = op_index_map[op];
	  p_basic_block->addVertex(p_dfg_vertex);
	  p_basic_block->addEdge(p_dfg_vertex, p_dfg_vertex_target, p_dfg_edge);

	  operands_to_add[index_opnd] = false;
	}
      }
    }
    tn_index_map.clear();


    {
      // Indicate the global tns that have already been added as output of the basic
      // block.
      set<TN*> set_output_gtns;

      // We explore the operations in reverse order.
      // For each global tn defined in the basic block, we find the last operation that redefines it.
      FOR_ALL_BB_OPs_REV(bb, op) {

	// Add a DfgVertex and a DfgEdge for the remaining output gtns
	// appearing as a result of the operation.
	for (INT16 index_result = 0;
	     index_result < OP_results(op);
	     index_result++) {
	  TN* tn=OP_result(op, index_result);
	  if (TN_is_global_reg(tn)) {
	    set<TN*>::iterator it_tn = set_output_gtns.find(tn);
	    if (it_tn == set_output_gtns.end()) {
	      p_dfg_vertex = op_index_map[op];

	      Variable* p_variable = Export_Variable_From_TN(tn);
	      p_dfg_vertex_target = new DfgVertexVariable(*p_variable);
	      p_basic_block->addVertex(p_dfg_vertex_target);

	      p_dfg_edge = new DfgEdge(EDGE_OUT_VARIABLE);
	      p_dfg_edge->setOperandResult(p_variable);
	      p_dfg_edge->setResultRankInSourceOperation(index_result);
	      p_basic_block->addEdge(p_dfg_vertex, p_dfg_vertex_target, p_dfg_edge);

	      set_output_gtns.insert(tn);
	    }
	  }
	}
      }
    }

    // Clearing the temporary variables.
    op_index_map.clear();


    // Deleting the dependency graph.
    CG_DEP_Delete_Graph (bb);


    // Returning the BasicBlock.
    return p_basic_block;

  };




  DfgVertex* Export_DfgVertex_From_OP(OP *op) {
    DfgVertex* p_dfg_vertex;
    Operation* p_operation = new Operation(OpCode(OP_code(op)));
    p_operation->setSourceFileLine(op ? Srcpos_To_Line(OP_srcpos(op)) : 0);
    p_operation->setDefaultName(TOP_Name(OP_code(op)));
    if (p_operation->getOpClass() == OPCLASS_CMP) {
      p_operation->setCmpVariant(CmpVariant(OP_cmp_variant(op)));
    }
    p_dfg_vertex = new DfgVertexOperation(*p_operation);
    return p_dfg_vertex;
  };




  Variable* Export_Variable_From_TN(TN *tn) {
    Variable* p_variable;
    ostringstream variable_name;
    if (TN_is_global_reg(tn)) {
      variable_name << "G";
    }
    variable_name << "TN" << TN_number(tn);
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	variable_name << "(" << REGISTER_name(TN_register_class(tn), TN_register(tn)) << ")";
      } else {
	variable_name << "(" << TN_register_class(tn) << "," << TN_register(tn) << ")";
      }
    }
    if (TN_is_save_reg(tn)) {
      variable_name << "(sv:" << REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)) << ")";
    }
    p_variable = gpCurrentFunction->getOrCreateVariable(TN_number(tn), variable_name.str(), TN_bitwidth(tn));
    return p_variable;
  };

#endif // BOOST_ENABLED


};
