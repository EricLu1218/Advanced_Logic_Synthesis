/*******************************************************************************
+
+  LEDA 5.2  
+
+  graph_morphism_checker.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:13 $

#ifndef GRAPH_MORPHISM_CHECKER_H
#define GRAPH_MORPHISM_CHECKER_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520312
#include <LEDA/internal/PREAMBLE.h>
#endif


#include <LEDA/graph/graph_morphism_algorithm.h>

LEDA_BEGIN_NAMESPACE


template<class graph_t>
class graph_morphism_checker
{
protected:
   typedef graph_morphism_algorithm<graph_t> gma;
   typedef typename graph_t::node node;
   typedef typename graph_t::edge edge;
   typedef typename gma::node_morphism node_morphism;
   typedef typename gma::edge_morphism edge_morphism;
   typedef typename gma::node_compat node_compat;
   typedef typename gma::edge_compat edge_compat;

public:
   bool is_graph_isomorphism(const graph_t& g1, const graph_t& g2, 
                        node_morphism const* node_morph, 
                        edge_morphism const* edge_morph = NULL, 
                        const node_compat& node_comp = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP);

   bool is_subgraph_isomorphism(const graph_t& g1, const graph_t& g2, 
                        node_morphism const* node_morph, 
                        edge_morphism const* edge_morph = NULL, 
                        const node_compat& node_comp = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP);

   bool is_graph_monomorphism(const graph_t& g1, const graph_t& g2, 
                        node_morphism const* node_morph, 
                        edge_morphism const* edge_morph = NULL, 
                        const node_compat& node_comp = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP);
};


template<class graph_t>
bool graph_morphism_checker<graph_t>::is_graph_isomorphism(
                                              const graph_t& g1, 
                                              const graph_t& g2, 
                                              node_morphism const* node_morph, 
                                              edge_morphism const* edge_morph, 
                                              const node_compat& node_comp, 
                                              const edge_compat& edge_comp)
{
  node_array<bool, graph_t> g1n(g1,false);
  edge_array<bool, graph_t> g1e(g1,false);

  if(g1.number_of_nodes() != g2.number_of_nodes() || 
     g1.number_of_edges() != g2.number_of_edges())   return false;

  node n2;
  forall_nodes(n2, g2)
  { node n1 = (*node_morph)[n2]; //corresponding node

    if (g1n[n1])  return false; //used twice

    g1n[n1] = true;

    edge e2;
    forall_out_edges_graph_t(g2,e2,n2)
    { 
      node t1 = (*node_morph)[g2.opposite(e2, n2)];
      bool found = false;

      edge e1;
      forall_out_edges_graph_t(g1,e1,n1)
      { if(g1.opposite(e1, n1) == t1)
        { if (g1e[e1] || (edge_morph && (*edge_morph)[e2] != e1))   //used twice
             return false;
          g1e[e1] = true;
          found = true;
          break;
         }
       }
      if(!found) return false;
    }
  }
  return true;
}


template<class graph_t>
bool graph_morphism_checker<graph_t>::is_subgraph_isomorphism(
                                             const graph_t& g1, 
                                             const graph_t& g2, 
                                             node_morphism const* node_morph, 
                                             edge_morphism const* edge_morph, 
                                             const node_compat& node_comp, 
                                             const edge_compat& compat_edges)
{
  node_array<bool,graph_t> g1n(g1,false);
  edge_array<bool,graph_t> g2e(g2,false);

  if(g1.number_of_nodes() < g2.number_of_nodes() || 
     g1.number_of_edges() < g2.number_of_edges()) return false;

  node_morphism reverse(g1);

  node n2;
  forall_nodes(n2, g2)
      reverse[(*node_morph)[n2]] = n2;

  forall_nodes(n2, g2)
  { //for all matched nodes in g1, effectively

    node n1 = (*node_morph)[n2]; //corresponding node
    if (g1n[n1])  return false;   //double use

    g1n[n1] = true;

    edge e1;
    forall_out_edges_graph_t(g2, e1, n1)
    {
      if (reverse[g1.opposite(e1, n1)] == nil) //only matched adjacent nodes
        continue;

      node t2 = reverse[g1.opposite(e1, n1)];
      bool found = false;
      edge e2;
      forall_out_edges_graph_t(g1, e2, n2)
      {
        if(g1.opposite(e2, n2) == t2)
        {
          if (g2e[e2] || (edge_morph && (*edge_morph)[e2] != e1))  //double use
             return false;

          g2e[e2] = true;
          found = true;
          break;
         }
       }
      if(!found) return false;
     }
   }
  return true;
}



template<class graph_t>
bool graph_morphism_checker<graph_t>::is_graph_monomorphism(
                                               const graph_t& g1, 
                                               const graph_t& g2, 
                                               node_morphism const* node_morph, 
                                               edge_morphism const* edge_morph, 
                                               const node_compat& node_comp, 
                                               const edge_compat& edge_comp)
{
  node_array<bool,graph_t> g1n(g1,false);
  edge_array<bool,graph_t> g1e(g1,false);

  if(g1.number_of_nodes() < g2.number_of_nodes() || 
     g1.number_of_edges() < g2.number_of_edges())   return false;

  node n2;
  forall_nodes(n2, g2)
  {
    node n1 = (*node_morph)[n2]; //corresponding node

    if (g1n[n1])  return false;  //used twice

    g1n[n1] = true;

    edge e2;
    forall_out_edges_graph_t(g2, e2, n2)
    {
      node t1 = (*node_morph)[g2.opposite(e2, n2)];
      bool found = false;
      edge e1;
      forall_out_edges_graph_t(g1, e1, n1)
      {
        if(g1.opposite(e1, n1) == t1)
        {
          if (g1e[e1] || (edge_morph && (*edge_morph)[e2] != e1))  //used twice
             return false;

          g1e[e1] = true;
          found = true;
          break;
         }
       }
      if (!found) return false;
     }
   }
  return true;
}


LEDA_END_NAMESPACE


#if LEDA_ROOT_INCL_ID == 520312
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
