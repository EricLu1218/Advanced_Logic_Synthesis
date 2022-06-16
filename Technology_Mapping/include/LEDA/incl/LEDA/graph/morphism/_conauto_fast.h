/*******************************************************************************
+
+  _conauto_fast.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  fast implementation of the conauto algorithm, uses low-level data structures
+
*******************************************************************************/

#ifndef CONAUTO_FAST_H
#define CONAUTO_FAST_H

//#define VERBOSE

#include <LEDA/core/list.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/morphism/_bounded_ordered_partition.h>
#include <LEDA/graph/morphism/_ext_bounded_adjacency_matrix.h>
#include <LEDA/graph/morphism/_compose.h>
#include <LEDA/graph/morphism/_conauto_base.h>


LEDA_BEGIN_NAMESPACE


template<class graph_t, class impl> class graph_morphism;

//ord_t: integer data type, must be capable of storing the node indices
template<class graph_t = graph, typename ord_t = short>
class conauto_fast : public conauto_base<graph_t, ord_t>
{
  friend class graph_morphism<graph_t, conauto_fast<graph_t, ord_t> >;

#if defined(__SUNPRO_CC)
  public:
#else
  protected:
#endif

  typedef graph_morphism_base<graph_t> gmb;
  typedef conauto_base<graph_t, ord_t> cb;
  typedef conauto_base<graph_t, ord_t> base;
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
  typedef typename base::PIVOT PIVOT;

//private:

  //additional node information for the internal static graph
  struct node_info_conauto_fast : data_slots<0>
  { node original;
    ord_t index;
   };

  typedef static_graph<opposite_graph, node_info_conauto_fast> graph_conauto_fast;
  typedef typename graph_conauto_fast::node node_conauto_fast;
  typedef typename graph_conauto_fast::edge edge_conauto_fast;

  typedef cell_info<graph_conauto_fast, ord_t> cell_info_t;
  typedef bounded_ordered_partition<ord_t, cell_info_t> partition;
  typedef typename partition::cell_t cell_t;

  struct step_info
  { PIVOT pivot_kind;
    ord_t pivot_cell;  //if pivot_kind == cb::NODE, 
                       //pivot node is first one in cell with index pivot_cell

    step_info()
    { pivot_kind = cb::UNKNOWN;
      pivot_cell = 0;
     }
   };

  //one step in the sequence of partitions
  typedef compose<step_info, partition> step;

  //internal prepared graph structure, contains all data 
  //that can be associated with one graph

  class prep_graph_conauto_fast
  {
    public:

    const graph_t& g;       //original
    graph_conauto_fast sg;  //internal static graph
    node_conauto_fast* nodes;  //index->node mapping
    ord_t size;             //number of nodes
    list<step*> seq;        //sequence of partitions
    partition discard;      //order in which nodes are discarded
    bool* mapped;           //flags for recursion: Is node already mapped?
    ord_t* temp[4];         //temporary storage of size "size"

    //available degree for a node with respect to a cell (temporary)
    cell_info_t* degree;      

    //adjacency matrix with entries 0..3
    ext_bounded_adjacency_matrix<ord_t> adj;    

    //cardinality of automorphism found so far
    cardinality_t card_automorphism;      
    cardinality_t card_automorphism_backtr;      

    //internal status of this structure: What has already been computed?
    bool initially_partitioned;
    bool first_sequence_generated;
    bool searched_for_automorphisms;

    // node compatibility functor, 
    // can incorporate external node compatibility function
    const leda_cmp_base<cell_info_t>* initial_comp;

    prep_graph_conauto_fast(const graph_t& _g) : g(_g), 
                                                 discard(_g.number_of_nodes()),
                                                 adj(_g.number_of_nodes())
    { 
      size = g.number_of_nodes();

      //allocate memory and initialize it
      nodes = new node_conauto_fast[size];
      mapped = new bool[size];
      degree = new cell_info_t[size];

      for(int j = 0; j < 4; j++) temp[j] = new ord_t[size];

      //current state of the structure
      card_automorphism = 1;
      card_automorphism_backtr = 1;
      initially_partitioned = false;
      first_sequence_generated = false;
      searched_for_automorphisms = false;
      initial_comp = NULL;

      clear();
     }


    void clear()
    {
      for(ord_t n = 0; n < size; n++)
      { mapped[n] = false;
        for(int j = 0; j < 4; j++) degree[n].cdegrees[j] = 0;
       }
     }

   ~prep_graph_conauto_fast()
    { delete[] nodes;
      delete[] mapped;
      delete[] degree;
      for(int j = 0; j < 4; j++) delete[] temp[j];
      if(initial_comp != &ccp) delete initial_comp;
     }
  };


  //compare cells by available degree with respect to all remaining nodes, 
  //then by external compatibility

  class cell_comparator_part_compat : public leda_cmp_base<cell_info_t>
  {
    private:
    const node_compat* node_comp;

