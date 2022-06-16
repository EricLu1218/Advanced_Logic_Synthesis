/*******************************************************************************
+
+  _conauto_basic.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  first implementation of the conauto algorithm
+
*******************************************************************************/

#ifndef CONAUTO_BASIC_H
#define CONAUTO_BASIC_H

//#define VERBOSE

#include <LEDA/core/list.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/graph/morphism/_node_ordered_partition.h>
#include <LEDA/graph/morphism/_ext_adjacency_matrix.h>
#include <LEDA/graph/morphism/_compose.h>
#include <LEDA/graph/morphism/_conauto_base.h>

LEDA_BEGIN_NAMESPACE


template<class graph_t, class impl> class graph_morphism;

template<class graph_t = graph>
class conauto_basic : public conauto_base<graph_t, degree_t>
{
  friend class graph_morphism<graph_t, conauto_basic<graph_t> >;

protected:
  typedef graph_morphism_base<graph_t> gmb;
  typedef conauto_base<graph_t, degree_t> base;
  typedef conauto_base<graph_t, degree_t> cb;
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
  typedef typename base::cardinality_t cardinality_t;
  typedef typename base::JOB JOB;
  typedef typename base::PIVOT PIVOT;

private:
  typedef cell_info<graph_t, degree_t> cell_info_t;
  typedef node_ordered_partition<graph_t, cell_info_t> partition;
  typedef typename partition::cell_t cell_t;

  struct step_info
  { PIVOT   pivot_kind;
    cell_t* pivot_cell;
    step_info() { pivot_kind = cb::UNKNOWN; pivot_cell = NULL; }
   };

  typedef compose<step_info, partition> step;

  class prep_graph_conauto_basic
  {
    public:
    const graph_t& g;
    list<step*> seq;
    partition discard;
    node_array<bool, graph_t> mapped;
    node_array<cell_info_t, graph_t> degree;
    ext_adjacency_matrix<graph_t> adj;
    cardinality_t card_automorphism, card_automorphism_backtr;
    bool initially_partitioned;
    bool first_sequence_generated;
    bool searched_for_automorphisms;

    const leda_cmp_base<cell_info_t>* initial_comp;

    prep_graph_conauto_basic(const graph_t& _g) : 
                             g(_g), discard(_g), mapped(_g), degree(_g), adj(_g)
    { card_automorphism = 1;
      card_automorphism_backtr = 1;
      initially_partitioned = false;
      first_sequence_generated = false;
      searched_for_automorphisms = false;
      initial_comp = NULL;
     }

   ~prep_graph_conauto_basic() 
    { if (initial_comp != &ccp) delete initial_comp; }
  };


  class cell_comparator_part_compat : public leda_cmp_base<cell_info_t>
  {
    private:
    const node_compat* node_comp;

    public:
    cell_comparator_part_compat(const node_compat* _compat_nodes) 
    { node_comp = _compat_nodes; }

    virtual int operator()(const cell_info_t& ci1, const cell_info_t& ci2) const
    { for(int i = 1; i <= 3; i++)
      { if (ci1.pdegrees[i] < ci2.pdegrees[i]) return -1;
        if (ci1.pdegrees[i] > ci2.pdegrees[i]) return  1;
       }
      return (*node_comp)(ci1.rep, ci2.rep);
     }
   };

  static typename base::template cell_comparator_part<graph_t> ccp;
  static typename base::template cell_comparator_cell<graph_t> ccc;

  prep_graph_conauto_basic* i1, * i2;
  bool full_backtracking;

  void calculate_degree(prep_graph_conauto_basic* i, const graph_t& g, 
                        const node n, degree_t* pdegrees) const;

  void partition_initially(prep_graph_conauto_basic* i, partition& p) const;

  void partition_by_node(prep_graph_conauto_basic* i, const cell_t& c, 
                         node pivot, bool valid, partition& result) const;

