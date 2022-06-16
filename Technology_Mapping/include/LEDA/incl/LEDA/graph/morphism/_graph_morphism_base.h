/*******************************************************************************
+
+  graph_morphism_base.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  Base class for the graph morphism implementations, holds common code
+
*******************************************************************************/


#ifndef GRAPH_MORPHISM_BASE_H
#define GRAPH_MORPHISM_BASE_H

#include <LEDA/core/list.h>
#include <LEDA/core/tuple.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/morphism/_callback_base.h>
#include <LEDA/graph/morphism/_node_comparator.h>
#include <LEDA/graph/morphism/_forall.h>

#include <LEDA/graph/graph_morphism_algorithm.h>


LEDA_BEGIN_NAMESPACE

template<class graph_t>
class graph_morphism_base
{
protected:

  virtual ~graph_morphism_base() {};
   

  typedef graph_morphism_algorithm<graph_t> gma;
  typedef typename gma::cardinality_t cardinality_t;
  typedef typename gma::node node;
  typedef typename gma::edge edge;
  typedef typename gma::node_morphism node_morphism;
  typedef typename gma::edge_morphism edge_morphism;
  typedef typename gma::morphism morphism;
  typedef typename gma::morphism_list morphism_list;
  typedef typename gma::node_compat node_compat;
  typedef typename gma::edge_compat edge_compat;
  typedef typename gma::callback callback;
  typedef typename gma::prep_graph prep_graph;

  enum JOB {FINDFIRST, CARDINALITY, FINDALL, ENUM};

  graph_t const* g1, * g2;
        
  node_morphism* node_morph;
  edge_morphism* edge_morph;

  morphism_list* morphisms;

  const node_compat* node_comp;
  const edge_compat* edge_comp;

  callback* callb;


  //"internal" data
  cardinality_t num_mappings, num_calls;
  bool constructed1, constructed2;
  JOB job;

  //dummy functions in case an algorithm does not support all variants
  virtual bool call_iso();
  virtual bool call_sub();
  virtual bool call_mono();

  //reconstruct edge mapping from node mapping
  void map_edges(const graph_t& g1, const graph_t& g2, 
                 const node_morphism& node_morph, edge_morphism& edge_morph);

  bool go_iso();
  bool go_find_iso(node_morphism* _node_morph, edge_morphism* _edge_morph, 
                   const node_compat* _node_comp, const edge_compat* _edge_comp);

  cardinality_t go_cardinality_iso(const node_compat* _node_comp,  
                                   const edge_compat* _edge_comp);

  cardinality_t go_find_all_iso(list<morphism*>& _isomorphisms, 
                                const node_compat* _node_comp, 
                                const edge_compat* _edge_comp);

  cardinality_t go_enumerate_iso(leda_callback_base<morphism>& _callb, 
                                 const node_compat* _node_comp, 
                                 const edge_compat* _edge_comp);

   bool go_sub();
   bool go_find_sub(node_morphism* _node_morph, edge_morphism* _edge_morph, 
                                                const node_compat* _node_comp, 
                                                const edge_compat* _edge_comp);

   cardinality_t go_cardinality_sub(const node_compat* _node_comp, 
                                    const edge_compat* _edge_comp);

   cardinality_t go_find_all_sub(list<morphism*>& _submorphisms, 
                                 const node_compat* _node_comp, 
                                 const edge_compat* _edge_comp);

   cardinality_t go_enumerate_sub(leda_callback_base<morphism>& _callb, 
                                  const node_compat* _node_comp, 
                                  const edge_compat* _edge_comp);

   bool go_mono();
   bool go_find_mono(node_morphism* _node_morph, edge_morphism* _edge_morph, 
                                                 const node_compat* _node_comp,
                                                 const edge_compat* _edge_comp);

   cardinality_t go_cardinality_mono(const node_compat* _node_comp, 
                                     const edge_compat* _edge_comp);

   cardinality_t go_find_all_mono(list<morphism*>& _monomorphisms, 
                                  const node_compat* _node_comp, 
                                  const edge_compat* _edge_comp);

   cardinality_t go_enumerate_mono(leda_callback_base<morphism>& _callb, 
                                   const node_compat* _node_comp, 
                                   const edge_compat* _edge_comp);
};



template<class graph_t>
bool graph_morphism_base<graph_t>::call_iso()
{
  leda_error_handler(1, "Algorithm does not support graph isomorphism.");
  return false;
}



template<class graph_t>
bool graph_morphism_base<graph_t>::call_sub()
{
  leda_error_handler(1, "Algorithm does not support subgraph isomorphism.");
  return false;
}


template<class graph_t>
bool graph_morphism_base<graph_t>::call_mono()
{
  leda_error_handler(1, "Algorithm does not support graph monomorphism.");
  return false;
}


template<class graph_t>
bool graph_morphism_base<graph_t>::go_iso()
{
  if (g1->number_of_nodes() != g2->number_of_nodes() || 
      g1->number_of_edges() != g2->number_of_edges()) return false;

  return call_iso();      //call gma class
}


