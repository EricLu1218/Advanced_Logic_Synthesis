/*******************************************************************************
+
+  _vf_2_better.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  An implementation of VF2 improved in comparison to the original due to the
+  use of more sophisticated data structures
+
*******************************************************************************/
#ifndef VF2_BETTER_H
#define VF2_BETTER_H

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/core/list.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/b_prio.h>
#include <LEDA/core/impl/f_heap.h>
#include <LEDA/core/impl/p_heap.h>
#include <LEDA/core/impl/k_heap.h>
#include <LEDA/core/impl/m_heap.h>
#include <LEDA/core/impl/eb_tree.h>
#include <LEDA/graph/morphism/_ll_item.h>
#include <LEDA/graph/morphism/_vf2_base.h>

LEDA_BEGIN_NAMESPACE

#define PQUEUE
#define SEARCH_PQUEUE
#define PQ_KIND p_heap


template<class graph_t, class impl> class graph_morphism;

class __exportC vf2_better : public vf2_base<graph>
{
  friend class graph_morphism<graph, vf2_better>;

#if defined(__SUNPRO_CC)
  public:
#else
  protected:
#endif

  typedef graph_morphism_base<graph> gmb;
  typedef vf2_base<graph> base;
  typedef base::node node;
  typedef base::edge edge;
  typedef base::node_morphism node_morphism;
  typedef base::edge_morphism edge_morphism;
  typedef base::morphism morphism;
  typedef base::morphism_list morphism_list;
  typedef base::node_compat node_compat;
  typedef base::edge_compat edge_compat;
  typedef base::callback callback;
  typedef base::prep_graph prep_graph;
  typedef base::JOB JOB;

//private:
  typedef static_graph<opposite_graph, data_slots<0> >::node node_vf2_better_helper;

  struct node_info_vf2_better1 : data_slots<0>
  {
    node original;
    node_vf2_better_helper match;
    ll_item<void*> in, out;  //actually node_vf2_better1, 
                             //must avoid template recursion
    int indegree;

    void initialize()
    { match = nil;
      in.clear();
      out.clear();
     }
   };

  struct node_info_vf2_better2 : data_slots<0>
  { node original;
    node_vf2_better_helper match;
    void* in, * out;
    unsigned int in_depth, out_depth;
    int indegree;
    bool reached;

    void initialize()
    { match = nil;
      in_depth = out_depth = 0;
      reached = false;
     }
   };

  typedef static_graph<opposite_graph, node_info_vf2_better1> graph_vf2_better1;
  typedef graph_vf2_better1::node node_vf2_better1;
  typedef graph_vf2_better1::edge edge_vf2_better1;
  typedef two_tuple<graph const*, graph_vf2_better1*> prep_graph_vf2_better1_t;

  typedef static_graph<opposite_graph, node_info_vf2_better2> graph_vf2_better2;
  typedef graph_vf2_better2::node node_vf2_better2;
  typedef graph_vf2_better2::edge edge_vf2_better2;
  typedef two_tuple<graph const*, graph_vf2_better2*> prep_graph_vf2_better2_t;

  //"internal" data
  graph_vf2_better1* sg1;
  graph_vf2_better2* sg2;
  int t1in, t2in, t1out, t2out, t1both, t2both;
  ll_item<void*> t1in_nodes, t1out_nodes;

#ifdef PQUEUE
#define PQ p_queue<int, node_vf2_better2, PQ_KIND>
#define PQ_ITEM PQ::pq_item
#else
#define PQ b_priority_queue<node_vf2_better2>
#define PQ_ITEM b_pq_item
#endif

  PQ * t2in_nodes, * t2out_nodes;

  void construct_static_graph1(const graph& g, graph_vf2_better1& sg) const;
  void construct_static_graph2(const graph& g, graph_vf2_better2& sg) const;
  bool check_pair(node_vf2_better1 n1, node_vf2_better2 n2, unsigned int depth);
  bool match_iso(int depth);

protected:
  virtual bool call_iso();

  void input1(const graph& _g1);
  void input1(prep_graph pg1);
  void input2(const graph& _g2);
  void input2(prep_graph pg2);
  void clean_up();
  prep_graph prepare_graph(const graph& _g,const node_compat* _node_comp, 
                                           const edge_compat* _edge_comp) const;

  void delete_prepared_graph(prep_graph pg) const;
};

LEDA_END_NAMESPACE

#endif