    public:
    cell_comparator_part_compat(const node_compat* _compat_nodes) 
    { node_comp = _compat_nodes; }

   virtual int operator()(const cell_info_t& ci1, const cell_info_t& ci2) const
   {
     for(int i = 1; i <= 3; i++)
     {
       if(ci1.pdegrees[i] < ci2.pdegrees[i]) return -1;
       if(ci1.pdegrees[i] > ci2.pdegrees[i]) return 1;
      }

     int res = (*node_comp)(ci1.rep->original, ci2.rep->original);
     if(res != 0) return res;

#ifdef STABLE
     //ensure stable sorting, index must have been stored in cdegrees[0]
     if(ci1.cdegrees[0] < ci2.cdegrees[0]) return -1;
     if(ci1.cdegrees[0] > ci2.cdegrees[0]) return 1;
#endif
     return 0;
    }
  };


  //compare cells by available degree with respect to the pivot cell

  class cell_comparator_cell_pointer_rev : public leda_cmp_base<cell_info_t*>
  {
    public:
    virtual int operator()(cell_info_t* const& ci1, cell_info_t* const& ci2) const
    { for(int i = 1; i <= 3; i++)
      { if (ci1->cdegrees[i] < ci2->cdegrees[i]) return 1;
        if (ci1->cdegrees[i] > ci2->cdegrees[i]) return -1;
       }
#ifdef STABLE
      //ensure stable sorting, index must have been stored in pdegrees[0]
      if(ci1->pdegrees[0] < ci2->pdegrees[0]) return 1;
      if(ci1->pdegrees[0] > ci2->pdegrees[0]) return -1;
#endif
      return 0;
     }
   };

   prep_graph_conauto_fast* i1;      //inputs
   prep_graph_conauto_fast* i2;

   partition* discard;     //order in which nodes are discarded
   bool full_backtracking; //full backtracking due to the need for all mappings?
   mutable list<step*> step_freelist; //freelist for step structures

   //static const adj_t mask_in = 0x1, mask_out = 0x2;
   enum { mask_in = 0x1, mask_out = 0x2 };

   //without external node compatiblity
   static typename base::template cell_comparator_part<graph_conauto_fast> ccp;

   //for pointers on cell_info_t, reverse
   static cell_comparator_cell_pointer_rev cccpr;

   protected:

   //convenience functions for the step freelist
   step* new_step(ord_t size) const;   
   void delete_step(step* s) const;

   static void construct_static_graph(const graph_t& g, 
                                      prep_graph_conauto_fast* i);

   void calculate_degree(prep_graph_conauto_fast* i, node_conauto_fast n, 
                                                     ord_t* pdegrees) const;

   void partition_initially(prep_graph_conauto_fast* i, partition& p) const;

   void initialize(prep_graph_conauto_fast* i, 
                   const node_compat* _node_comp, 
                   const edge_compat* _edge_comp) const;

   void partition_by_node(prep_graph_conauto_fast* i, 
                          const partition& curr, 
                          const cell_t& c, 
                          ord_t pivot, 
                          bool valid, 
                          partition& result) const;

   bool find_best_pivot(const partition& p, cell_t*& best, 
                                            ord_t& best_count) const;

   inline int get_num_links(const cell_t& c) const { return c.pdegrees[0]; }
   inline bool has_links(const cell_t& c)    const { return c.pdegrees[0] > 0;}

   void refine_by_node(prep_graph_conauto_fast* i, const partition& curr, 
                                                   ord_t pivot_node, 
                                                   const cell_t& pivot_cell, 
                                                   partition& next, 
                                                   bool first_sequence, 
                                                   partition* discard) const;

   void calculate_avail_degree(prep_graph_conauto_fast* i, 
                               const partition& curr, 
                               const cell_t& pivot_cell, 
                               ord_t offset) const;

   bool refine_by_set(prep_graph_conauto_fast* i, const partition& curr, 
                                                  const cell_t& pivot_cell,
                                                  partition& next, 
                                                  partition* discard) const;

   void generate_first_sequence_of_partitions(prep_graph_conauto_fast* i) const;

   bool is_equivalent_to_pivot_node(prep_graph_conauto_fast* i, ord_t n, 
                                    const cell_t& c, list_item st) const;

   bool are_vertices_in_pivot_set_equivalent(prep_graph_conauto_fast* i, 
                                             list_item st) const;

   bool is_refine_by_set_compatible(prep_graph_conauto_fast* i, 
                                    const partition& next, 
                                    const partition& pcurr, 
                                    const cell_t& pivot_cell, 
                                    partition& pnext, 
                                    partition* discard) const;

   bool are_final_partitions_equivalent(prep_graph_conauto_fast* i1, 
                                        prep_graph_conauto_fast* i2, 
                                        const partition& p) const;

