#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "graphml_dumper.h"
#include "mempool.h"

static MEM_POOL graphml_pool;

graphMLNode* new_graphML_node(int index) {
	graphMLNode *node = TYPE_MEM_POOL_ALLOC(graphMLNode, &graphml_pool);
	node->index_ = index;
	node->type_ = graphML_BB;
	node->label_ = NULL;
	node->is_folder_ = 0;
	node->sons_= NULL;
	node->parent_ = NULL;
	node->x_coord_ = 0;
	node->next_ = NULL;
	return node;
}

graphMLEdge* new_graphML_edge() {
	graphMLEdge *edge = TYPE_MEM_POOL_ALLOC(graphMLEdge, &graphml_pool);
	edge->type_= graphML_CONTROL_DEPENDENCE;
	edge->label_ = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 25);
	edge->parent_ = NULL;
	edge->source_ = NULL;
	edge->target_ = NULL;
	edge->next_ = NULL;
	return edge;
}

graphMLGraph* new_graphML_graph() {
	graphMLGraph *graph_ = TYPE_MEM_POOL_ALLOC(graphMLGraph, &graphml_pool);
	graph_->name_ = NULL;
	graph_->parent_ = NULL;
	graph_->nodes_ = NULL;
	graph_->edges_ = NULL;
	graph_->next_ = NULL;
	return graph_;
}

void add_node(graphMLGraph* g, graphMLNode* n) {
	if (!g->nodes_) {
		g->nodes_ = n;
	} else {
		graphMLNode* t = g->nodes_;
		while (t->next_) {
			t=t->next_;
		}
		t->next_=n;
	}
}

void add_edge(graphMLGraph* g, graphMLEdge* e) {
	if (!g->edges_)
		g->edges_ = e;
	else {
		graphMLEdge* t = g->edges_;
		while (t->next_ != NULL)
			t=t->next_;
		t->next_=e;
	}
}

void set_label(graphMLNode* n, char* label) {
	n->label_= TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, strlen(label)+2);
	//[TDR] Fix for bug #96132: When uggly strings are passed, strcopy may fail
	// due to undeterminated end of string 
	strncpy(n->label_, label, strlen(label)+1);
	n->label_[strlen(label)]='\0';
}

/******************* XML dumping function *********************/

/*
 * dump an instruction node
 */

void graphMLDumpInstNode(FILE* f, graphMLNode* n, int parent_index, char* color_code) {
	graphMLNode* n1=n;
	int size=1;
	fprintf(f, "\t\t<node id=\"n%d::n%d\">\n", parent_index, n->index_);
	fprintf(f, "\t\t<data key=\"d0\"><y:ShapeNode >\n");
	fprintf(f, "\t\t<y:Fill hasColor=\"true\" color=\"#%s\"  transparent=\"false\"/>\n",color_code);
	fprintf(f, "\t\t<y:BorderStyle hasColor=\"false\" type=\"line\" width=\"1.0\"  />\n");
	fprintf(f, "\t\t<y:NodeLabel visible=\"true\" alignment=\"left\" fontFamily=\"Dialog\" fontSize=\"8\" fontStyle=\"plain\" textColor=\"#000000\" hasBackgroundColor=\"false\" hasLineColor=\"false\" modelName=\"internal\" modelPosition=\"c\" autoSizePolicy=\"content\">");
	while (n1) {
		if (n1->label_ != NULL)
			fprintf(f, "%s\n", n1->label_);
		n1 = n1->next_;
		size++;
	}
	fprintf(f, "</y:NodeLabel>\n");
	fprintf(f, "\t\t<y:Geometry  x=\"0.0\" y=\"%d\" width=\"400.0\" height=\"%d.0\"/>\n", n->index_*60,8*size);
	fprintf(f, "\t\t<y:Shape type=\"rectangle\"/>\n\t\t</y:ShapeNode>\n");
	fprintf(f, "\t\t</data>\n\t\t</node>\n");
}

/*
 * dump a bb node
 */
