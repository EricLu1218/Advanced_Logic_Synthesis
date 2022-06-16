/*******************************************************************************
+
+  _vf_2_exp.h
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
+  experimental version
+
*******************************************************************************/
#ifndef VF2_EXP_H
#define VF2_EXP_H

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/morphism/_vf2_base.h>
#include <LEDA/graph/morphism/_forall.h>

#include <LEDA/graph/graph_morphism_algorithm.h>


LEDA_BEGIN_NAMESPACE


#define SORT_NODES
#define SPECIAL_SORTING
#define SEARCH_LIST

//declaration

template<class graph_t, class impl> class graph_morphism;

template<class graph_t = graph>
class vf2_exp : public vf2_base<graph_t>
{
  friend class graph_morphism<graph_t, vf2_exp<graph_t> >;

#if defined(__SUNPRO_CC)
  public:
#else
  protected:
#endif

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

    
  virtual ~vf2_exp() {}; // virtual destructor (sn)

private:
  struct node_info_vf2_exp : data_slots<0>
  { 
    node_info_vf2_exp* pred_in;
    node_info_vf2_exp* succ_in;
    node_info_vf2_exp* pred_out;
    node_info_vf2_exp* succ_out;

    node original;

    void* match;
    void* nodep;
    void* reached_edge;

    int indegree;
    short member;

    void initialize()
    { match = nil;
      reached_edge = NULL;
      pred_in = this;
      succ_in = this;
      pred_out = this;
      succ_out = this;
      member = 0;
     }

    //static const short mask_in = 0x1, mask_out = 0x2;
    enum { mask_in = 0x1, mask_out = 0x2 };

    void draw_back_in() 
    { //extract from list, but remember position
      pred_in->succ_in = succ_in; 
      succ_in->pred_in = pred_in; 
     }  

    void infiltrate_in() 
    { //insert into former position
      pred_in->succ_in = this; 
      succ_in->pred_in = this;
     }       

    void append_in(node_info_vf2_exp* i) 
    { //"this" must be list head
      pred_in->succ_in = i; 
      i->pred_in = pred_in; 
      i->succ_in = this; 
      pred_in = i; 
      i->member |= mask_in; 
     }

    bool empty_in() 
    { return pred_in == this /*or succ_in == this */; } // must be list head

    void remove_in() 
    { //remove permanently
      pred_in->succ_in = succ_in; 
      succ_in->pred_in = pred_in; 
      succ_in = NULL; 
      pred_in = NULL; 
      member &= ~mask_in; 
     } 

    int member_in() { return member & mask_in; }

    void draw_back_out() 
    { //extract from list, but remember position
      pred_out->succ_out = succ_out; 
      succ_out->pred_out = pred_out; 
     }   

    void infiltrate_out() 
    { //insert into former position
      pred_out->succ_out = this; 
      succ_out->pred_out = this; 
     }  

    void append_out(node_info_vf2_exp* i) 
    { //"this" must be list head
      pred_out->succ_out = i; 
      i->pred_out = pred_out; 
      i->succ_out = this; 
      pred_out = i;  
      i->member |= mask_out; 
     }

    bool empty_out() 
    { return pred_out == this /*or succ_out == this */; }  // must be list head

    void remove_out() 
    { //remove permanently
      pred_out->succ_out = succ_out; 
      succ_out->pred_out = pred_out; 
      succ_out = NULL; 
      pred_out = NULL; 
      member &= ~mask_out;
    }

    int member_out() { return member & mask_out; }
  };


  struct edge_info_vf2_exp : public data_slots<0>
  { typename graph_t::edge original;
    void* match;
   };


  typedef static_graph<opposite_graph, node_info_vf2_exp, edge_info_vf2_exp> 
                                                                 graph_vf2_exp;

  typedef typename graph_vf2_exp::node node_vf2_exp;
  typedef typename graph_vf2_exp::edge edge_vf2_exp;
  typedef two_tuple<graph_t const*, graph_vf2_exp*> prep_graph_vf2_exp_t;

  //"internal" data
  graph_vf2_exp* sg1, * sg2;
  int t1in, t2in, t1out, t2out;
  node_info_vf2_exp t1_head, t2_head;

  static void sort_special(const graph_t& g, array<node>& nodes);
  static void construct_static_graph(const graph_t& g, graph_vf2_exp& sg);
  void initialize_static_graph(graph_vf2_exp& sg);
  inline void fill_in_mapping();

  bool check_pair(node_vf2_exp n1, node_vf2_exp n2, unsigned int depth);
  bool check_pair_sub(node_vf2_exp n1, node_vf2_exp n2, unsigned int depth);
  bool check_pair_mono(node_vf2_exp n1, node_vf2_exp n2, unsigned int depth);