  bool find_best_pivot(const partition& p, cell_t*& best) const;

  int  get_num_links(cell_t& c) const { return c.pdegrees[0]; }
  bool has_links(cell_t& c)     const { return c.pdegrees[0] > 0; }

  void refine_by_node(prep_graph_conauto_basic* i, const partition& curr, 
                      node pivot_node, 
                      const cell_t& pivot_cell, 
                      partition& next, 
                      bool first_sequence, 
                      partition* discard) const;


  void calculate_avail_degree(prep_graph_conauto_basic* i, const graph_t& g, 
                              const cell_t& pivot_cell, 
                              degree_t offset) const;

  bool refine_by_set(prep_graph_conauto_basic* i, const partition& curr, 
                     const cell_t& pivot_cell, 
                     partition& next, 
                     partition* discard) const;

  void generate_first_sequence_of_partitions(prep_graph_conauto_basic* i) const;

  bool is_equivalent_to_pivot_node(prep_graph_conauto_basic* i, node n, 
                                   cell_t* c, list_item st) const;

  bool are_vertices_in_pivot_set_equivalent(prep_graph_conauto_basic* i, 
                                            list_item st) const;

  bool is_refine_by_set_compatible(prep_graph_conauto_basic* i, 
                                   const partition& next, 
                                   const partition& pcurr, 
                                   const cell_t& pivot_cell, 
                                   partition& pnext, 
                                   partition* discard) const;

  bool are_final_partitions_equivalent(prep_graph_conauto_basic* i1, 
                                       prep_graph_conauto_basic* i2, 
                                       const partition& p) const;

  void search_for_automorphisms(prep_graph_conauto_basic* i) const;
  bool recurse_nodes(list_item cell_item1, list_item cell_item2, node n1);
  bool recurse_cells(list_item cell_item1, list_item cell_item2);
  void fill_in_mapping(const partition& curr, const partition& pcurr);
  bool match(list_item s, const step& pcurr);

  void initialize(prep_graph_conauto_basic* i, 
                  const node_compat* _node_comp, 
                  const edge_compat* _edge_comp) const;

  void input1(const graph_t& _g1);
  void input1(prep_graph pg1);

  void input2(const graph_t& _g2);
  void input2(prep_graph pg2);

  void clean_up();

  typename graph_morphism_algorithm<graph_t>::prep_graph 
       prepare_graph(const graph_t& g, const node_compat* _node_comp, 
                                       const edge_compat* _edge_comp) const;

  void delete_prepared_graph(prep_graph pg) const;

  //compute graph isomorphism
  virtual bool call_iso();
};



template<class graph_t>
typename conauto_base<graph_t, degree_t>::template cell_comparator_part<graph_t> conauto_basic<graph_t>::ccp;

template<class graph_t>
typename conauto_base<graph_t, degree_t>::template cell_comparator_cell<graph_t> conauto_basic<graph_t>::ccc;

template<class graph_t>
void conauto_basic<graph_t>::initialize(prep_graph_conauto_basic* i, 
                                        const node_compat* _node_comp, 
                                        const edge_compat* _edge_comp) const
{ if(_edge_comp != NULL)
  { leda_error_handler(1, "Algorithm ignores edge compatibility.");
    return;
   }

  if (i->initially_partitioned) return;

  if (_node_comp)
     i->initial_comp = new cell_comparator_part_compat(_node_comp);
  else
     i->initial_comp = &ccp;

  i->seq.push_back(new step(i->g));
  partition_initially(i, *i->seq.front());

  i->initially_partitioned = true;
 }


template<class graph_t>
void conauto_basic<graph_t>::input1(const graph_t& _g1)
{ gmb::g1 = &_g1;
  i1 = new prep_graph_conauto_basic(_g1);
  gmb::constructed1 = true;
 }