void graphMLDumpFolderNode(FILE* f, graphMLNode* graphML_node) {
	char my_color_code[7];
	// dump a colored node for according to type of bb
	switch (graphML_node->type_) {
		case graphML_BB_CALL : 
			sprintf(my_color_code,"CC99FF");
			break;
		case graphML_BB_TAIL_CALL : 
			sprintf(my_color_code,"00FF00");
			break;
		case graphML_BB_EXIT : 
			sprintf(my_color_code,"FF0000");
			break;
		case graphML_BB :
		default : 
			sprintf(my_color_code,"FFFF00");
	}
	fprintf(f, "\t<node id=\"n%d\" yfiles.foldertype=\"folder\">\n", graphML_node->index_);
	fprintf(f, "\t\t<data key=\"d0\" >\n\t\t<y:ProxyAutoBoundsNode >\n\t\t<y:Realizers active=\"1\">\n");
	fprintf(f, "\t\t<y:GroupNode >\n");
	fprintf(f, "\t\t<y:Fill color=\"#%s\"  transparent=\"false\"/>\n\t\t<y:BorderStyle type=\"line\" width=\"1.0\" color=\"#000000\" />\n",my_color_code);
	fprintf(f, "\t\t<y:Shape type=\"rectangle\"/>\n");
	if (graphML_node->label_ != NULL)
		fprintf(f, "\t\t<y:NodeLabel visible=\"true\" alignment=\"right\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" textColor=\"#000000\" hasBackgroundColor=\"false\" modelName=\"sandwich\" modelPosition=\"n\" autoSizePolicy=\"node_width\">%s</y:NodeLabel>\n", graphML_node->label_);

	fprintf(f, "\t\t<y:DropShadow offsetX=\"4\" offsetY=\"4\" color=\"#D2D2D2\" />\n");
	fprintf(f, "\t\t<y:State closed=\"false\" innerGraphDisplayEnabled=\"false\"/>\n");
	fprintf(f, "\t\t</y:GroupNode>\n\t\t<y:GroupNode>\n");
	fprintf(f, "\t\t<y:Geometry  x=\"%d\" y=\"0\"/>\n", graphML_node->x_coord_);
	fprintf(f, "\t\t<y:BorderStyle type=\"line\" width=\"1.0\" color=\"#000000\" />\n");

	fprintf(f, "\t\t<y:Fill color=\"#%s\" color2=\"#%s\"  transparent=\"false\"/>\n",my_color_code,my_color_code);
	fprintf(f, "\t\t<y:Shape type=\"rectangle\"/>\n");

	if (graphML_node->label_ != NULL) {
		fprintf(f, "\t\t<y:NodeLabel alignment=\"center\" autoSizePolicy=\"node_width\" fontFamily=\"Dialog\" fontSize=\"16\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\">%s</y:NodeLabel>\n", graphML_node->label_);
	}
	fprintf(f, "\t\t<y:DropShadow offsetX=\"4\" offsetY=\"4\" color=\"#%s\" />\n",my_color_code);
	fprintf(f, "\t\t<y:State closed=\"true\" innerGraphDisplayEnabled=\"false\"/>\n");
	fprintf(f, "\t\t</y:GroupNode>\n\t\t</y:Realizers>\n\t\t</y:ProxyAutoBoundsNode>\n\t\t</data>\n");

	// add instructions	
	fprintf(f, "\t\t<graph id=\"n%d:\" edgedefault=\"directed\">\n",graphML_node->index_);

	// first son is the graph with instructions
	if (graphML_node->sons_) {
		graphMLNode* n1=graphML_node->sons_->nodes_;
		if(n1) graphMLDumpInstNode(f, n1, graphML_node->index_, my_color_code);
	} else {
		fprintf(f, "\t\t<node id=\"n%d::n0\">\n", graphML_node->index_);
		fprintf(f, "\t\t<data key=\"d0\"><y:ShapeNode >\n");
		fprintf(f, "\t\t<y:Fill color=\"#%s\"  transparent=\"false\"/>\n",my_color_code);
		fprintf(f, "\t\t<y:BorderStyle type=\"line\" width=\"1.0\" color=\"#000000\" />\n");
		fprintf(f, "\t\t<y:Shape type=\"rectangle\"/>\n\t\t</y:ShapeNode>\n");
		fprintf(f, "\t\t</data>\n\t\t</node>\n");
	}

	fprintf(f, "\t\t</graph>\n");

	// end of story
	fprintf(f, "\t</node>\n");

}

/*
 * effective dump stuff
 */