  bool match_iso(int depth);
  bool match_sub(int depth);
  bool match_mono(int depth);

protected:
  virtual bool call_iso();
  virtual bool call_sub();
  virtual bool call_mono();

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

//unfortunately, these casts are necessary to avoid template recursion
#define MATCHN_EXP(n) ((node_vf2_exp)(n->match))
#define MATCHE_EXP(n) ((edge_vf2_exp)(n->match))

template<class graph_t>
void vf2_exp<graph_t>::input1(const graph_t& _g1)
{ //convert to static graphs for best performance
  sg1 = new graph_vf2_exp();
  gmb::g1 = &_g1;
  construct_static_graph(*gmb::g1, *sg1);
  gmb::constructed1 = true;
 }

template<class graph_t>
void vf2_exp<graph_t>::input1(prep_graph _pg1)
{ prep_graph_vf2_exp_t* pg1 = (prep_graph_vf2_exp_t*)_pg1;
  gmb::g1 = pg1->first();
  sg1 = pg1->second();
  gmb::constructed1 = false;
 }

template<class graph_t>
void vf2_exp<graph_t>::input2(const graph_t& _g2)
{ //convert to static graphs for best performance
  sg2 = new graph_vf2_exp();
  gmb::g2 = &_g2;
  construct_static_graph(*gmb::g2, *sg2);
  gmb::constructed2 = true;
 }

template<class graph_t>
void vf2_exp<graph_t>::input2(prep_graph _pg2)
{ prep_graph_vf2_exp_t* pg2 = (prep_graph_vf2_exp_t*)_pg2;
  gmb::g2 = pg2->first();
  sg2 = pg2->second();
  gmb::constructed2 = false;
 }

template<class graph_t>
void vf2_exp<graph_t>::clean_up()
{ if(gmb::constructed1) delete sg1;
  if(gmb::constructed2) delete sg2;
 }


template<class graph_t>
typename graph_morphism_algorithm<graph_t>::prep_graph 
vf2_exp<graph_t>::prepare_graph(const graph_t& _g, 
                                const node_compat* _node_comp, 
                                const edge_compat* _edge_comp) const
{
  prep_graph_vf2_exp_t* pg = new prep_graph_vf2_exp_t;
  pg->first() = &_g;
  pg->second() = new graph_vf2_exp();
  construct_static_graph(_g, *pg->second());
  return (prep_graph)pg;
 }

template<class graph_t>
void vf2_exp<graph_t>::delete_prepared_graph(prep_graph _pg) const
{ prep_graph_vf2_exp_t* pg = (prep_graph_vf2_exp_t*)_pg;
  delete pg->second();
  delete pg;
 }


template<typename T1, typename T2>
int compare_by_second(const two_tuple<T1, T2>& a, const two_tuple<T1, T2>& b)
{ return -compare(a.second(), b.second()); }


template<class graph_t>
void vf2_exp<graph_t>::sort_special(const graph_t& g, array<node>& nodes)
{ node_comparator<graph_t> nc(&g);
  list<two_tuple<int, int> > breaks;

  int last_begin = 0;
  int i; 
  for(i = 0; i < nodes.size() - 1; i++)
  { if(nc(nodes[i], nodes[i + 1]) != 0)
    { breaks.push_back(two_tuple<int, int>(last_begin, i + 1 - last_begin));
      last_begin = i + 1;
     }
   }
  breaks.push_back(two_tuple<int, int>(last_begin, i + 1 - last_begin));

  if(breaks.size() > 1)
  { breaks.sort(compare_by_second<int, int>);
    array<node> nodes2(nodes);
    int counter = 0;
    two_tuple<int, int> b;
    forall(b, breaks)
      for(int j = 0; j < b.second(); j++)
         nodes2[counter++] = nodes[b.first() + j];
    nodes = nodes2;
   }
 }


//copy a graph or static_graph into a static_graph with additional data 
//needed for this algorithm
template<class graph_t>
void vf2_exp<graph_t>::construct_static_graph(const graph_t& g, 
                                              graph_vf2_exp& sg)
{
  sg.start_construction(g.number_of_nodes(), g.number_of_edges());

  node_comparator<graph_t> nc(&g);

  //for sorting, copy node references to array temporarily
  array<node> nodes(g.number_of_nodes());
  int i = 0;

  node n;
  forall_nodes(n, g) nodes[i++] = n;
#ifdef SORT_NODES
  nodes.sort(nc);
#ifdef SPECIAL_SORTING
  sort_special(g, nodes);
#endif
#endif

  //finally, construct all the nodes and edges
  node_array<node_vf2_exp, graph_t> mapping(g);
  forall(n, nodes)
  { node_vf2_exp sn = sg.new_node();
    sn->original = n;           //remember where it came from
    sn->indegree = g.indeg(n);  //calculating the in-degree is expensive, 
                                //so store it here
    sn->nodep = (void*)sn;      //to get from the node information to the 
                                // node reference itself
    mapping[n] = sn;            //temporary, to reconstruct the edges
   }

  forall(n, nodes)
  { node_vf2_exp sn = mapping[n];

    edge e;
    forall_out_edges_graph_t(g,e,n)
    { node o = g.opposite(e, n);
      edge_vf2_exp se = sg.new_edge(sn, mapping[o]);
      se->original = e;
     }
   }
  sg.finish_construction(); 
}


//initialize the static graph
template<class graph_t>
void vf2_exp<graph_t>::initialize_static_graph(graph_vf2_exp& sg)
{ node_vf2_exp n;
  forall_nodes(n, sg) n->initialize();
 }


//check pair for feasibility, graph-graph isomorphism
template<class graph_t>
bool vf2_exp<graph_t>::check_pair(node_vf2_exp n1, node_vf2_exp n2, 
                                                   unsigned int depth)
{
  if (n1->indegree != n2->indegree ||
      sg1->outdeg(n1) != sg2->outdeg(n2) ||
      (gmb::node_comp && (*gmb::node_comp)(n1->original, n2->original)) != 0)
    return false;

  //list positions to roll back to when backtracking
  node_info_vf2_exp* rollback1in;
  node_info_vf2_exp* rollback1out;
  node_info_vf2_exp* rollback2in;
  node_info_vf2_exp* rollback2out;

  { //block to save stack space during recursion

    edge_vf2_exp e1, e2;
    node_vf2_exp t1, t2;
                
//                              e1
//      sg1             n1      ->      t1
//                      |               |       match
//                      V               V
//      sg2             n2      ->      t2
//                              e2

    //counter for every memebership status (bit encoded: 00, 01, 10, 11)

    int t1inc[4]  = {0, 0, 0, 0};
    int t1outc[4] = {0, 0, 0, 0};
    int t2inc[4]  = {0, 0, 0, 0};
    int t2outc[4] = {0, 0, 0, 0};

    //number of matched adjacent nodes
    int num_matched;

    //check out-edges
    num_matched = 0;

    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if (t1->match != nil)
        { MATCHN_EXP(t1)->reached_edge = e1;
          num_matched++;
         }
      else
        t1outc[t1->member]++;
     }

    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if (t2->reached_edge && (!gmb::edge_comp || 
         (*gmb::edge_comp)(e2->original, ((edge_vf2_exp)(t2->reached_edge))->original)==0))
      { //store corresponding edge for easier isomorphism construction
        e2->match = t2->reached_edge;   
        num_matched--;
       }

      if(t2->match == nil) t2outc[t2->member]++;
     }

    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match != nil) MATCHN_EXP(t1)->reached_edge = NULL;
     }

    if(num_matched != 0) return false;

    //check in-edges
    num_matched = 0;
    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match != nil)
       { MATCHN_EXP(t1)->reached_edge = e1;
         num_matched++;
        }
      else
        t1inc[t1->member]++;
     }

    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->reached_edge && (!gmb::edge_comp || 
         (*gmb::edge_comp)(e2->original, ((edge_vf2_exp)(t2->reached_edge))->original)==0))
      { e2->match = t2->reached_edge;
        num_matched--;
       }

      if(t2->match == nil) t2inc[t2->member]++;
     }

    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match != nil) MATCHN_EXP(t1)->reached_edge = NULL;
     }

    if(num_matched != 0) return false;

    //any chance left that this will become a feasible mapping?
    if (t1inc[0]  != t2inc[0]  || t1inc[1]  != t2inc[1]  || 
        t1inc[2]  != t2inc[2]  || t1inc[3]  != t2inc[3]  ||
        t1outc[0] != t2outc[0] || t1outc[1] != t2outc[1] || 
        t1outc[2] != t2outc[2] || t1outc[3] != t2outc[3] )
      return false;

    //add pair, let's try it

    n1->match = (void*)n2;
    n2->match = (void*)n1;

    //remove chosen elements from lists, but remember former position
    if(n1->member_in())
    { t1in--;
      n1->draw_back_in();
     }

    if(n1->member_out())
    { t1out--;
      n1->draw_back_out();
     }

    if(n2->member_in())
    { t2in--;
      n2->draw_back_in();
     }

    if(n2->member_out())
    { t2out--;
      n2->draw_back_out();
     }

    //remember rollback points
    rollback1in = t1_head.pred_in;  //last element
    rollback1out = t1_head.pred_out;
    rollback2in = t2_head.pred_in;
    rollback2out = t2_head.pred_out;

    //add new nodes
    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match == nil && !t1->member_out())
      { t1_head.append_out(t1);
        t1out++;
       }
     }

     forall_in_edges(e1, n1)
     { t1 = sg1->opposite(e1, n1);
       if(t1->match == nil && !t1->member_in())
       { t1_head.append_in(t1);
         t1in++;
        }
      }

     forall_out_edges(e2, n2)
     { t2 = sg2->opposite(e2, n2);
       if(t2->match == nil && !t2->member_out())
       { t2_head.append_out(t2);
         t2out++;
        }
      }

     forall_in_edges(e2, n2)
     { t2 = sg2->opposite(e2, n2);
       if(t2->match == nil && !t2->member_in())
       { t2_head.append_in(t2);
         t2in++;
        }
      }

    }

   //
   //recursion
   //

   if(match_iso(depth + 1) && gmb::job == gmb::FINDFIRST) return true;


   //backtracking
        
   node_info_vf2_exp* it;
        
   for(it = t2_head.pred_in; it != rollback2in; )
   { t2in--;
     it = it->pred_in;
     it->succ_in->remove_in();
    }

   for(it = t2_head.pred_out; it != rollback2out; )
   { t2out--;
     it = it->pred_out;
     it->succ_out->remove_out();
    }

   for(it = t1_head.pred_in; it != rollback1in; )
   { t1in--;
     it = it->pred_in;
     it->succ_in->remove_in();
    }

   for(it = t1_head.pred_out; it != rollback1out; )
   { t1out--;
     it = it->pred_out;
     it->succ_out->remove_out();
    }

   if(n2->member_in())
   { t2in++;
     n2->infiltrate_in();
    }

   if(n2->member_out())
   { t2out++;
     n2->infiltrate_out();
    }

   if(n1->member_in())
   { t1in++;
     n1->infiltrate_in();
    }

   if(n1->member_out())
   { t1out++;
     n1->infiltrate_out();
    }

   n1->match = nil;
   n2->match = nil;

   return false;
}