template<class graph_t>
void conauto_basic<graph_t>::input1(prep_graph pg1)
{ i1 = (prep_graph_conauto_basic*)pg1;
  gmb::g1 = &i1->g;
  gmb::constructed1 = false;
 }


template<class graph_t>
void conauto_basic<graph_t>::input2(const graph_t& _g2)
{ gmb::g2 = &_g2;
  i2 = new prep_graph_conauto_basic(_g2);
  gmb::constructed2 = true;
 }


template<class graph_t>
void conauto_basic<graph_t>::input2(prep_graph pg2)
{ i2 = (prep_graph_conauto_basic*)pg2;
  gmb::g2 = &i2->g;
  gmb::constructed2 = false;
 }


template<class graph_t>
void conauto_basic<graph_t>::clean_up()
{ if(gmb::constructed1) delete_prepared_graph(i1);
  if(gmb::constructed2) delete_prepared_graph(i2);
 }

template<class graph_t>
typename graph_morphism_algorithm<graph_t>::prep_graph 
    conauto_basic<graph_t>::prepare_graph(const graph_t& g, 
                                          const node_compat* _node_comp, 
                                          const edge_compat* _edge_comp) const
{ if(_edge_comp != NULL)
  { leda_error_handler(1, "Algorithm ignores edge compatibility.");
    return NULL;
   }

  prep_graph_conauto_basic* pg = new prep_graph_conauto_basic(g);
  initialize(pg, _node_comp, _edge_comp);
  return (prep_graph)pg;
}


template<class graph_t>
void conauto_basic<graph_t>::delete_prepared_graph(prep_graph _pg) const
{ prep_graph_conauto_basic* pg = (prep_graph_conauto_basic*)_pg;
  step* s;
  forall(s, pg->seq) delete s;
  delete pg;
}


template<class graph_t>
void conauto_basic<graph_t>::calculate_degree(prep_graph_conauto_basic* i, 
                                              const graph_t& g, 
                                              node n, 
                                              degree_t* pdegrees) const
{ for(int j = 0; j < 4; j++) pdegrees[j] = 0;

  edge e;
  { forall_out_edges_graph_t(g, e, n)
      pdegrees[i->adj(g.index(n), g.index(g.opposite(e, n)))]++;
   }

  { forall_in_edges_graph_t(g, e, n)
      pdegrees[i->adj(g.index(n), g.index(g.opposite(e, n)))]++;
   }
    
  pdegrees[3] /= 2;
  pdegrees[0] = pdegrees[1] + pdegrees[2] + pdegrees[3];
}


template<class graph_t>
void conauto_basic<graph_t>::partition_initially(prep_graph_conauto_basic* i, 
                                                 partition& p) const
{ const graph_t& g = p.get_graph();
  sortseq<cell_info_t, cell_t*> cells(*i->initial_comp);

  node n;
  forall_nodes(n, g)
  { cell_info_t ci;
    calculate_degree(i, g, n, ci.pdegrees);
    ci.rep = n;

    seq_item si = cells.lookup(ci);
    cell_t* c;
    if(si == nil)
      { c = p.new_cell(ci);
        cells.insert(ci, c);
       }
    else
      c = cells[si];

    c->append(n);
   }

  cell_t* c;
  forall(c, cells) p.push_back(c);
}


template<class graph_t>
bool conauto_basic<graph_t>::find_best_pivot(const partition& p, 
                                             cell_t*& best) const
{ bool possible = false;
  best = NULL;

  cell_t* c;
  forall(c, p)
  { if (c->size() > 1 && has_links(*c)) possible = true;

    if (!best) { best = c; continue; }

    if (c->valid)
    { if (!best->valid || best->size() > c->size() || 
          (c->size() == 1 && get_num_links(*c) > get_num_links(*best)))
          best = c;
      }
    else
    { if (!best->valid && has_links(*c) && 
          (get_num_links(*best) == 0 || best->size() > c->size())) 
          best = c;
     }
   }

 return possible;
}