void graphMLDumpGraph(FILE* f, graphMLGraph* graphML_graph) {

	graphMLEdge* graphML_edge;
	graphMLNode* graphML_node;

	int counter=0;
	fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	fprintf(f, "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns/graphml\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns/graphml http://www.yworks.com/xml/schema/graphml/1.0/ygraphml.xsd\">");
	fprintf(f, "<key for=\"node\" id=\"d0\" yfiles.type=\"nodegraphics\"/>\n");
	fprintf(f, "<key for=\"edge\" id=\"d1\" yfiles.type=\"edgegraphics\"/>\n");
	fprintf(f, "<graph id=\"G\" edgedefault=\"directed\">\n");

	// dump root graph nodes
	graphML_node=graphML_graph->nodes_;
	while (graphML_node) {
		graphMLDumpFolderNode(f, graphML_node);
		graphML_node=graphML_node->next_;
	}

	// dump edges
	graphML_edge = graphML_graph->edges_;
	while (graphML_edge) {
		fprintf(f, "\t<edge id=\"e%d\" source=\"n%d\" target=\"n%d\">\n", counter, graphML_edge->source_->index_, graphML_edge->target_->index_);
		fprintf(f, "\t<data key=\"d1\">\n\t<y:PolyLineEdge>\n");
		fprintf(f, "\t<y:LineStyle type=\"line\" width=\"1.0\" color=\"#000000\"/>\n");
		fprintf(f, "\t<y:Arrows source=\"none\" target=\"standard\"/>\n");
		fprintf(f, "\t<y:EdgeLabel visible=\"true\" alignment=\"center\" modelName=\"two_pos\" modelPosition=\"head\" fontFamily=\"Dialog\" fontSize=\"11\" fontStyle=\"plain\" textColor=\"#000000\" hasBackgroundColor=\"false\" hasLineColor=\"false\">%s</y:EdgeLabel>\n", graphML_edge->label_);
		fprintf(f, "\t</y:PolyLineEdge>\n\t</data>\n\t</edge>\n");
		graphML_edge = graphML_edge->next_;
		counter++;
	}
	fprintf(f, "\t</graph>\n</graphml>\n");

}

char* get_xml_string(char* text) {
	char *newString;

	unsigned int counter=0, i, j;
	char* local, *ptr;
    const char *sep = "&<>\'\"";
	/*
	 * count the number of special characters
	 */
	local = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 2+strlen(text));
	strcpy(local, text);

	ptr = strtok(local, sep);
	while (ptr!=NULL) {
		counter++;
		ptr = strtok(NULL, sep);
	}

	if ((strchr(sep, text[strlen(text)-1]) == NULL) && (strchr(sep, text[0]) == NULL))
		counter--;

	/*********************/
	newString = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, strlen(text)+2+counter*7);
	newString[0] = '\0';

	for (i=0; i<strlen(text); i++) {
		switch (text[i]) {
		case '&':
			strcat(newString, "&amp;");
			break;

		case '<':
			strcat(newString, "&lt;");
			break;

		case '>':
			strcat(newString, "&gt;");
			break;

		case '\"':
			strcat(newString, "&quot;");
			break;

		case '\'':
			strcat(newString, "&apos;");
			break;

		default:

			j = strlen(newString) ;
			newString[j] = text[i];
			newString[j+1] = '\0';
		}
	}
	return newString;
}

typedef std::map<int, graphMLNode *> MapgraphMLNode;
static MapgraphMLNode saved_node;

/* ====================================================================
 *   sPrint_TN
 *
 *   Format a TN to a string.  It uses the 'buf' passed in as the string.
 *   Returns a pointer to buf.
 *
 * ====================================================================
 */
