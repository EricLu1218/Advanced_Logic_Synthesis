/*******************************************************************************
+
+  _vf_2_simple.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  An implementation of VF2 quite close to the original, but using the LEDA
+  static graph data structure
+ 
*******************************************************************************/
#ifndef VF2_SIMPLE_H
#define VF2_SIMPLE_H

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/morphism/_vf2_base.h>
#include <LEDA/graph/morphism/_forall.h>

#define SORT_NODES

LEDA_BEGIN_NAMESPACE

//declaration

template<class graph_t, class impl> class graph_morphism;

template<class graph_t = graph>
class vf2_simple : public vf2_base<graph_t>
{
  friend class graph_morphism<graph_t, vf2_simple<graph_t> >;

protected:
  typedef graph_morphism_base<graph_t> gmb;
  typedef vf2_base<graph_t> base;
  typedef typename base::node node;
  typedef typename base::edge edge;
  typedef typename base::node_morphism node_morphism;
  typedef typename base::edge_morphism edge_morphism;
  typedef typename base::morphism morphism;
  typedef typename base::morphism_list morphism_list;
  typedef typename base::node_compat node_compat;
  typedef typename base::edge_compat edge_compat;
  typedef typename base::callback callback;
  typedef typename base::prep_graph prep_graph;
  typedef typename base::JOB JOB;

private:
  struct node_info_vf2_simple : data_slots<0>
  { typename graph_t::node original;
    void* match;
    unsigned short in, out;
    unsigned int indegree;

    void initialize()
    { match = nil;
      in = 0;
      out = 0;
     }
   };

  struct edge_info_vf2_simple : data_slots<0>
  { typename graph_t::edge original;
    void* match;
   };

  typedef static_graph<opposite_graph, node_info_vf2_simple, edge_info_vf2_simple> graph_vf2_simple;
  typedef typename graph_vf2_simple::node node_vf2_simple;
  typedef typename graph_vf2_simple::edge edge_vf2_simple;
  typedef two_tuple<graph_t const*, graph_vf2_simple*> prep_graph_vf2_simple_t;

  //"internal" data
  graph_vf2_simple* sg1, * sg2;
  unsigned int t1in, t2in, t1out, t2out, t1both, t2both;

  void construct_static_graph(const graph_t& g, graph_vf2_simple& sg) const;
  bool check_pair(node_vf2_simple n1, node_vf2_simple n2, unsigned int depth);
  bool match_iso(int depth);

protected:
  virtual bool call_iso();

  void input1(const graph_t& _g1);
  void input1(prep_graph pg1);
  void input2(const graph_t& _g2);
  void input2(prep_graph pg2);
  void clean_up();
  typename graph_morphism_algorithm<graph_t>::prep_graph 
  prepare_graph(const graph_t& g, const node_compat* _node_comp, 
                                  const edge_compat* _edge_comp) const;
  void delete_prepared_graph(prep_graph pg) const;
};


//definitions

#define SORT_NODES
//unfortunately, these casts are necessary to avoid template recursion
#define MATCHN_SIMPLE(n) ((node_vf2_simple)(n->match))
#define MATCHE_SIMPLE(n) ((edge_vf2_simple)(n->match))

template<class graph_t>
void vf2_simple<graph_t>::input1(const graph_t& _g1)
{ //convert to static graphs for best performance
  sg1 = new graph_vf2_simple();
  gmb::g1 = &_g1;
  construct_static_graph(*gmb::g1, *sg1);
  gmb::constructed1 = true;
 }


template<class graph_t>
void vf2_simple<graph_t>::input1(prep_graph _pg1)
{ prep_graph_vf2_simple_t* pg1 = (prep_graph_vf2_simple_t*)_pg1;
  gmb::g1 = pg1->first();
  sg1 = pg1->second();
  gmb::constructed1 = false;
 }


template<class graph_t>
void vf2_simple<graph_t>::input2(const graph_t& _g2)
{ //convert to static graphs for best performance
  sg2 = new graph_vf2_simple();
  gmb::g2 = &_g2;
  construct_static_graph(*gmb::g2, *sg2);
  gmb::constructed2 = true;
 }


template<class graph_t>
void vf2_simple<graph_t>::input2(prep_graph _pg2)
{ prep_graph_vf2_simple_t* pg2 = (prep_graph_vf2_simple_t*)_pg2;
  gmb::g2 = pg2->first();
  sg2 = pg2->second();
  gmb::constructed2 = false;
 }


template<class graph_t>
void vf2_simple<graph_t>::clean_up()
{ if(gmb::constructed1) delete sg1;
  if(gmb::constructed2) delete sg2;
}


template<class graph_t>
typename graph_morphism_algorithm<graph_t>::prep_graph vf2_simple<graph_t>::prepare_graph(const graph_t& g, const node_compat* _node_comp, const edge_compat* _edge_comp) const
{ prep_graph_vf2_simple_t* pg = new prep_graph_vf2_simple_t;
  pg->first() = &g;
  pg->second() = new graph_vf2_simple();
  construct_static_graph(g, *pg->second());
  return (prep_graph)pg;
}