template<class graph_t>
void conauto_basic<graph_t>::partition_by_node(prep_graph_conauto_basic* i, 
                                               const cell_t& c, 
                                               node pivot, 
                                               bool valid, 
                                               partition& result) const
{ cell_t* conn[4];
  for(int j = 0; j < 4; j++) conn[j] = result.new_cell(c);

  node n;
  forall_nodes(n, c)
  { if(n == pivot) continue;
    adj_t a = i->adj(pivot, n);
    conn[a]->append(n);
   }

  int parts = 0;
  for(int i = 0; i < 4; i++)
     if(conn[i]->size() > 0) parts++;

  for(int i = 3; i >= 0; i--)
  { if (conn[i]->size() > 0)
      { if(i != 0)
        { conn[i]->pdegrees[i]--; //pivot node will be discarded
          conn[i]->pdegrees[0]--;
         }
        conn[i]->valid = (valid || parts > 1) && conn[i]->pdegrees[0] > 0;
        result.append(conn[i]);
       }
    else
      delete conn[i];
   }
}


template<class graph_t>
void conauto_basic<graph_t>::refine_by_node(prep_graph_conauto_basic* i, 
                                            const partition& curr, 
                                            node pivot_node, 
                                            const cell_t& pivot_cell, 
                                            partition& next, 
                                            bool first_sequence, 
                                            partition* discard) const
{ cell_t* c;
  forall(c, curr)
  { if (has_links(*c))
      partition_by_node(i, *c, pivot_node, c->valid || c == &pivot_cell, next);
    else 
      if (c != &pivot_cell)
      { if (first_sequence) //discard complete cell
           i->card_automorphism *= fac(c->size());
        if(discard)
        { discard->copy_cell(*c);
#ifdef VERBOSE
          cout << "discarding " << *((node_disjoint_set<graph_t>*)c) << endl;
#endif
         }
       }
   }

   if (discard)
   { c = discard->new_cell();
     c->rep = pivot_node;
     c->append(pivot_node);
#ifdef VERBOSE
     cout << "discarding " << *((node_disjoint_set<graph_t>*)c) << endl;
#endif
     discard->push_back(c);
   }
}


template<class graph_t>
void conauto_basic<graph_t>::calculate_avail_degree(prep_graph_conauto_basic* i,
                                                    const graph_t& g, 
                                                    const cell_t& pivot_cell, 
                                                    degree_t offset) const
{ node s;
  forall_nodes(s, pivot_cell)
  { edge e;
    { forall_out_edges_graph_t(g, e, s)
      { node t = g.opposite(e, s);
        i->degree[t].cdegrees[i->adj(t, s)] += offset;
       }
     }

     { forall_in_edges_graph_t(g, e, s)
       { node t = g.opposite(e, s);
         i->degree[t].cdegrees[i->adj(t, s)] += offset;
        }
      }
   }
}


template<class graph_t>
bool conauto_basic<graph_t>::refine_by_set(prep_graph_conauto_basic* i, 
                                           const partition& curr, 
                                           const cell_t& pivot_cell, 
                                           partition& next, 
                                           partition* discard) const
{ bool success = false;
  cardinality_t factor = 1;
  list<cell_t*> to_discard;

  calculate_avail_degree(i, i->g, pivot_cell, +1);

  cell_t *c;
  forall(c, curr)
  { if (has_links(*c))
      { // split c
        sortseq<cell_info_t, cell_t*> cells(ccc);
  
        node n;
        forall_nodes(n, *c)
        { cell_info_t& ci = i->degree[n];
          seq_item si = cells.lookup(ci);
          cell_t* nc;
          if(si == nil)
            { nc = next.new_cell(*c);
              for(int i = 0; i < 4; i++) nc->cdegrees[i] = ci.cdegrees[i];
              cells.insert(ci, nc);
             }
          else
            nc = cells[si];
  
          nc->append(n);
         }
  
        for(seq_item si = cells.last_item(); si != nil; si = cells.pred(si))
        { cells[si]->valid = c->valid || cells.size() > 1;
          next.push_back(cells[si]);
         }
  
        success |= cells.size() > 1;
       }
    else
      { //discard complete cell
        factor *= fac(c->size());
        to_discard.push_back(c);
       }
   }
                        
   calculate_avail_degree(i, i->g, pivot_cell, -1);

   if(success)
   { i->card_automorphism *= factor;
     if (discard)
     { forall(c, to_discard)
       { discard->copy_cell(*c);
#ifdef VERBOSE
         cout << "discarding " << *((node_disjoint_set<graph_t>*)c) << endl;
#endif
        }
      }
    }
  return success;
}