   void search_for_automorphisms(prep_graph_conauto_fast* i) const;
   bool recurse_nodes(ord_t ci1, ord_t ni1);
   bool recurse_cells(ord_t ci1);
   void fill_in_mapping(const partition& curr, const partition& pcurr);
   bool match(list_item s, const step& pcurr);

   void input1(const graph_t& _g1);
   void input1(prep_graph pg1);
   void input2(const graph_t& _g2);
   void input2(prep_graph pg2);
   void clean_up();

   typename graph_morphism_algorithm<graph_t>::prep_graph prepare_graph(
                                          const graph_t& g, 
                                          const node_compat* _node_comp, 
                                          const edge_compat* _edge_comp) const;

   void delete_prepared_graph(prep_graph pg) const;

   //compute graph isomorphism
   virtual bool call_iso();
};


//without external node compatiblity

template<class graph_t, typename ord_t>
typename conauto_base<graph_t, ord_t>::template cell_comparator_part<static_graph<opposite_graph, typename conauto_fast<graph_t, ord_t>::node_info_conauto_fast> > conauto_fast<graph_t, ord_t>::ccp;


//for pointers on cell_info, reverse

template<class graph_t, typename ord_t>
typename conauto_fast<graph_t, ord_t>::cell_comparator_cell_pointer_rev conauto_fast<graph_t, ord_t>::cccpr;

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::initialize(prep_graph_conauto_fast* i, const node_compat* _node_comp, const edge_compat* _edge_comp) const
{ if(_edge_comp != NULL)
  { leda_error_handler(1, "Algorithm ignores edge compatibility.");
    return;
   }

  if (_node_comp)
     i->initial_comp = new cell_comparator_part_compat(gmb::node_comp);
  else
     i->initial_comp = &ccp;

  if (i->initially_partitioned) return; //already done

  //initial partition is the first step in the sequence
  i->seq.push_back(new step(i->size));    
  partition_initially(i, *i->seq.front());
  i->initially_partitioned = true;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::input1(const graph_t& _g1)
{
  gmb::g1 = &_g1;
  i1 = new prep_graph_conauto_fast(_g1);
  construct_static_graph(_g1, i1);
  gmb::constructed1 = true;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::input1(prep_graph pg1)
{
  i1 = (prep_graph_conauto_fast*)pg1;
  gmb::g1 = &i1->g;
  gmb::constructed1 = false;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::input2(const graph_t& _g2)
{
  gmb::g2 = &_g2;
  i2 = new prep_graph_conauto_fast(_g2);
  construct_static_graph(_g2, i2);
  gmb::constructed2 = true;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::input2(prep_graph pg2)
{
  i2 = (prep_graph_conauto_fast*)pg2;
  gmb::g2 = &i2->g;
  gmb::constructed2 = false;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::clean_up()
{
  if(gmb::constructed1) delete_prepared_graph(i1);
  if(gmb::constructed2) delete_prepared_graph(i2);
}

template<class graph_t, typename ord_t>
typename graph_morphism_algorithm<graph_t>::prep_graph conauto_fast<graph_t, ord_t>::prepare_graph(const graph_t& g, const node_compat* _node_comp, const edge_compat* _edge_comp) const
{
  if(_edge_comp != NULL)
  { leda_error_handler(1, "Algorithm ignores edge compatibility.");
    return NULL;
   }

  prep_graph_conauto_fast* pg = new prep_graph_conauto_fast(g);
  construct_static_graph(g, pg);
  initialize(pg, _node_comp, _edge_comp);
  return (prep_graph)pg;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::delete_prepared_graph(prep_graph _pg) const
{
  prep_graph_conauto_fast* pg = (prep_graph_conauto_fast*)_pg;
  step* s;
  forall(s, pg->seq) delete s;
  delete pg;
}

template<class graph_t, typename ord_t>
typename conauto_fast<graph_t, ord_t>::step* conauto_fast<graph_t, ord_t>::new_step(ord_t size) const
{
  if (step_freelist.empty()) return new step(size);

  step* ns = step_freelist.pop();
  ns->clear();
  return ns;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::delete_step(step* s) const
{
  step_freelist.push_front(s);
}

//copy a graph or static_graph into a static_graph with additional data needed for this algorithm
template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::construct_static_graph(const graph_t& g, prep_graph_conauto_fast* i)
{
  i->sg.start_construction(i->size, g.number_of_edges());

  //construct all the nodes and edges
  ord_t running_index = 0;
  node_array<node_conauto_fast, graph_t> mapping(g);
  node n;
  forall_nodes(n, g)
  { node_conauto_fast sn = i->sg.new_node();
    sn->original = n;       //remember where it came from
    i->nodes[running_index] = sn;
    sn->index = running_index;
    mapping[n] = sn;        //temporary, to reconstruct the edges
    running_index++;
   }

  forall_nodes(n, g)
  { node_conauto_fast sn = mapping[n];

    edge e;
    forall_out_edges_graph_t(g,e,n)
    { node o = g.opposite(e, n);
      node_conauto_fast so = mapping[o];
      /*edge_conauto_fast se = */ // unused variable
      i->sg.new_edge(sn, so);

      i->adj(sn->index, so->index) |= mask_out;
      i->adj(so->index, sn->index) |= mask_in;
     }
   }

   i->sg.finish_construction();
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::calculate_degree(prep_graph_conauto_fast* i, node_conauto_fast n, ord_t* pdegrees) const
{
  for(int j = 0; j < 4; j++) pdegrees[j] = 0;

  edge_conauto_fast e;

  forall_out_edges(e, n)
     pdegrees[i->adj(n->index, i->sg.opposite(e, n)->index)]++;

  forall_in_edges(e, n)
     pdegrees[i->adj(n->index, i->sg.opposite(e, n)->index)]++;

  pdegrees[3] /= 2;       //counted twice before
  pdegrees[0] = pdegrees[1] + pdegrees[2] + pdegrees[3];
}


template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::partition_initially(
                              prep_graph_conauto_fast* i, partition& p) const
{
  array<cell_info_t> node_degrees(0, i->size - 1);

  node_conauto_fast n;
  ord_t index = 0;
  forall_nodes(n, i->sg)
  { calculate_degree(i, n, node_degrees[index].pdegrees);
    node_degrees[index].rep = n;
    node_degrees[index].cdegrees[1] = index;  //remember index in cdegrees[0]
#ifdef STABLE
    node_degrees[index].cdegrees[0] = index;  //and ensure stable sorting, abuse cdegrees[1]
#endif
    i->nodes[index] = n;
    index++;
   }

  if(i->size > 1) node_degrees.sort(*i->initial_comp);

  for(ord_t di = 0; di < i->size; di++)
  {
#ifdef STABLE
    node_degrees[di].cdegrees[0] = 0;       //clean up again
#endif
    if(di == 0 || (*i->initial_comp)(node_degrees[di], node_degrees[di-1]) != 0)
    { cell_t& c = p.new_cell(node_degrees[di]);
      for(int j = 0; j < 4; j++) c.cdegrees[j] = 0;
     }

    p.append_to_curr_cell(node_degrees[di].cdegrees[1]);
    node_degrees[di].cdegrees[1] = 0;       //clean up again
   }

  p.finish();
}

template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::find_best_pivot(const partition& p, 
                                                   cell_t*& best, 
                                                   ord_t& best_count) const
{
  bool feasible = false;

  best = NULL;
  for(ord_t ci = 0; ci < p.num_cells(); ci++)
  {
    cell_t& c = p[ci];
    ord_t c_count = p.num_elem(ci);

    if(c_count > 1 && has_links(c)) feasible = true;

    if(!best)
    { best = &c;
      best_count = c_count;
      continue;
     }

    if(c.valid)
      { if(!best->valid || best_count > c_count || 
                   (c_count == 1 && get_num_links(c) > get_num_links(*best)))
        { best = &c;
          best_count = c_count;
         }
       }
    else
      { if(!best->valid && has_links(c) && 
           (!has_links(*best) == 0 || best_count > c_count))
        { best = &c;
          best_count = c_count;
          }
         }
       }

    return feasible;
}


template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::partition_by_node(prep_graph_conauto_fast* i,
                                                     const partition& curr, 
                                                     const cell_t& c, 
                                                     ord_t pivot, 
                                                     bool valid, 
                                                     partition& result) const
{
  ord_t conn_size[4];     //4 different kinds of connection
  for(int j = 0; j < 4; j++) conn_size[j] = 0;

  for(ord_t ni = curr.first_elem(c.index); ni < curr.end_elem(c.index); ni++)
  { ord_t n = curr.element(ni);

    if(n == pivot)  continue; //skip pivot, is discarded

    adj_t a = i->adj(n, pivot);
    i->temp[a][conn_size[a]++] = n;
   }

   int parts = 0;
   for(int j = 0; j < 4; j++)
       if(conn_size[j] > 0) parts++;        //count number of new cells

   for(int j = 3; j >= 0; j--)
       if(conn_size[j] > 0)
       { cell_t& nc = result.new_cell(c);
         if(j != 0)
         { nc.pdegrees[j]--;   //pivot node will be discarded, 
                               //therefore one less connection for this node
           nc.pdegrees[0]--;
          }
         nc.valid = (valid || parts > 1) && has_links(nc);
         for(ord_t ei = 0; ei < conn_size[j]; ei++)
             result.append_to_curr_cell(i->temp[j][ei]);
        }
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::refine_by_node(prep_graph_conauto_fast* i, 
                                                  const partition& curr, 
                                                  ord_t pivot_node, 
                                                  const cell_t& pivot_cell, 
                                                  partition& next, 
                                                  bool first_sequence, 
                                                  partition* _discard) const
{
  for(ord_t ci = 0; ci < curr.num_cells(); ci++)
  { cell_t& c = curr[ci];
    ord_t c_count = curr.num_elem(ci);

    if(has_links(c))
       partition_by_node(i,curr,c,pivot_node,c.valid || &c == &pivot_cell,next);
    else 
       if(&c != &pivot_cell)
       { //discard complete cell
         if(first_sequence) i->card_automorphism *= fac(c_count);
         if(_discard) _discard->copy_cell(curr, ci);
        }
   }

  if(_discard)
  { //store discarded cells
    cell_t& c = _discard->new_cell();
    c.rep = i->nodes[pivot_node];
    _discard->append_to_curr_cell(pivot_node);
   }

  next.finish();
}


//calculate available degree for all nodes with respect to cell pivot_cell
//offset == -1 means cleaning up again
template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::calculate_avail_degree(
                                                    prep_graph_conauto_fast* i, 
                                                    const partition& curr, 
                                                    const cell_t& pivot_cell, 
                                                    ord_t offset) const
{ node_conauto_fast s, t;
  edge_conauto_fast e;

  forall_nodes(s, i->sg) i->degree[s->index].rep = s;

  for(int si = curr.first_elem(pivot_cell.index); 
                                si < curr.end_elem(pivot_cell.index); si++)
  { s = i->nodes[curr.element(si)];

    forall_out_edges(e, s)
    { t = i->sg.opposite(e, s);
      i->degree[t->index].cdegrees[i->adj(t->index, s->index)] += offset;
     }

    forall_in_edges(e, s)
    { t = i->sg.opposite(e, s);
      i->degree[t->index].cdegrees[i->adj(t->index, s->index)] += offset;
     }
   }
}


template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::is_refine_by_set_compatible(
                                                   prep_graph_conauto_fast* i, 
                                                   const partition& next, 
                                                   const partition& pcurr, 
                                                   const cell_t& pivot_cell, 
                                                   partition& pnext, 
                                                   partition* _discard) const
{
  ord_t*& to_discard = i->temp[0];
  ord_t to_discard_count = 0;

  calculate_avail_degree(i, pcurr, pivot_cell, +1);

  array<cell_info_t*> node_degrees(0, i->size - 1);
  ord_t cell_count = 0;

  for(ord_t ci = 0; ci < pcurr.num_cells(); ci++)
  { cell_t& c = pcurr[ci];

    if(has_links(c))
    { //then split c
      ord_t index = 0;
      for(ord_t ni = pcurr.first_elem(ci); ni < pcurr.end_elem(ci); ni++)
      { node_degrees[index] = &i->degree[pcurr.element(ni)];
#ifdef STABLE
        node_degrees[index]->pdegrees[0] = index;  //ensure stable sorting
#endif
        index++;
       }

      if(index > 1) node_degrees.sort(cccpr, 0, index - 1);

      ord_t num_cells = 0;
      for(ord_t n = 0; n < index; n++)
      {
#ifdef STABLE
        node_degrees[n]->pdegrees[0] = 0;
#endif
        if (n == 0 || cccpr(node_degrees[n - 1], node_degrees[n]) != 0)
        {
          if (cccpr(node_degrees[n], &next[cell_count]) != 0)
          { calculate_avail_degree(i, pcurr, pivot_cell, -1);
            return false;
           }
          cell_t& nc = pnext.new_cell(c);
          for(int j = 0; j < 4; j++) 
             nc.cdegrees[j] = node_degrees[n]->cdegrees[j];
          num_cells++;
          cell_count++;
         }
        pnext.append_to_curr_cell(node_degrees[n]->rep->index);
      }
    }
    else
      { //discard complete cell
        to_discard[to_discard_count++] = ci;
        }
  }
                        
  calculate_avail_degree(i, pcurr, pivot_cell, -1);

  if(_discard)
     for(ord_t d = 0; d < to_discard_count; d++)
        _discard->copy_cell(pcurr, to_discard[d]);

  pnext.finish();
  return true;
}


template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::refine_by_set(prep_graph_conauto_fast* i, const partition& curr, const cell_t& pivot_cell, partition& next, partition* _discard) const
{
  bool success = false;
  cardinality_t factor = 1;
  ord_t*& to_discard = i->temp[0];
  ord_t to_discard_count = 0;

  calculate_avail_degree(i, curr, pivot_cell, +1);

  array<cell_info_t*> node_degrees(0, i->size - 1);
  for(ord_t ci = 0; ci < curr.num_cells(); ci++)
  {
    cell_t& c = curr[ci];

    if(has_links(c))
    { //then split c
      ord_t index = 0;
      for(ord_t ni = curr.first_elem(ci); ni < curr.end_elem(ci); ni++)
      { node_degrees[index] = &i->degree[curr.element(ni)];
#ifdef STABLE
        node_degrees[index]->pdegrees[0] = index;  //ensure stable sorting
#endif
        index++;
       }

      if(index > 1) node_degrees.sort(cccpr, 0, index - 1);

      ord_t num_cells = 0;
      for(ord_t n = 0; n < index; n++)
      { if(n == 0 || cccpr(node_degrees[n - 1], node_degrees[n]) != 0)
        {
#ifdef STABLE
          node_degrees[n]->pdegrees[0] = 0;
#endif
          cell_t& nc = next.new_cell(c);
          for(int j = 0; j < 4; j++)
          nc.cdegrees[j] = node_degrees[n]->cdegrees[j];
          num_cells++;
         }
        next.append_to_curr_cell(node_degrees[n]->rep->index);
       }

      if(num_cells > 1)
         for(ord_t cj = next.num_cells()-num_cells; cj < next.num_cells(); cj++)
            next[cj].valid = true;

      success |= num_cells > 1;
     }
     else
       { //discard complete cell
         factor *= curr.num_elem(ci);
         to_discard[to_discard_count++] = ci;
        }
   }
                        
  calculate_avail_degree(i, curr, pivot_cell, -1);

  if(success)
  { i->card_automorphism *= factor;
    if(_discard)
      for(ord_t d = 0; d < to_discard_count; d++)
         _discard->copy_cell(curr, to_discard[d]);

   }

  next.finish();
  return success;
}

template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::generate_first_sequence_of_partitions(
                                             prep_graph_conauto_fast* i) const
{
  if(i->first_sequence_generated) return;

  step* curr = i->seq.front();
  step* next;  //current partition, next partition

  for(ord_t ci = 0; ci < curr->num_cells(); ci++)
     (*curr)[ci].valid = (curr->num_cells() > 0) && (has_links((*curr)[ci]));

#ifdef VERBOSE
  cout << *curr << endl << "////////////////////" << endl;
#endif

  cell_t* best;
  ord_t best_count;
  while(find_best_pivot(*curr, best, best_count))
  { next = new step(i->size);
    if(best_count == 1)
     { curr->pivot_kind = cb::NODE;
       curr->pivot_cell = best->index;
       refine_by_node(i, *curr, curr->element(best->start), *best, *next, true, &i->discard);
      }
    else
     { bool success = false;
       while(best->valid)
       { //try to find cell that splits something
         best->valid = false;
         curr->pivot_kind = cb::CELL;
         success = refine_by_set(i, *curr, *best, *next, &i->discard);
         if(!success)
           { next->clear();
             find_best_pivot(*curr, best, best_count);
            }
         else
           { curr->pivot_cell = best->index;
             break;
            }
        }
       if(!success)
       { //worst case: try some cell
         curr->pivot_kind = cb::UNKNOWN;
         curr->pivot_cell = 0;
         refine_by_node(i, *curr, curr->element(0), (*curr)[0], *next, true, &i->discard);
        }
      }

#ifdef VERBOSE
    switch(curr->pivot_kind)
    {
       case cb::NODE:    cout << "node" << endl;
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
  }

  //discard rest cells
  for(ord_t ci = 0; ci < curr->num_cells(); ci++)
  { i->card_automorphism *= fac(curr->num_elem(ci));
    i->discard.copy_cell(*curr, ci);
   }

  i->discard.finish();
  i->first_sequence_generated = true;
}


template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::are_final_partitions_equivalent(prep_graph_conauto_fast* _i1, prep_graph_conauto_fast* _i2, const partition& p) const
{
  ord_t* linked1 = _i1->temp[0]; 
  ord_t* linked2 = _i1->temp[1];
  partition& final = *_i1->seq.tail();

  if (final.num_cells() != p.num_cells() || final.num_elem()  != p.num_elem())
     return false;

  //find out corresponding nodes
  ord_t lc = 0;
  for(ord_t ci = 0; ci < final.num_cells(); ci++)
  { if(has_links(final[ci]))
     { //only one element
       linked1[lc] = final.element(final.first_elem(ci)); 
       linked2[lc] = p.element(p.first_elem(ci));
       lc++;
      }
    }

  //check for equivalency
  for(ord_t ei1 = 0; ei1 < lc; ei1++)
     for(ord_t ei2 = 0; ei2 < lc; ei2++)
        if((_i1->adj)(linked1[ei1],linked1[ei2]) != 
                      (i2->adj)(linked2[ei1], linked2[ei2]))
          return false;

  return true;
}



template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::is_equivalent_to_pivot_node(
                                                   prep_graph_conauto_fast* i, 
                                                   ord_t n, 
                                                   const cell_t& c, 
                                                   list_item st) const
{
  //steps (in 1st graph)

  step* curr = i->seq[st], * next = i->seq[i->seq.succ(st)];

  //parallel steps (in 2nd graph), don't store complete sequence, 
  //but only the current and the next partition

  step* pcurr = new_step(i->size), * pnext = new_step(i->size);

  refine_by_node(i, *curr, n, c, *pnext, false, NULL);

  bool compatible = next->compatible(*pnext, *i->initial_comp);

  bool equivalent = compatible && next->equivalent(*pnext, *i->initial_comp);

  while(compatible && !equivalent)
  { st = i->seq.succ(st);   //check next partition
    if(i->seq.succ(st) == nil) break;
    curr = i->seq[st];
    next = i->seq[i->seq.succ(st)];
    leda::swap(pcurr, pnext);
    pnext->clear();

    if (curr->pivot_kind == cb::BACKTR 
      || (full_backtracking && curr->pivot_kind == cb::NODE_BACKTR) 
      || curr->pivot_kind == cb::UNKNOWN)
     {
       delete_step(pcurr);
       delete_step(pnext);
       return false;
      }
    else
     { if (curr->pivot_kind == cb::NODE 
        || (!full_backtracking && curr->pivot_kind == cb::NODE_BACKTR))
         { refine_by_node(i, *pcurr, 
                          pcurr->element(pcurr->first_elem(curr->pivot_cell)),
                          (*pcurr)[curr->pivot_cell], *pnext, false, NULL);

           compatible = next->compatible(*pnext, *i->initial_comp);
          }
       else 
          if(curr->pivot_kind == cb::CELL)
             compatible = is_refine_by_set_compatible(i, *next, *pcurr, 
                                      (*curr)[curr->pivot_cell], *pnext, NULL);

       equivalent = compatible && next->equivalent(*pnext, *i->initial_comp);
      }
   }

  equivalent = equivalent || are_final_partitions_equivalent(i, i, *pnext);
        
  delete_step(pcurr);     //using free list
  delete_step(pnext);

  return compatible && equivalent;
}


template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::are_vertices_in_pivot_set_equivalent(
                                                    prep_graph_conauto_fast* i,
                                                    list_item st) const
{ step& s = *i->seq[st];
  ord_t ci = s.pivot_cell;
  bool success = true;
  for(ord_t ni = s.first_elem(ci) + 1; ni < s.end_elem(ci) && success; ni++)
     success = is_equivalent_to_pivot_node(i, s.element(ni), s[ci], st);
  return success;
}


template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::search_for_automorphisms(
                                             prep_graph_conauto_fast* i) const
{
  if(i->searched_for_automorphisms) return;

  list_item li;
  for(li = i->seq.pred(i->seq.last_item()); 
               li != nil && li != i->seq.first_item(); li = i->seq.pred(li))
  { //don't consider backtracking in the highest level
    step* s = i->seq[li];
                
    if(s->pivot_kind == cb::UNKNOWN)
    { if(are_vertices_in_pivot_set_equivalent(i, li))
       { s->pivot_kind = cb::NODE_BACKTR;
         i->card_automorphism_backtr *= s->num_elem(s->pivot_cell);
        }
      else
        s->pivot_kind = cb::BACKTR;
     }
   }

  i->searched_for_automorphisms = true;
}


//generate mapping through dfs

template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::recurse_nodes(ord_t ci1, ord_t ni1)
{
  if (ni1 == i1->discard.end_elem(ci1))
    return recurse_cells(ci1 + 1);
  else
    { for(ord_t ni2 = discard->first_elem(ci1); ni2 < discard->end_elem(ci1); ni2++)
      { if(i2->mapped[ni2]) continue;

        (*gmb::node_morph)[i2->nodes[discard->element(ni2)]->original] = 
                                 i1->nodes[i1->discard.element(ni1)]->original; 
        i2->mapped[ni2] = true;

        if (!recurse_nodes(ci1, ni1 + 1)) return false;
        i2->mapped[ni2] = false;
       }
      return true;
     }
}


//generate mapping through dfs

template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::recurse_cells(ord_t ci1)
{
  if(ci1 == i1->discard.num_cells())
    { map_edges(i1->g, i2->g, *gmb::node_morph, *gmb::edge_morph);
      if(gmb::job == gmb::FINDALL)
        { morphism* m = new morphism(*gmb::node_morph, *gmb::edge_morph);
          gmb::morphisms->push_back(m);
         }
      else
        { if ((*gmb::callb)(morphism(*gmb::node_morph, *gmb::edge_morph)))
              gmb::job = gmb::FINDFIRST;      //break recursion
          return false;
         }
     }
  else
    recurse_nodes(ci1, i1->discard.first_elem(ci1));

  return true;
}


template<class graph_t, typename ord_t>
void conauto_fast<graph_t, ord_t>::fill_in_mapping(const partition& curr, 
                                                   const partition& pcurr)
{
  switch(gmb::job) {

  case gmb::FINDFIRST:
       if(gmb::node_morph)
       { for(ord_t ci = 0; ci < discard->num_cells(); ci++)
           for(ord_t ni = discard->first_elem(ci); ni < discard->end_elem(ci); ni++)
             (*gmb::node_morph)[i2->nodes[discard->element(ni)]->original] = i1->nodes[i1->discard.element(ni)]->original;

         map_edges(i1->g, i2->g, *gmb::node_morph, *gmb::edge_morph);
        }
       gmb::num_mappings++;
       break;

  case gmb::FINDALL:
       recurse_cells(0);
       gmb::num_mappings += i1->card_automorphism;
       break;

  case gmb::CARDINALITY:
       gmb::num_mappings += i1->card_automorphism*i1->card_automorphism_backtr;
       break;

  case gmb::ENUM:
       recurse_cells(0);
       gmb::num_mappings += i1->card_automorphism;
       break;
  }
}


template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::match(list_item s, const step& pcurr)
{
#ifdef COUNT_CALLS
  gmb::num_calls++;
#endif

  if(i1->seq.succ(s) == nil)
  { //end of sequence reached
    if(are_final_partitions_equivalent(i1, i2, pcurr))
      { //discard rest cells
        for(ord_t ci = 0; ci < pcurr.num_cells(); ci++)
          discard->copy_cell(pcurr, ci);
        discard->finish();
        fill_in_mapping(*i1->seq[s], pcurr);
        return true;
      }
    else
      return false;
   }
 
  step* curr = i1->seq[s], * next = i1->seq[i1->seq.succ(s)];
  step* pnext = new_step(i2->size);

  if(curr->pivot_kind == cb::BACKTR 
    || (full_backtracking && curr->pivot_kind == cb::NODE_BACKTR) 
    || curr->pivot_kind == cb::UNKNOWN)
  {
    for(ord_t ni = pcurr.first_elem(curr->pivot_cell); ni < pcurr.end_elem(curr->pivot_cell); ni++)
    { ord_t status = discard->num_cells();    //remember backtracking point

      refine_by_node(i2, pcurr, pcurr.element(ni), pcurr[curr->pivot_cell], 
                                                        *pnext, false, discard);
      if(next->compatible(*pnext, *i1->initial_comp))
      { if(match(i1->seq.succ(s), *pnext) && gmb::job == gmb::FINDFIRST)
        { delete_step(pnext);
          return true;
         }
       }

      pnext->clear();


      discard->clear_after(status);   //backtracking
    }

    delete_step(pnext);
    return false;
   }
   else
     { bool compatible = false;
       if(curr->pivot_kind == cb::CELL)
         { compatible = is_refine_by_set_compatible(i2, *next, pcurr, 
                                      pcurr[curr->pivot_cell], *pnext, discard);
          }
       else 
         if (curr->pivot_kind == cb::NODE || 
             (!full_backtracking && curr->pivot_kind == cb::NODE_BACKTR))
         {
           refine_by_node(i2, pcurr, 
                          pcurr.element(pcurr.first_elem(curr->pivot_cell)), 
                          pcurr[curr->pivot_cell], *pnext, false, discard);
           compatible = next->compatible(*pnext, *i1->initial_comp);
          }

       if(compatible)
         { bool res = match(i1->seq.succ(s), *pnext);      //recursion
           delete_step(pnext);
           return res;
          }
       else
         { delete_step(pnext);
           return false;
          }
      }
}



template<class graph_t, typename ord_t>
bool conauto_fast<graph_t, ord_t>::call_iso()
{
  initialize(i1, gmb::node_comp, gmb::edge_comp);
  initialize(i2, gmb::node_comp, gmb::edge_comp);
  step* initial1 = i1->seq.front(), * initial2 = i2->seq.front();

  bool compatible = initial1->compatible(*initial2, ccp);

#ifdef VERBOSE
  cout << *initial1 << endl << *initial2 << endl;
  cout << (compatible ? "compatible" : "not compatible") << endl;
#endif

  //no other choice
  full_backtracking = gmb::job == gmb::FINDALL || gmb::job == gmb::ENUM;  

  if(gmb::job == gmb::FINDALL)
  { gmb::node_morph = new node_morphism(i2->g);
    gmb::edge_morph = new edge_morphism(i2->g);
   }

  if(compatible)
  { generate_first_sequence_of_partitions(i1);
    if(!full_backtracking) search_for_automorphisms(i1);
    discard = new partition(i2->size);
    match(i1->seq.first_item(), *initial2);
    delete discard;
   }
        
  if(gmb::job == gmb::FINDALL)
  { delete gmb::node_morph;
    delete gmb::edge_morph;
   }

  step* s;
  forall(s, step_freelist) delete s;

  step_freelist.clear();
  return gmb::num_mappings > 0;
}

#undef VERBOSE


LEDA_END_NAMESPACE

#endif