template<class graph_t>
void vf2_simple<graph_t>::delete_prepared_graph(prep_graph _pg) const
{ prep_graph_vf2_simple_t* pg = (prep_graph_vf2_simple_t*)_pg;
  delete pg->second();
  delete pg;
}

template<class graph_t>
void vf2_simple<graph_t>::construct_static_graph(const graph_t& g, graph_vf2_simple& sg) const
{ sg.start_construction(g.number_of_nodes(), g.number_of_edges());
  node_comparator<graph_t> nc(&g);

  //for sorting, copy node references to array temporarily
  array<node> nodes(g.number_of_nodes());

  int i = 0;
  node n;
  forall_nodes(n, g) nodes[i++] = n;
#ifdef SORT_NODES
  nodes.sort(nc);
#endif

  //finally, construct all the nodes and edges
  node_array<node_vf2_simple, graph_t> mapping(g);
  forall(n, nodes)
  { node_vf2_simple sn = sg.new_node();
    sn->original = n;
    sn->indegree = g.indeg(n);
    mapping[n] = sn;
   }

  forall(n, nodes)
  { node_vf2_simple sn = mapping[n];
    edge e;
    forall_out_edges_graph_t(g,e,n)
    { node o = g.opposite(e, n);
      edge_vf2_simple se = sg.new_edge(sn, mapping[o]);
      se->original = e;
     }
   }
  sg.finish_construction();
}





template<class graph_t>
bool vf2_simple<graph_t>::check_pair(node_vf2_simple n1, node_vf2_simple n2, 
                                                         unsigned int depth)
{
  if (n1->indegree != n2->indegree || sg1->outdeg(n1) != sg2->outdeg(n2))
     return false;

  edge_vf2_simple e1, e2;
  node_vf2_simple t1, t2;
                
//                              e1
//      sg1             n1      ->      t1
//                      |               |       match
//                      V               V
//      sg2             n2      ->      t2
//                              e2

  bool found;

  //check out-edges
  forall_out_edges(e1, n1)
  { t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    { found = false;
      forall_out_edges(e2, n2)
        if(sg2->opposite(e2, n2) == MATCHN_SIMPLE(t1))
        { e2->match = e1;
          found = true;
          break;
         }
      if(!found) return false;
     }
   }

  //check in-edges
  forall_in_edges(e1, n1)
  { t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    { found = false;
      forall_in_edges(e2, n2)
        if(sg2->opposite(e2, n2) == MATCHN_SIMPLE(t1))
        { e2->match = e1;
          found = true;
          break;
         }
      if(!found) return false;
     }
   }

  //add pair
  n1->match = (void*)n2;
  n2->match = (void*)n1;

  int t1both_old = t1both, t2both_old = t2both;

  if(n1->in > 0) t1in--;

  if(n1->out > 0)
  { t1out--;
    if(n1->in > 0) t1both--;
   }

  if(n2->in > 0) t2in--;

  if(n2->out > 0)
  { t2out--;
    if(n2->in > 0) t2both--;
   }

  forall_out_edges(e1, n1)
  { t1 = sg1->opposite(e1, n1);
    if(t1->match == nil && t1->out == 0)
    { t1->out = depth;
      t1out++;
      //if(t1->in > 0)
      //      t1both++;
     }
   }

  forall_out_edges(e2, n2)
  { t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->out == 0)
    { t2->out = depth;
      t2out++;
      //if(t2->in > 0)
      //      t2both++;
     }
   }

//if(t1out == t2out/* && t1both == t2both*/)
  {
   forall_in_edges(e1, n1)
   { t1 = sg1->opposite(e1, n1);
     if(t1->match == nil && t1->in == 0)
     { t1->in = depth;
       t1in++;
       if(t1->out > 0) t1both++;
      }
    }

   forall_in_edges(e2, n2)
   { t2 = sg2->opposite(e2, n2);
     if(t2->match == nil && t2->in == 0)
     { t2->in = depth;
       t2in++;
       if(t2->out > 0) t2both++;
      }
    }

   //Recursion
// if(t1in == t2in && t1both == t2both)
   if(match_iso(depth + 1) && gmb::job == gmb::FINDFIRST) return true;


   //Backtracking
   
   forall_in_edges(e2, n2)
   { t2 = sg2->opposite(e2, n2);
     if(t2->match == nil && t2->in == depth)
     { t2->in = 0;
       t2in--;
       //if(t2->out > 0)
       //      t2both--;
      }
    }

   forall_in_edges(e1, n1)
   { t1 = sg1->opposite(e1, n1);
     if(t1->match == nil && t1->in == depth)
     { t1->in = 0;
       t1in--;
       //if(t1->out > 0)
       //      t1both--;
      }
    }
  }

 forall_out_edges(e2, n2)
 { t2 = sg2->opposite(e2, n2);
   if(t2->match == nil && t2->out == depth)
   { t2->out = 0;
     t2out--;
     //if(t2->in > 0)
     //t2both--;
    }
  }

 forall_out_edges(e1, n1)
 { t1 = sg1->opposite(e1, n1);
   if(t1->match == nil && t1->out == depth)
   { t1->out = 0;
     t1out--;
     //if(t1->in > 0)
     //      t1both--;
    }
  }

 if(n2->in > 0) t2in++;

 if(n2->out > 0)
 { t2out++;
   //if(n2->in > 0)
   //      t2both++;
  }

 if(n1->in > 0) t1in++;

 if(n1->out > 0)
 { t1out++;
   //if(n1->in > 0)
   //      t1both++;
  }

 t1both = t1both_old;
 t2both = t2both_old;

 n1->match = nil;
 n2->match = nil;

 return false;
}