template<class graph_t>
bool 
conauto_basic<graph_t>::is_refine_by_set_compatible(prep_graph_conauto_basic* i,
                                                    const partition& next, 
                                                   const partition& pcurr, 
                                                   const cell_t& pivot_cell, 
                                                   partition& pnext, 
                                                   partition* discard) const
{ list<cell_t*> to_discard;

   calculate_avail_degree(i, i->g, pivot_cell, +1);

   cell_t *pc;
   list_item c = next.first_item();
   forall(pc, pcurr)
   { if(has_links(*pc))
       { // split pc
         sortseq<cell_info_t, cell_t*> cells(ccc);
  
         node n;
         forall_nodes(n, *pc)
         { cell_info_t& ci = i->degree[n];
           seq_item si = cells.lookup(ci);
           cell_t* nc;
           if(si == nil)
             { nc = pnext.new_cell(*pc);
               for(int i = 0; i < 4; i++) nc->cdegrees[i] = ci.cdegrees[i];
               cells.insert(ci, nc);
              }
           else
             nc = cells[si];
  
           nc->append(n);
          }
  
         // push back in reverse order
         for(seq_item si = cells.last_item(); si != nil; si = cells.pred(si))
         { if(c == nil) return false;
  
           cells[si]->valid = pc->valid || cells.size() > 1;
           if (ccc(cells.key(si), *((cell_info_t*)next[c])) != 0)
           { calculate_avail_degree(i, i->g, pivot_cell, -1);
             return false;
            }
  
           pnext.push_back(cells[si]);
           c = next.succ(c);
          }
        }
     else
       { // discard complete cell
         to_discard.push_back(pc);
        }

    }
                        
   calculate_avail_degree(i, i->g, pivot_cell, -1);

   if (discard)
   {  forall(pc, to_discard)
      { discard->copy_cell(*pc);
#ifdef VERBOSE
        cout << "discarding " << *((<node_disjoint_set<graph_t>*)pc) << endl;
#endif
       }
    }

  return true;
}



template<class graph_t>
void conauto_basic<graph_t>::generate_first_sequence_of_partitions(
                                            prep_graph_conauto_basic* i) const
{ if(i->first_sequence_generated) return;

  step* curr = i->seq.front(), * next;

  cell_t* c;
  forall(c, *curr) c->valid = (curr->size() > 0) && (c->pdegrees[0] > 0);

#ifdef VERBOSE
  cout << *curr << endl << "////////////////////" << endl;
#endif

  cell_t* best;
  while (find_best_pivot(*curr, best))
  { next = new step(i->g);
    if(best->size() == 1)
      { curr->pivot_kind = cb::NODE;
        curr->pivot_cell = best;
        refine_by_node(i, *curr, best->first_node(), *best, *next, true, &i->discard);
       }
    else
      { bool success = false;
        while(best->valid)
        { best->valid = false;
          curr->pivot_kind = cb::CELL;
          success = refine_by_set(i, *curr, *best, *next, &i->discard);
          if(!success)
            { next->clear();
              find_best_pivot(*curr, best);
             }
          else
            { curr->pivot_cell = best;
              break;
             }
         }

        if (!success)
        { curr->pivot_kind = cb::UNKNOWN;
          cell_t* c = curr->front();
          curr->pivot_cell = c;
          refine_by_node(i,*curr,c->first_node(), *c,*next,true,&i->discard);
         }
       }

#ifdef VERBOSE
     switch (curr->pivot_kind)
     { case cb::NODE:    cout << "node" << endl;
                         break;

       case cb::CELL:    cout << "cell" << endl;
                         break;
       case cb::UNKNOWN: cout << "unknown" << endl;
                         break;
      }
     cout << *next << endl << "////////////////////" << endl;
#endif

     i->seq.push_back(next);
     curr = next;
   } // while

  //rest
  forall(c, *curr)
  { i->card_automorphism *= fac(c->size());
    i->discard.copy_cell(*c);
   }

  i->first_sequence_generated = true;
}