//check pair for feasibility, graph-subgraph isomorphism

template<class graph_t>
bool vf2_exp<graph_t>::check_pair_sub(node_vf2_exp n1, node_vf2_exp n2, 
                                                       unsigned int depth)
{ if (n1->indegree < n2->indegree ||
      sg1->outdeg(n1) < sg2->outdeg(n2) ||
      (gmb::node_comp && (*gmb::node_comp)(n1->original, n2->original)) != 0)
    return false;

  node_info_vf2_exp* rollback1in;
  node_info_vf2_exp* rollback1out;
  node_info_vf2_exp* rollback2in;
  node_info_vf2_exp* rollback2out;

  { //block to save stack space during recursion

    edge_vf2_exp e1, e2;
    node_vf2_exp t1, t2;
                
//                              e1
//      sg1             n1      ->      t1
//                      |               |       match
//                      V               V
//      sg2             n2      ->      t2
//                              e2

    int t1inc[4]  = {0, 0, 0, 0};
    int t1outc[4] = {0, 0, 0, 0};
    int t2inc[4]  = {0, 0, 0, 0};
    int t2outc[4] = {0, 0, 0, 0};

    int num_matched;
    bool ok;

    //check out-edges
    num_matched = 0;
    ok = true;
    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if (t2->match != nil)
        { MATCHN_EXP(t2)->reached_edge = e2;
          num_matched++;
         }
      else
        t2outc[t2->member]++;
     }

    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);

      if(t1->match == nil)
      { t1outc[t1->member]++;
        continue;
       }

      if(t1->reached_edge && (!gmb::edge_comp || 
         (*gmb::edge_comp)(e1->original, ((edge_vf2_exp)(t1->reached_edge))->original)==0))
        { ((edge_vf2_exp)(t1->reached_edge))->match = e1;
          num_matched--;
         }
      else
        { ok = false;
          break;
         }
     }

    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil) MATCHN_EXP(t2)->reached_edge = NULL;
     }

    if(!ok || num_matched != 0) return false;

    //check in-edges
    num_matched = 0;
    ok = true;
    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil)
        { MATCHN_EXP(t2)->reached_edge = e2;
          num_matched++;
         }
      else
        t2inc[t2->member]++;
     }

    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);

      if(t1->match == nil)
      { t1inc[t1->member]++;
        continue;
       }

      if(t1->reached_edge && (!gmb::edge_comp || 
        (*gmb::edge_comp)(e1->original, ((edge_vf2_exp)(t1->reached_edge))->original)==0))
        { ((edge_vf2_exp)(t1->reached_edge))->match = e1;
          num_matched--;
         }
      else
        { ok = false;
          break;
         }
     }

    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil) MATCHN_EXP(t2)->reached_edge = NULL;
     }

    if(!ok || num_matched != 0) return false;

    if (t1inc[0]  < t2inc[0]  || t1inc[1]  < t2inc[1]  || 
        t1inc[2]  < t2inc[2]  || t1inc[3]  < t2inc[3]  ||
        t1outc[0] < t2outc[0] || t1outc[1] < t2outc[1] || 
        t1outc[2] < t2outc[2] || t1outc[3] < t2outc[3] )
      return false;


    //add pair
    n1->match = (void*)n2;
    n2->match = (void*)n1;

    //remove chosen elements from lists, but remember former position

    if(n1->member_in())
    { t1in--;
      n1->draw_back_in();
     }

    if(n1->member_out())
    { t1out--;
      n1->draw_back_out();
     }

    if(n2->member_in())
    { t2in--;
      n2->draw_back_in();
     }

    if(n2->member_out())
    { t2out--;
      n2->draw_back_out();
     }

    //remember rollback points
    rollback1in = t1_head.pred_in;  //last element
    rollback1out = t1_head.pred_out;
    rollback2in = t2_head.pred_in;
    rollback2out = t2_head.pred_out;

    //add new nodes
    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match == nil && !t1->member_out())
      { t1_head.append_out(t1);
        t1out++;
       }
     }

    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match == nil && !t1->member_in())
      { t1_head.append_in(t1);
        t1in++;
       }
     }

    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match == nil && !t2->member_out())
      { t2_head.append_out(t2);
        t2out++;
       }
     }

    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match == nil && !t2->member_in())
      { t2_head.append_in(t2);
        t2in++;
       }
     }

   }

  //recursion
  if(match_sub(depth + 1) && gmb::job == gmb::FINDFIRST) return true;

  //backtracking
        
  node_info_vf2_exp* it;
        
  for(it = t2_head.pred_in; it != rollback2in; )
  { t2in--;
    it = it->pred_in;
    it->succ_in->remove_in();
   }

  for(it = t2_head.pred_out; it != rollback2out; )
  { t2out--;
    it = it->pred_out;
    it->succ_out->remove_out();
   }

  for(it = t1_head.pred_in; it != rollback1in; )
  { t1in--;
    it = it->pred_in;
    it->succ_in->remove_in();
   }

  for(it = t1_head.pred_out; it != rollback1out; )
  { t1out--;
    it = it->pred_out;
    it->succ_out->remove_out();
   }

  if(n2->member_in())
  { t2in++;
    n2->infiltrate_in();
   }

  if(n2->member_out())
  { t2out++;
    n2->infiltrate_out();
   }

  if(n1->member_in())
  { t1in++;
    n1->infiltrate_in();
   }

  if(n1->member_out())
  { t1out++;
    n1->infiltrate_out();
   }

  n1->match = nil;
  n2->match = nil;

  return false;
}





