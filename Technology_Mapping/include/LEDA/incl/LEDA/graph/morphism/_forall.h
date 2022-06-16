/*******************************************************************************
+
+  _forall.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  Work-around to make macros like forall_out_edges work for regular and
+  static graphs at the same time.
+
*******************************************************************************/

#ifndef FORALL_H
#define FORALL_H


#define forall_adj_edges_nonstatic(e,v)\
LEDA_FORALL_PREAMBLE \
for(leda_edge loop_var=First_Adj_Edge(v,0);e=loop_var,\
loop_var=Succ_Adj_Edge(loop_var,v), e;)

#define forall_out_edges_graph_t(g,e,n) \
typename graph_t::node next_node = g.next_node(n); \
typename graph_t::edge next_edge; \
if(next_node != g.stop_node()) \
        next_edge = g.first_out_edge(next_node); \
else \
        next_edge = g.stop_edge(); \
for(e = g.first_out_edge(n); e != g.stop_edge() && e != next_edge; e = g.out_succ(e))

#define forall_in_edges_graph_t(g,e,n) \
for(e = g.first_in_edge(n); e != nil; e = g.in_succ(e))


#endif