template<class graph_t>
bool conauto_basic<graph_t>::are_final_partitions_equivalent(
                                              prep_graph_conauto_basic* _i1, 
                                              prep_graph_conauto_basic* _i2, 
                                              const partition& p) const
{ list<two_tuple<int, int> > linked_nodes;
  list_item c2b = p.first_item();
  partition& final = *_i1->seq.tail();

  cell_t* c2a;
  forall(c2a, final)
  { if (has_links(*c2a))
      linked_nodes.push_back(two_tuple<int,int>(_i1->g.index(c2a->first_node()),
                   p.get_graph().index(p[c2b]->first_node()))); //only node
    c2b = p.succ(c2b);
   }
        
  two_tuple<int, int> ni, nj;
  forall(ni, linked_nodes)
    forall(nj, linked_nodes)
      { if (_i1->adj(ni.first(),nj.first()) != _i2->adj(ni.second(),nj.second()))
           return false;
       }

  return true;
}


template<class graph_t>
bool conauto_basic<graph_t>::is_equivalent_to_pivot_node(
                                                prep_graph_conauto_basic* i, 
                                                node n, 
                                                cell_t* c, 
                                                list_item st) const
{ // steps
  step* curr = i->seq[st], * next = i->seq[i->seq.succ(st)];

  // parallel steps
  step pc(i->g), pn(i->g);
  step* pcurr = &pc, * pnext = &pn;
  refine_by_node(i, *curr, n, *c, *pnext, false, NULL);

  bool compatible = next->compatible(*pnext, *i->initial_comp);
  bool equivalent = compatible && next->equivalent(*pnext, *i->initial_comp);

  while (compatible && !equivalent)
  { st = i->seq.succ(st);
    if(i->seq.succ(st) == nil) break;

    curr = i->seq[st];
    next = i->seq[i->seq.succ(st)];

    leda::swap(pcurr, pnext);
    pnext->clear();

    if (curr->pivot_kind == cb::BACKTR
        || (full_backtracking && curr->pivot_kind == cb::NODE_BACKTR)
        || curr->pivot_kind == cb::UNKNOWN)
      return false;
    else
      { cell_t* ppivot_cell = pcurr->corr_cell(*curr, curr->pivot_cell);

        if (curr->pivot_kind == cb::NODE || 
            (!full_backtracking && curr->pivot_kind == cb::NODE_BACKTR))
          {
            refine_by_node(i, *pcurr, ppivot_cell->first_node(), *ppivot_cell, 
                                                          *pnext, false, NULL);
            compatible = next->compatible(*pnext, *i->initial_comp);
           }
        else 
           if(curr->pivot_kind == cb::CELL)
             compatible = is_refine_by_set_compatible(i, *next, *pcurr, 
                                                   *ppivot_cell, *pnext, NULL);
           else
             assert(false);

        equivalent = compatible && next->equivalent(*pnext, *i->initial_comp);
       }
   } // while

  equivalent = equivalent || are_final_partitions_equivalent(i, i, *pnext);
        
  return compatible && equivalent;
}