//check pair for feasibility, graph-graph monomorphism
template<class graph_t>
bool vf2_exp<graph_t>::check_pair_mono(node_vf2_exp n1, node_vf2_exp n2, unsigned int depth)
{ if (n1->indegree < n2->indegree ||
      sg1->outdeg(n1) < sg2->outdeg(n2) ||
      (gmb::node_comp && (*gmb::node_comp)(n1->original, n2->original)) != 0)
    return false;

  node_info_vf2_exp* rollback1in, * rollback1out, * rollback2in, * rollback2out;

  { //block to save stack space during recursion
    edge_vf2_exp e1, e2;
    node_vf2_exp t1, t2;

//                              e1
//      sg1             n1      ->      t1
//                      |               |       match
//                      V               V
//      sg2             n2      ->      t2
//                              e2

    int t1inc[4]  = {0, 0, 0, 0};
    int t1outc[4] = {0, 0, 0, 0};
    int t2inc[4]  = {0, 0, 0, 0};
    int t2outc[4] = {0, 0, 0, 0};

    int num_matched;

    //check out-edges
    num_matched = 0;
    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil)
      { MATCHN_EXP(t2)->reached_edge = e2;
        num_matched++;
       }
      else
        t2outc[t2->member]++;
     }

    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->reached_edge && 
        (!gmb::edge_comp || (*gmb::edge_comp)(e1->original, ((edge_vf2_exp)(t1->reached_edge))->original)==0))
        if(t1->reached_edge)
       { ((edge_vf2_exp)(t1->reached_edge))->match = e1;
         num_matched--;
        }

       if(t1->match == nil)
       t1outc[t1->member]++;
      }

    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil) MATCHN_EXP(t2)->reached_edge = NULL;
     }

    if(num_matched != 0) return false;

    //check in-edges
    num_matched = 0;
    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil)
       { MATCHN_EXP(t2)->reached_edge = e2;
         num_matched++;
        }
       else 
          t2inc[t2->member]++;
     }

    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->reached_edge && (!gmb::edge_comp || 
         (*gmb::edge_comp)(e1->original, ((edge_vf2_exp)(t1->reached_edge))->original)==0))
      { ((edge_vf2_exp)(t1->reached_edge))->match = e1;
        num_matched--;
       }

      if(t1->match == nil) t1inc[t1->member]++;
     }

    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match != nil) MATCHN_EXP(t2)->reached_edge = NULL;
     }

    if(num_matched != 0) return false;

    if (t1inc[0] + t1inc[1] + t1inc[2] + t1inc[3] < t2inc[0] || 
        t1inc[1] + t1inc[3] < t2inc[1] || 
        t1inc[2] + t1inc[3] < t2inc[2] || 
        t1inc[3] < t2inc[3] ||
        t1outc[0] + t1outc[1] + t1outc[2] + t1outc[3] < t2outc[0] || 
        t1outc[1] + t1outc[3] < t2outc[1] || 
        t1outc[2] + t1outc[3] < t2outc[2] || 
        t1outc[3] < t2outc[3] )
      return false;

    //add pair
    n1->match = (void*)n2;
    n2->match = (void*)n1;

    //remove chosen elements from lists, but remember former position
    if(n1->member_in())
    { t1in--;
      n1->draw_back_in();
     }

    if(n1->member_out())
    { t1out--;
      n1->draw_back_out();
     }

    if(n2->member_in())
    { t2in--;
      n2->draw_back_in();
     }

    if(n2->member_out())
    { t2out--;
      n2->draw_back_out();
     }

    //remember rollback points
    rollback1in = t1_head.pred_in;  //last element
    rollback1out = t1_head.pred_out;
    rollback2in = t2_head.pred_in;
    rollback2out = t2_head.pred_out;

    //add new nodes
    forall_out_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match == nil)
      { if(!t1->member_out())
        { t1_head.append_out(t1);
          t1out++;
         }
       }
     }

    forall_in_edges(e1, n1)
    { t1 = sg1->opposite(e1, n1);
      if(t1->match == nil)
      { if(!t1->member_in())
        { t1_head.append_in(t1);
          t1in++;
         }
       }
     }

    forall_out_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match == nil)
      { if(!t2->member_out())
        { t2_head.append_out(t2);
          t2out++;
         }
       }
     }

    forall_in_edges(e2, n2)
    { t2 = sg2->opposite(e2, n2);
      if(t2->match == nil)
      { if(!t2->member_in())
        { t2_head.append_in(t2);
          t2in++;
         }
       }
     }

   }

   //recursion
   if(match_mono(depth + 1) && gmb::job == gmb::FINDFIRST) return true;

  //backtracking
        
  node_info_vf2_exp* it;
        
  for(it = t2_head.pred_in; it != rollback2in; )
  { t2in--;
    it = it->pred_in;
    it->succ_in->remove_in();
   }

  for(it = t2_head.pred_out; it != rollback2out; )
  { t2out--;
    it = it->pred_out;
    it->succ_out->remove_out();
   }

  for(it = t1_head.pred_in; it != rollback1in; )
  { t1in--;
    it = it->pred_in;
    it->succ_in->remove_in();
   }

  for(it = t1_head.pred_out; it != rollback1out; )
  { t1out--;
    it = it->pred_out;
    it->succ_out->remove_out();
   }

  if(n2->member_in())
  { t2in++;
    n2->infiltrate_in();
   }

  if(n2->member_out())
  { t2out++;
    n2->infiltrate_out();
   }

  if(n1->member_in())
  { t1in++;
    n1->infiltrate_in();
   }

  if(n1->member_out())
  { t1out++;
    n1->infiltrate_out();
   }

  n1->match = nil;
  n2->match = nil;

  return false;
}