static char *
Print_TN ( 
  TN *tn, 
  char *buf 
)
{
  char *result = buf;

  if (tn == NULL) {
    buf += sprintf ( buf, "--nil--");
    return result;
  }

  if (TN_is_constant(tn)) {
    if ( TN_has_value(tn)) {
      buf += sprintf ( buf, "(0x%llx)", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	buf += sprintf ( buf, "!!! TN_value=0x%llx is too big to fit in a word",
			 TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      buf += sprintf ( buf, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      if ( offset == 0 ) {
	buf += sprintf ( buf, "(lab:%s)", name );
      }
      else {
	buf += sprintf ( buf, "(lab:%s+%lld)", name, offset );
      }
    } 
    else if ( TN_is_tag(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      buf += sprintf ( buf, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);

      buf += sprintf ( buf, "(sym" );
      // [JV] On STxP70, some relocs name starts with '%' ...
      buf += sprintf ( buf, "%s", TN_RELOCS_Name(TN_relocs(tn)) );

      if (ST_class(var) == CLASS_CONST) {
      	buf += sprintf ( buf, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      }
      else {
      	buf += sprintf ( buf, ":%s%+lld)", ST_name(var), TN_offset(tn) );
      }
    } 
    else {
      ErrMsg (EC_Unimplemented, "sPrint_TN: illegal constant TN");
    }
  }
  else {  /* register TN */
    if (TN_is_global_reg(tn)) {
      buf += sprintf ( buf, "GTN%d", TN_number(tn) );
    }
    else {
      buf += sprintf ( buf, "TN%d", TN_number(tn) );
    }
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	buf += sprintf (buf, "(%s)", 
		REGISTER_name(TN_register_class(tn), TN_register(tn)));
      } else {
	buf += sprintf (buf, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    if (TN_is_save_reg(tn)) {
	buf += sprintf (buf, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
  if (tn && Get_Trace(TP_MISC, 8))
    buf += sprintf(buf, ":%d", TN_size(tn));
  
  return result;
}


graphMLGraph* graphML_BuildFlowGraph(BB *bb) {
	graphMLGraph* graphML_graph;
	char block_name[50];
	graphMLNode *source, *target;
	graphMLEdge* graphML_edge;
	graphMLNode* graphML_node;
	int index, index1;
	char *str1; // used to dump instructions operands
	char buf[1024];
	saved_node.clear();

	BB* cur_bb;
	graphMLNode* inst_node;
	graphMLGraph* inst_graph,*tmp_graph;
	OP *op;
	int i;
	
	str1 = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 512);
	graphML_graph = new_graphML_graph();
	char *tmp1;
	/* nodes */

	graphML_node = new_graphML_node(0);
	graphML_node->is_folder_ = 0;
	saved_node[0] = graphML_node;
	set_label(graphML_node, "EXIT BLOCK");
 	graphML_node->type_ = graphML_BB_EXIT;
	inst_graph = new_graphML_graph();
	inst_graph->parent_ = graphML_node;
	add_node(graphML_graph, graphML_node);

	index = 1;
	
	for (cur_bb = bb; cur_bb; cur_bb = BB_next(cur_bb), index++) {
		
		graphML_node = new_graphML_node(index);
		graphML_node->is_folder_ = 1;

		saved_node[BB_id(cur_bb)] = graphML_node;
		switch(BB_kind(cur_bb)) {
		case  BBKIND_RETURN:
			sprintf(block_name, "BB%d [exit]", BB_id(cur_bb));
			break;
		case  BBKIND_CALL:
			sprintf(block_name, "BB%d [call]", BB_id(cur_bb));
			break;
		case  BBKIND_TAIL_CALL:
			sprintf(block_name, "BB%d [tailcall]", BB_id(cur_bb));
			break;
		case  BBKIND_REGION_EXIT:
		case  BBKIND_GOTO:
		case  BBKIND_LOGIF:
		case  BBKIND_VARGOTO:
		case  BBKIND_INDGOTO:
		case  BBKIND_UNKNOWN:
		default:
			sprintf(block_name, "BB%d ", BB_id(cur_bb));
		}
		set_label(graphML_node, block_name);
		inst_graph = new_graphML_graph();
		inst_graph->parent_ = graphML_node;
		index1=0;

		for (op=BB_first_op(cur_bb); op; op=OP_next(op), index1++) {
			inst_node = new_graphML_node(index1);
			// instruction source ref 
			sprintf(str1, "[%4d] ", Srcpos_To_Line(OP_srcpos(op)));
			strcat(str1, " ");

			for (i = 0; i < OP_results(op); i++) {
				strcat(str1,Print_TN(OP_result(op,i),buf));
				strcat(str1, " ");
			}

			strcat(str1, ":- ");
			strcat(str1, TOP_Name(OP_code(op)));
			strcat(str1, " ");
			if ( OP_variant(op) != 0) {
				tmp1 = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 128);
				sprintf(tmp1, "(%x) ", OP_variant(op));
				strcat(str1, tmp1);
			}
			if (OP_code(op) == TOP_psi) {
				for (i=0; i<PSI_opnds(op); i++) {
					TN *guard = PSI_guard(op, i);
					if (guard) {
						// (cbr) Support for guards on false
						if (PSI_Pred_False(op, i))
							strcat(str1, "!");
						strcat(str1,Print_TN(guard, buf));
					}
					strcat(str1, "?");
					TN *tn = PSI_opnd(op, i);
					strcat(str1,Print_TN(tn, buf));
					if (OP_Defs_TN(op, tn)) 	
						strcat(str1, "<defopnd>");
					strcat(str1, " ");
				}
			} else {
				for (i=0; i<OP_opnds(op); i++) {
					TN *tn= OP_opnd(op,i);
					if (OP_code(op) == TOP_phi) {
						BB *pred_bb = Get_PHI_Predecessor(op, i);
						if (pred_bb) {
							tmp1 = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 128);
							sprintf(tmp1, "BB%d?", BB_id(pred_bb));
							strcat(str1, tmp1);
						}
					}
					if (OP_Pred_False(op, i)) strcat(str1, "!");
					strcat(str1,Print_TN(tn, buf));
					if (OP_Defs_TN(op, tn))
						strcat(str1, "<defopnd>");
					strcat(str1, " ");
				}
			}
			strcat(str1, ";");
			/* print flags */
			if (OP_glue(op))
				strcat(str1, " glue");
			if (OP_no_alias(op))
				strcat(str1, " noalias");
			if (OP_copy(op))
				strcat(str1, " copy");
			if (OP_volatile(op))
				strcat(str1, " volatile");
			if (OP_side_effects(op))
				strcat(str1, " side_effects");
			if (OP_hoisted(op))
				strcat(str1, " hoisted");
			if (OP_cond_def(op))
				strcat(str1, " cond_def");
			if (OP_end_group(op))
				strcat(str1, " end_group");
			if (OP_tail_call(op))
				strcat(str1, " tail_call");
			if (OP_no_move_before_gra(op))
				strcat(str1, " no_move");
			if (OP_spill(op))
				strcat(str1, " spill");
			if (OP_ssa_move(op))
				strcat(str1, " ssa_move");
			if (OP_black_hole(op))
				strcat(str1, " black_hole");
			if (OP_Get_Flag_Effects(op) & OP_FE_WRITE)
				strcat(str1, " flag_write");
			if (OP_Get_Flag_Effects(op) & OP_FE_READ)
				strcat(str1, " flag_read");
			if (OP_prologue(op))
				strcat(str1, " prologue");
			if (OP_epilogue(op))
				strcat(str1, " epilogue");
			if (OP_Get_Affirm(op))
				strcat(str1, " affirm");
			if (OP_unrolling(op)) {
				UINT16 unr= OP_unrolling(op);
				tmp1 = TYPE_MEM_POOL_ALLOC_N(char, &graphml_pool, 128);
				sprintf(tmp1, " %d%s unrolling", unr, unr == 1 ? "st"
						: unr == 2 ? "nd" : unr == 3 ? "rd" : "th");
				strcat(str1, tmp1);
			}

			// transform string
			set_label(inst_node, get_xml_string(str1));
			add_node(inst_graph, inst_node);

		}
		if (graphML_node->sons_ == NULL) {
			graphML_node->sons_ = inst_graph;
		} else {
			tmp_graph = graphML_node->sons_;
			while (tmp_graph->next_) {
				tmp_graph=tmp_graph->next_;
			}
			tmp_graph->next_=inst_graph;
		}
		add_node(graphML_graph, graphML_node);
	}

	/* edges */
	for (cur_bb = bb; cur_bb; cur_bb = BB_next(cur_bb)) {
		source = saved_node[BB_id(cur_bb)];
		BBLIST *bl;
	
		switch(BB_kind(cur_bb)) {
		case  BBKIND_GOTO:
	      if (BB_succs(cur_bb)) {
			target = saved_node[BB_id(BBLIST_item(BB_succs(cur_bb)))];
			graphML_edge = new_graphML_edge();
			strcpy(graphML_edge->label_," ");
			graphML_edge->target_ = target;
			graphML_edge->source_ = source;
			add_edge(graphML_graph, graphML_edge);
	      }
	      break;
		case  BBKIND_LOGIF:
			{
			BB *targ_bb;
			BBLIST *target_edge;
			BBLIST *fall_through_edge;
			OP *br = BB_branch_op(bb);
			/* Get the targets. Note that target[0] is always the "true" target.
			 */
			target_edge = BB_succs(cur_bb);
			fall_through_edge = BBLIST_next(target_edge);
			targ_bb = BBLIST_item(target_edge);
			if (fall_through_edge == NULL) {
			  fall_through_edge = target_edge;
			} else if (targ_bb == BB_next(cur_bb)) {
			  target_edge = fall_through_edge;
			  fall_through_edge = BB_succs(cur_bb);
			  targ_bb = BBLIST_item(target_edge);
			}
			target = saved_node[BB_id(targ_bb)];
			graphML_edge = new_graphML_edge();
			strcpy(graphML_edge->label_,"TRUE");
			graphML_edge->target_ = target;
			graphML_edge->source_ = source;
			add_edge(graphML_graph, graphML_edge);
			target = saved_node[BB_id(targ_bb)];
			graphML_edge = new_graphML_edge();
			strcpy(graphML_edge->label_,"FALSE");
			graphML_edge->target_ = saved_node[BB_id(BBLIST_item(fall_through_edge))];
			graphML_edge->source_ = source;
			add_edge(graphML_graph, graphML_edge);
			}
			break;
		case  BBKIND_RETURN:
			source->type_ = graphML_BB_EXIT;
			graphML_edge = new_graphML_edge();
			strcpy(graphML_edge->label_,"EXIT");
			graphML_edge->source_ = source;
			graphML_edge->target_ = saved_node[0];
			add_edge(graphML_graph, graphML_edge);
			if (BB_succs(cur_bb)) {
				graphML_edge = new_graphML_edge();
				target = saved_node[BB_id(BBLIST_item(BB_succs(cur_bb)))];
				strcpy(graphML_edge->label_," ");
				graphML_edge->target_ = target;
				graphML_edge->source_ = source;
				add_edge(graphML_graph, graphML_edge);
			}
			break;
		case  BBKIND_CALL:
			source->type_ = graphML_BB_CALL;
			target = saved_node[BB_id(BBLIST_item(BB_succs(cur_bb)))];
			graphML_edge = new_graphML_edge();
			strcpy(graphML_edge->label_," ");
			graphML_edge->target_ = target;
			graphML_edge->source_ = source;
			add_edge(graphML_graph, graphML_edge);
			break;
		case  BBKIND_TAIL_CALL:
			source->type_ = graphML_BB_TAIL_CALL;
			break;
		case  BBKIND_REGION_EXIT:
		case  BBKIND_VARGOTO:
		case  BBKIND_INDGOTO:
		case  BBKIND_UNKNOWN:
		default:
			FOR_ALL_BB_SUCCS (cur_bb, bl) {
				target = saved_node[BB_id(BBLIST_item(bl))];
				if(target==NULL||source==NULL) *((int*)0)=0;
				graphML_edge = new_graphML_edge();
				strcpy(graphML_edge->label_,"UNKNOWN");
				graphML_edge->target_ = target;
				graphML_edge->source_ = source;
				add_edge(graphML_graph, graphML_edge);
			}
		}
	}
	return graphML_graph;
}

/****************************************************************/
/* Purpose..: Translates the CFG into a graphML representation		*/
/* Input....: ---												*/
/* Returns..: ---												*/
/****************************************************************/

static char *pu_info_name=NULL;
static PU_Info* pu_info=NULL;
static int pu_count=1000;
static int phase_count=100;

void graphML_DumpCFG(BB* bb, const char* suffix) {
	FILE *fp;
	graphMLGraph* graphML_graph= NULL;
	char filename[500];
	char *tmp_suffix;
	char *tmp_counter;
	MEM_POOL_Initialize(&graphml_pool, "graphml temporary memory", FALSE);
	MEM_POOL_Push(&graphml_pool);
	if (!pu_info_name || pu_info != Current_PU_Info) {
		pu_info_name = ST_name(PU_Info_proc_sym(Current_PU_Info));
		pu_info = Current_PU_Info;
		pu_count++;
		phase_count=100;
	}
	phase_count ++;	
	sprintf(filename, "%d_%d_%s",pu_count,phase_count,pu_info_name);
	if (suffix) {
		strcat(filename, "_");
		tmp_suffix=strdup(suffix);
		tmp_counter=tmp_suffix;
		while(*tmp_counter != '\0') {
			if (! isalnum(*tmp_counter)) *tmp_counter='_';
			tmp_counter++;
		}
		strcat(filename, tmp_suffix);
		free(tmp_suffix);
	}
	strcat(filename, ".graphml");
	fp = fopen(filename, "w");
	if (fp==NULL) {
		printf("graphMLDumpCFG: couldn't open file\n");
		return;
	}
	graphML_graph = graphML_BuildFlowGraph(bb);
	graphMLDumpGraph(fp, graphML_graph);
	fclose(fp);
	
	// Cleanup
	MEM_POOL_Pop(&graphml_pool);
	MEM_POOL_Delete(&graphml_pool);
}