template<class graph_t>
bool conauto_basic<graph_t>::are_vertices_in_pivot_set_equivalent(
                             prep_graph_conauto_basic* i, list_item st) const
{ step& s = *i->seq[st];
   cell_t* c = s.pivot_cell;
   bool success = true;
   node n;
   for (n=c->next_node(c->first_node()); n!=nil && success; n=c->next_node(n))
      success = is_equivalent_to_pivot_node(i, n, c, st);
   return success;
}



template<class graph_t>
void conauto_basic<graph_t>::search_for_automorphisms(
                                             prep_graph_conauto_basic* i) const
{ if (i->searched_for_automorphisms) return;

  list_item li;
  for(li = i->seq.pred(i->seq.last_item()); 
      li != nil && li != i->seq.first_item(); li = i->seq.pred(li))
  { // don't consider backtracking in the highest level
    step* s = i->seq[li];
    if (s->pivot_kind == cb::UNKNOWN)
    { if (are_vertices_in_pivot_set_equivalent(i, li))
        { s->pivot_kind = cb::NODE_BACKTR;
          i->card_automorphism_backtr *= s->pivot_cell->size();
         }
      else
        s->pivot_kind = cb::BACKTR;
     }
   }

  i->searched_for_automorphisms = true;
}



template<class graph_t>
bool conauto_basic<graph_t>::recurse_nodes(list_item cell_item1, 
                                           list_item cell_item2, node n1)
{ if (n1 == nil)
     return recurse_cells(i1->discard.succ(cell_item1), 
                          i2->discard.succ(cell_item2));

  node n2;
  forall_nodes(n2, *i2->discard.inf(cell_item2))
  { if(i2->mapped[n2]) continue;

    (*gmb::node_morph)[n2] = n1;
    i2->mapped[n2] = true;

    if (!recurse_nodes(cell_item1, cell_item2, 
               i1->discard.inf(cell_item1)->next_node(n1))) return false;

    i2->mapped[n2] = false;
   }

 return true;
}



template<class graph_t>
bool conauto_basic<graph_t>::recurse_cells(list_item cell_item1, 
                                           list_item cell_item2)
{ if (cell_item1 == nil)
   { map_edges(i1->g, i2->g, *gmb::node_morph, *gmb::edge_morph);
  
      if (gmb::job == gmb::FINDALL)
       gmb::morphisms->push_back(new morphism(node_morphism(*gmb::node_morph), 
                                              edge_morphism(*gmb::edge_morph)));
      else
        { if ((*gmb::callb)(morphism(*gmb::node_morph, *gmb::edge_morph)))
             gmb::job = gmb::FINDFIRST;      //break recursion
          return false;
         }
     }
   else 
    recurse_nodes(cell_item1, cell_item2, 
                              i1->discard.inf(cell_item1)->first_node());
  return true;
}


template<class graph_t>
void conauto_basic<graph_t>::fill_in_mapping(const partition& curr, 
                                             const partition& pcurr)
{ switch(gmb::job)
  {
    case gmb::FINDFIRST:
             if(gmb::node_morph)
             { //fill in mapping
               cell_t* c1, * c2;
               list_item d2 = i2->discard.first_item();
               forall(c1, i1->discard)
               { c2 = i2->discard.inf(d2);
                 node n1, n2 = c2->first_node();
                 forall_nodes(n1, *c1)
                 { (*gmb::node_morph)[n2] = n1;
                   n2 = c2->next_node(n2);
                  }
                 d2 = i2->discard.next_item(d2);
                }

               map_edges(i1->g, i2->g, *gmb::node_morph, *gmb::edge_morph);
              }
             gmb::num_mappings++;
             break;

    case gmb::FINDALL:
             recurse_cells(i1->discard.first_item(), i2->discard.first_item());
             gmb::num_mappings += i1->card_automorphism;
             break;

    case gmb::CARDINALITY:
             gmb::num_mappings += i1->card_automorphism * i1->card_automorphism_backtr;
             break;

    case gmb::ENUM:
             recurse_cells(i1->discard.first_item(), i2->discard.first_item());
             gmb::num_mappings += i1->card_automorphism;
             break;
   }
}