template<class graph_t>
void vf2_exp<graph_t>::fill_in_mapping()
{
  switch(gmb::job)
  {
    case gmb::FINDFIRST:
         if(gmb::node_morph)
         { //fill in mapping
           node_vf2_exp n;
           forall_nodes(n, *sg2)
             (*gmb::node_morph)[n->original] = MATCHN_EXP(n)->original;
           if(gmb::edge_morph)
           { edge_vf2_exp e;
             forall_edges(e, *sg2)
               (*gmb::edge_morph)[e->original] = MATCHE_EXP(e)->original;
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
           node_vf2_exp n;
           forall_nodes(n, *sg2) na[n->original] = MATCHN_EXP(n)->original;
           edge_vf2_exp e;
           forall_edges(e, *sg2) ea[e->original] = MATCHE_EXP(e)->original;
           gmb::morphisms->push_back(iso);
          }
         gmb::num_mappings++;
         break;

    case gmb::CARDINALITY:
         gmb::num_mappings++;
         break;

    case gmb::ENUM:
         { morphism iso;
           node_array<node, graph_t>& na = iso.first();
           edge_array<edge, graph_t>& ea = iso.second();
           na.init(*gmb::g2);
           ea.init(*gmb::g2);
           //fill in mapping
           node_vf2_exp n;
           forall_nodes(n, *sg2) na[n->original] = MATCHN_EXP(n)->original;
           edge_vf2_exp e;
           forall_edges(e, *sg2) ea[e->original] = MATCHE_EXP(e)->original;

           if ((*gmb::callb)(iso))
                 gmb::job = gmb::FINDFIRST;      //break recursion
          }

         gmb::num_mappings++;
         break;
    }
}





//recurse to match another pair of nodes, graph-graph isomorphism
template<class graph_t>
bool vf2_exp<graph_t>::match_iso(int depth)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;       //count calls to this routine
#endif

  if(depth == sg2->number_of_nodes() + 1)
  { //recursion successful
    fill_in_mapping();
    return true;
   }

  if (t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
    node_info_vf2_exp* it;
#ifdef SEARCH_LIST
    node_vf2_exp t2out_min = (node_vf2_exp)(t2_head.succ_out->nodep);
    for(it = t2_head.succ_out->succ_out; it != &t2_head; it = it->succ_out)
       if(sg2->index(((node_vf2_exp)(it->nodep))) < sg2->index(t2out_min))
          t2out_min = (node_vf2_exp)(it->nodep);
#else
    node_vf2_exp n, t2out_min = nil;
    forall_nodes(n, *sg2)
    if(n->match == nil && n->member_out())
    { t2out_min = n;
      break;
     }
#endif
    for(it = t1_head.succ_out; it != &t1_head; it = it->succ_out)
       if(check_pair((node_vf2_exp)(it->nodep), t2out_min, depth) && 
          gmb::job == gmb::FINDFIRST)
          return true;
   }
   else if(t1in > 0 && t2in > 0)
   { //P = T1in(s) x min {T2in(s)}
    node_info_vf2_exp* it;
#ifdef SEARCH_LIST
     node_vf2_exp t2in_min = (node_vf2_exp)(t2_head.succ_in->nodep);
     for(it = t2_head.succ_in->succ_in; it != &t2_head; it = it->succ_in)
        if(sg2->index((node_vf2_exp)(it->nodep)) < sg2->index(t2in_min))
           t2in_min = (node_vf2_exp)(it->nodep);
#else
     node_vf2_exp n, t2in_min = nil;
     forall_nodes(n, *sg2)
        if(n->match == nil && n->member_in())
        { t2in_min = n;
          break;
         }
#endif

     for(it = t1_head.succ_in; it != &t1_head; it = it->succ_in)
        if(check_pair((node_vf2_exp)(it->nodep), t2in_min, depth) && 
           gmb::job == gmb::FINDFIRST)
           return true;
    }
   else if((t2in + t2out == 0) && (t1in + t1out == 0))
   { //P = ...

     node_vf2_exp n, r2_min = nil;

     forall_nodes(n, *sg2)
       if(n->match == nil)
       { r2_min = n;
         break;
        }

     forall_nodes(n, *sg1)
       if(n->match == nil)
       { if (check_pair(n, r2_min, depth) && gmb::job == gmb::FINDFIRST)
            return true;
        }
    }

  //no mapping possible
  return false;
}