template<class graph_t>
bool graph_morphism_base<graph_t>::go_find_iso(node_morphism* _node_morph, 
                                               edge_morphism* _edge_morph, 
                                               const node_compat* _node_comp, 
                                               const edge_compat* _edge_comp)
{ job = FINDFIRST;
  num_mappings = 0;
  node_morph = _node_morph;
  edge_morph = _edge_morph;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  return go_iso();
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_cardinality_iso(const node_compat* _node_comp,
                                                 const edge_compat* _edge_comp)
{ job = CARDINALITY;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_iso();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_find_all_iso(list<morphism*>& _isomorphisms, 
                                              const node_compat* _node_comp, 
                                              const edge_compat* _edge_comp)
{ job = FINDALL;
  morphisms = &_isomorphisms;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_iso();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_enumerate_iso(
                                         leda_callback_base<morphism>& _callb, 
                                         const node_compat* _node_comp, 
                                         const edge_compat* _edge_comp)
{ job = ENUM;
  num_mappings = 0;
  callb = &_callb;
  node_morph = new node_morphism(*g2);
  edge_morph = new edge_morphism(*g2);
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_iso();
  delete node_morph;
  delete edge_morph;
  return num_mappings;
}



template<class graph_t>
bool graph_morphism_base<graph_t>::go_sub()
{
  if(g1->number_of_nodes() < g2->number_of_nodes() || 
     g1->number_of_edges() < g2->number_of_edges()) return false;

  return call_sub();      //call gma class
}


template<class graph_t>
bool graph_morphism_base<graph_t>::go_find_sub(node_morphism* _node_morph, 
                                               edge_morphism* _edge_morph, 
                                               const node_compat* _node_comp, 
                                               const edge_compat* _edge_comp)
{ job = FINDFIRST;
  num_mappings = 0;
  node_morph = _node_morph;
  edge_morph = _edge_morph;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  return go_sub();
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_cardinality_sub(const node_compat* _node_comp,
                                                 const edge_compat* _edge_comp)
{ job = CARDINALITY;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_sub();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_find_all_sub(list<morphism*>& _isomorphisms, 
                                              const node_compat* _node_comp, 
                                              const edge_compat* _edge_comp)
{ job = FINDALL;
  morphisms = &_isomorphisms;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_sub();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_enumerate_sub(
                                          leda_callback_base<morphism>& _callb, 
                                          const node_compat* _node_comp, 
                                          const edge_compat* _edge_comp)
{ job = ENUM;
   num_mappings = 0;
   callb = &_callb;
   node_morph = new node_morphism(*g2);
   edge_morph = new edge_morphism(*g2);
   node_comp = _node_comp;
   edge_comp = _edge_comp;
   go_sub();
   delete node_morph;
   delete edge_morph;
   return num_mappings;
}



template<class graph_t>
bool graph_morphism_base<graph_t>::go_mono()
{ if(g1->number_of_nodes() < g2->number_of_nodes() || 
     g1->number_of_edges() < g2->number_of_edges()) return false;

   return call_mono();   //call gma class
}


template<class graph_t>
bool graph_morphism_base<graph_t>::go_find_mono(node_morphism* _node_morph, 
                                                edge_morphism* _edge_morph, 
                                                const node_compat* _node_comp, 
                                                const edge_compat* _edge_comp)
{ job = FINDFIRST;
  num_mappings = 0;
  node_morph = _node_morph;
  edge_morph = _edge_morph;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  return go_mono();
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_cardinality_mono(const node_compat* _node_comp,
                                                  const edge_compat* _edge_comp)
{ job = CARDINALITY;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_mono();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_find_all_mono(list<morphism*>& _isomorphisms, 
                                               const node_compat* _node_comp, 
                                               const edge_compat* _edge_comp)
{ job = FINDALL;
  morphisms = &_isomorphisms;
  num_mappings = 0;
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_mono();
  return num_mappings;
}


template<class graph_t>
typename graph_morphism_base<graph_t>::cardinality_t 
graph_morphism_base<graph_t>::go_enumerate_mono(
                                           leda_callback_base<morphism>& _callb,
                                           const node_compat* _node_comp, 
                                           const edge_compat* _edge_comp)
{ job = ENUM;
  num_mappings = 0;
  callb = &_callb;
  node_morph = new node_morphism(*g2);
  edge_morph = new edge_morphism(*g2);
  node_comp = _node_comp;
  edge_comp = _edge_comp;
  go_mono();
  delete node_morph;
  delete edge_morph;
  return num_mappings;
}


template<class graph_t>
void graph_morphism_base<graph_t>::map_edges(const graph_t& _g1, 
                                             const graph_t& _g2, 
                                             const node_morphism& _node_morph, 
                                             edge_morphism& _edge_morph)
{ node n2;
  forall_nodes(n2, _g2)
  { node n1 = _node_morph[n2];    //corresponding node

    //typename graph_t::node next_node = _g2.next_node(n2); 
    edge e2;
    forall_out_edges_graph_t(_g2, e2, n2)
    { node t1 = _node_morph[_g2.opposite(e2, n2)];

      edge e1;
      forall_out_edges_graph_t(_g1, e1, n1)
        if(_g1.opposite(e1, n1) == t1) _edge_morph[e2] = e1;
     }
   }
}


LEDA_END_NAMESPACE

#endif