template<class graph_t>
bool vf2_simple<graph_t>::match_iso(int depth)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;
#endif
  if(depth == sg1->number_of_nodes() + 1)
  { //recursion successful
    switch(gmb::job)
    {
      case gmb::FINDFIRST:
           if(gmb::node_morph)
           { //fill in mapping
             node_vf2_simple n;
             forall_nodes(n, *sg2)
               (*gmb::node_morph)[n->original] = MATCHN_SIMPLE(n)->original;
             if(gmb::edge_morph)
             { edge_vf2_simple e;
               forall_edges(e, *sg2)
                 (*gmb::edge_morph)[e->original] = MATCHE_SIMPLE(e)->original;
              }
            }
           break;

      case gmb::FINDALL:
           { morphism* iso = new morphism();

             node_array<node, graph_t>& na = iso->first();
             edge_array<edge, graph_t>& ea = iso->second();
             na.init(*gmb::g2);
             ea.init(*gmb::g2);
             //fill in mapping
             node_vf2_simple n;
             forall_nodes(n, *sg2)
               na[n->original] = MATCHN_SIMPLE(n)->original;
             edge_vf2_simple e;
             forall_edges(e, *sg2)
               ea[e->original] = MATCHE_SIMPLE(e)->original;
             gmb::morphisms->push_back(iso);
            }
           break;

      case gmb::CARDINALITY:
           break;

      case gmb::ENUM:
           { morphism iso;
             node_array<node, graph_t>& na = iso.first();
             edge_array<edge, graph_t>& ea = iso.second();
             na.init(*gmb::g2);
             ea.init(*gmb::g2);
             //fill in mapping
             node_vf2_simple n;
             forall_nodes(n, *sg2)
               na[n->original] = MATCHN_SIMPLE(n)->original;
             edge_vf2_simple e;
             forall_edges(e, *sg2)
               ea[e->original] = MATCHE_SIMPLE(e)->original;

             if((*gmb::callb)(iso))
                gmb::job = gmb::FINDFIRST;      //break recursion
            }
           break;

      }

     gmb::num_mappings++;
     return true;
   }

  if(t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
    node_vf2_simple n, t2out_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->out > 0)
      { t2out_min = n;
        break;
       }

    assert(t2out_min != nil);

    forall_nodes(n, *sg1)
    { if(n->match == nil && n->out > 0)
      { if(check_pair(n, t2out_min, depth) && gmb::job == gmb::FINDFIRST)
          return true;
       }
     }
   }
  else if(t1in > 0 && t2in > 0)
  { //P = T1in(s) x min {T2in(s)}
    node_vf2_simple n, t2in_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->in > 0)
      { t2in_min = n;
        break;
       }

    assert(t2in_min != nil);

    forall_nodes(n, *sg1)
    { if(n->match == nil && n->in > 0)
      { if(check_pair(n, t2in_min, depth) && gmb::job == gmb::FINDFIRST)
           return true;
       }
     }
   }
   else if(t1in + t2in + t1out + t2out == 0)
   { //P = ...
     node_vf2_simple n, r2_min = nil;
     forall_nodes(n, *sg2)
       if(n->match == nil)
       { r2_min = n;
         break;
        }

     forall_nodes(n, *sg1)
     { if(n->match == nil)
       { if(check_pair(n, r2_min, depth) && gmb::job == gmb::FINDFIRST)
           return true;
        }
      }
    }

  //no mapping possible
  return false;
}



template<class graph_t>
bool vf2_simple<graph_t>::call_iso()
{ //initialization
  t1in = t2in = t1out = t2out = t1both = t2both = 0;
  gmb::num_mappings = 0;
  node_vf2_simple n;
  forall_nodes(n, *sg1) n->initialize();
  forall_nodes(n, *sg2) n->initialize();
  return match_iso(1);
}

#undef SORT_NODES

LEDA_END_NAMESPACE

#endif