//recurse to match another pair of nodes, graph-subgraph isomorphism
template<class graph_t>
bool vf2_exp<graph_t>::match_sub(int depth)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;
#endif

  if(depth == sg2->number_of_nodes() + 1)
  { //recursion successful
    fill_in_mapping();
    return true;
   }

  if(t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
    node_info_vf2_exp* it;
#ifdef SEARCH_LIST
    node_vf2_exp t2out_min = (node_vf2_exp)(t2_head.succ_out->nodep);
    for(it = t2_head.succ_out->succ_out; it != &t2_head; it = it->succ_out)
       if(sg2->index(((node_vf2_exp)(it->nodep))) < sg2->index(t2out_min))
         t2out_min = (node_vf2_exp)(it->nodep);
#else
    node_vf2_exp n, t2out_min = nil;
    forall_nodes(n, *sg2)
       if(n->match == nil && n->member_out())
       { t2out_min = n;
         break;
        }
#endif
    for(it = t1_head.succ_out; it != &t1_head; it = it->succ_out)
       if(check_pair_sub((node_vf2_exp)(it->nodep), t2out_min, depth) && 
          gmb::job == gmb::FINDFIRST)
          return true;
   }
  else 
    if(t1in > 0 && t2in > 0)
    { //P = T1in(s) x min {T2in(s)}
      node_info_vf2_exp* it;
#ifdef SEARCH_LIST
      node_vf2_exp t2in_min = (node_vf2_exp)(t2_head.succ_in->nodep);
      for(it = t2_head.succ_in->succ_in; it != &t2_head; it = it->succ_in)
         if(sg2->index((node_vf2_exp)(it->nodep)) < sg2->index(t2in_min))
           t2in_min = (node_vf2_exp)(it->nodep);
#else
      node_vf2_exp n, t2in_min = nil;
      forall_nodes(n, *sg2)
      if(n->match == nil && n->member_in())
      { t2in_min = n;
        break;
       }
#endif

      for(it = t1_head.succ_in; it != &t1_head; it = it->succ_in)
          if(check_pair_sub((node_vf2_exp)(it->nodep), t2in_min, depth) && 
             gmb::job == gmb::FINDFIRST) return true;
     }
     else if(t2in + t2out == 0)
     { //P = ...
       node_vf2_exp n, r2_min = nil;
       forall_nodes(n, *sg2)
          if(n->match == nil)
          { r2_min = n;
            break;
           }

       forall_nodes(n, *sg1)
          if(n->match == nil)
          { if(check_pair_sub(n, r2_min, depth) && gmb::job == gmb::FINDFIRST)
              return true;
           }
      }

  //no mapping possible
  return false;
}