template<class graph_t>
bool conauto_basic<graph_t>::match(list_item s, const step& pcurr)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;
#endif

  if(i1->seq.succ(s) == nil)
  { if (are_final_partitions_equivalent(i1, i2, pcurr))
      { cell_t* c;
        forall(c, pcurr) i2->discard.copy_cell(*c);
        fill_in_mapping(*i1->seq[s], pcurr);
        return true;
       }
    else
      return false;
   }

  step* curr = i1->seq[s], * next = i1->seq[i1->seq.succ(s)];

  step pnext(i2->g);
  cell_t* ppivot_cell = pcurr.corr_cell(*curr, curr->pivot_cell);

  if (curr->pivot_kind == cb::BACKTR 
      || (full_backtracking && curr->pivot_kind == cb::NODE_BACKTR) 
      || curr->pivot_kind == cb::UNKNOWN)
    {
      node q;
      forall_nodes(q, *ppivot_cell)
      { list_item status;
        status = i2->discard.last_item();

        refine_by_node(i2, pcurr, q, *ppivot_cell, pnext, false, &i2->discard);
        if (next->compatible(pnext, *i1->initial_comp))
        { if (match(i1->seq.succ(s), pnext) && gmb::job == gmb::FINDFIRST)
             return true;
         }
        pnext.clear();

        list<cell_t*> rest;

        i2->discard.split(status, i2->discard, rest, leda::after);
        cell_t* c;
        forall(c, rest)
        delete c;
       }
      return false;
     }
   else
    { bool compatible = false;
      if(curr->pivot_kind == cb::CELL)
        compatible = is_refine_by_set_compatible(i2, *next, pcurr, *ppivot_cell,
                                                 pnext, &i2->discard);
      else 
        if (curr->pivot_kind == cb::NODE || 
            (!full_backtracking && curr->pivot_kind == cb::NODE_BACKTR))
          { refine_by_node(i2, pcurr, ppivot_cell->first_node(), *ppivot_cell, 
                                                   pnext, false, &i2->discard);
            compatible = next->compatible(pnext, *i1->initial_comp);
           }
        else
          assert(false);

       if (compatible)
         return match(i1->seq.succ(s), pnext);
       else
         return false;
     }
}


template<class graph_t>
bool conauto_basic<graph_t>::call_iso()
{ 
  initialize(i1, gmb::node_comp, gmb::edge_comp);
  initialize(i2, gmb::node_comp, gmb::edge_comp);

  step* initial1 = i1->seq.front(); 
  step* initial2 = i2->seq.front();

  bool compatible = initial1->compatible(*initial2, ccp);

#ifdef VERBOSE
  cout << (compatible ? "compatible" : "not compatible") << endl 
  cout << *initial1 << endl << *initial2 << endl;
#endif

  full_backtracking = gmb::job == gmb::FINDALL || gmb::job == gmb::ENUM;

  if(gmb::job == gmb::FINDALL)
  { gmb::node_morph = new node_morphism(i2->g);
    gmb::edge_morph = new edge_morphism(i2->g);
   }

  if(compatible)
  { generate_first_sequence_of_partitions(i1);
    if (!full_backtracking) search_for_automorphisms(i1);
    match(i1->seq.first_item(), *initial2);
   }
        
  if (gmb::job == gmb::FINDALL)
  { delete gmb::node_morph;
    delete gmb::edge_morph;
   }

  return gmb::num_mappings > 0;
}

#undef VERBOSE

LEDA_END_NAMESPACE

#endif

