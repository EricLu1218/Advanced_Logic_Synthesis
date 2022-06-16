/*******************************************************************************
+
+  _vf_2_best.h
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
#ifndef VF2_BEST_H
#define VF2_BEST_H


#include <LEDA/system/basic.h>
#include <LEDA/graph/morphism/_ll_item.h>
#include <LEDA/graph/morphism/_vf2_base.h>
#include <LEDA/graph/morphism/_node_comparator.h>

LEDA_BEGIN_NAMESPACE


template<class graph_t, class impl> class graph_morphism;

class __exportC vf2_best : public vf2_base<graph>
{
  friend class graph_morphism<graph, vf2_best>;

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

private:
  struct node_info_vf2_best : data_slots<0>
  {
    node original;
    void* match;
    ll_item<void*> in, out;  // actually node_vf2_best, 
                            // must avoid template recursion
    int indegree;
    bool reached;

    void initialize()
    { match = nil;
      in.clear();
      out.clear();
      reached = false;
     }
   };

  typedef static_graph<opposite_graph, node_info_vf2_best> graph_vf2_best;
  typedef graph_vf2_best::node node_vf2_best;
  typedef graph_vf2_best::edge edge_vf2_best;
  typedef two_tuple<graph const*, graph_vf2_best*> prep_graph_vf2_best_t;

  //"internal" data
  graph_vf2_best* sg1, * sg2;
  int t1in, t2in, t1out, t2out, t1both, t2both;

  ll_item<void*> t1in_nodes, t2in_nodes, t1out_nodes, t2out_nodes;

  void construct_static_graph(const graph& g, graph_vf2_best& sg) const;
  bool check_pair(node_vf2_best n1, node_vf2_best n2, unsigned int depth);
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