//recurse to match another pair of nodes, graph-graph monomorphism
template<class graph_t>
bool vf2_exp<graph_t>::match_mono(int depth)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;
#endif

  if(depth == sg2->number_of_nodes() + 1)
  { //recursion successful
    fill_in_mapping();
    return true;
   }

  if(t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
    node_info_vf2_exp* it;
#ifdef SEARCH_LIST
    node_vf2_exp t2out_min = (node_vf2_exp)(t2_head.succ_out->nodep);
    for(it = t2_head.succ_out->succ_out; it != &t2_head; it = it->succ_out)
        if(sg2->index(((node_vf2_exp)(it->nodep))) < sg2->index(t2out_min))
           t2out_min = (node_vf2_exp)(it->nodep);
#else
    node_vf2_exp n, t2out_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->member_out())
      { t2out_min = n;
        break;
       }
#endif
    for(it = t1_head.succ_out; it != &t1_head; it = it->succ_out)
        if(check_pair_mono((node_vf2_exp)(it->nodep), t2out_min, depth) && 
            gmb::job == gmb::FINDFIRST)
            return true;
   }
  else if(t1in > 0 && t2in > 0)
  { //P = T1in(s) x min {T2in(s)}
    node_info_vf2_exp* it;
#ifdef SEARCH_LIST
    node_vf2_exp t2in_min = (node_vf2_exp)(t2_head.succ_in->nodep);
    for(it = t2_head.succ_in->succ_in; it != &t2_head; it = it->succ_in)
        if(sg2->index((node_vf2_exp)(it->nodep)) < sg2->index(t2in_min))
            t2in_min = (node_vf2_exp)(it->nodep);
#else
    node_vf2_exp n, t2in_min = nil;
    forall_nodes(n, *sg2)
       if(n->match == nil && n->member_in())
       { t2in_min = n;
         break;
        }
#endif

    for(it = t1_head.succ_in; it != &t1_head; it = it->succ_in)
        if(check_pair_mono((node_vf2_exp)(it->nodep), t2in_min, depth) && 
           gmb::job == gmb::FINDFIRST)
             return true;
   }
  else if(t2in + t2out == 0)
  {  //P = ...
     node_vf2_exp n, r2_min = nil;
     forall_nodes(n, *sg2)
        if(n->match == nil)
        { r2_min = n;
          break;
         }

     forall_nodes(n, *sg1)
        if(n->match == nil)
        { if(check_pair_mono(n, r2_min, depth) && gmb::job == gmb::FINDFIRST)
                 return true;
         }
    }

  //no mapping possible
  return false;
}


template<class graph_t>
bool vf2_exp<graph_t>::call_iso()
{ //initialization
  t1in = t2in = t1out = t2out = 0;        //all sets empty
  initialize_static_graph(*sg1);
  initialize_static_graph(*sg2);
  t1_head.initialize();
  t2_head.initialize();

  bool successful = match_iso(1);

  return successful;
}


template<class graph_t>
bool vf2_exp<graph_t>::call_sub()
{ //initialization
  t1in = t2in = t1out = t2out = 0;
  initialize_static_graph(*sg1);
  initialize_static_graph(*sg2);
  t1_head.initialize();
  t2_head.initialize();

  bool successful = match_sub(1);

  return successful;
}


template<class graph_t>
bool vf2_exp<graph_t>::call_mono()
{ //initialization
  t1in = t2in = t1out = t2out = 0;
  initialize_static_graph(*sg1);
  initialize_static_graph(*sg2);
  t1_head.initialize();
  t2_head.initialize();

  bool successful = match_mono(1);

  return successful;
}


#undef SORT_NODES
#undef SEARCH_LIST


LEDA_END_NAMESPACE

#endif
