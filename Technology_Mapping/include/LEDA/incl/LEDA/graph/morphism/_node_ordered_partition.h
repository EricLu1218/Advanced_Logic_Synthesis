/*******************************************************************************
+
+  node_ordered_partition.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  partition of nodes where additional information is associated with each cell
+
*******************************************************************************/
#ifndef NODE_ORDERED_PARTITION_H
#define NODE_ORDERED_PARTITION_H


#include <LEDA/core/list.h>
#include <LEDA/graph/morphism/_compose.h>
#include <LEDA/graph/morphism/_node_disjoint_set.h>

LEDA_BEGIN_NAMESPACE


template<class graph_t, typename cell_info_t>
class node_ordered_partition : public list<compose<cell_info_t, node_disjoint_set<graph_t> >*>
{
public:
  typedef compose<cell_info_t, node_disjoint_set<graph_t> > cell_t;
        
private:
  typedef list<cell_t*> base;
  typedef typename graph_t::node node;

private:
  const graph_t& g;
  //main storage
  node_array<typename node_disjoint_set<graph_t>::node_info, graph_t> space;

public:

  node_ordered_partition() {}; //sn: added default constructor

  node_ordered_partition(const graph_t& _g);
 ~node_ordered_partition();
  const graph_t& get_graph() const { return g; }
  bool compatible(const node_ordered_partition& nop2, 
                  const leda_cmp_base<cell_info_t>& initial_comp) const;

  bool equivalent(const node_ordered_partition& nop2, 
                  const leda_cmp_base<cell_info_t>& initial_comp) const;

  cell_t* new_cell(const cell_info_t& ci) { return new cell_t(ci, space); }
  cell_t* new_cell() { return new cell_t(space); }
  cell_t* copy_cell(const cell_t& c2) 
  { cell_t* c = new_cell(); 
    node n; 
    forall_nodes(n, c2) c->append(n); 
    push_back(c); 
    return c; 
   }

  void clear();
  cell_t* corr_cell(const node_ordered_partition& p1a, cell_t const* c1a) const;

#ifdef VERBOSE
  void print(ostream& o) const;
#endif
};

template<class graph_t, typename cell_info_t>
node_ordered_partition<graph_t, cell_info_t>::node_ordered_partition(
                                           const graph_t& _g) : g(_g), space(_g)
{}


template<class graph_t, typename cell_info_t>
node_ordered_partition<graph_t, cell_info_t>::~node_ordered_partition()
{
  cell_t* c;
  forall(c, *this) delete c;
}

#ifdef VERBOSE
template<class graph_t, typename cell_info_t>
void node_ordered_partition<graph_t, cell_info_t>::print(ostream& o) const
{ list_item si;
   forall_items(si, *this)
   { o << *((cell_info_t*)inf(si)) << '\t';
     o << *((node_disjoint_set<graph_t>*)inf(si)) << endl;
    }
}


template<class graph_t, typename cell_info_t>
ostream& operator<<(ostream& o, 
                    const node_ordered_partition<graph_t, cell_info_t>& nsp)
{ nsp.print(o);
  return o;
}
#endif

template<class graph_t, typename cell_info_t>
bool node_ordered_partition<graph_t, cell_info_t>::compatible(
                      const node_ordered_partition<graph_t, cell_info_t>& nop2,
                      const leda_cmp_base<cell_info_t>& initial_comp) const
{
  if(base::size() != nop2.base::size()) return false;

   list_item si1; 
   list_item si2 = nop2.first_item();
   forall_items(si1, *this)
   { cell_t* c1 = base::inf(si1), * c2 = nop2.inf(si2);
     if(c1->size() != c2->size()) return false;
     if(initial_comp(*c1, *c2) != 0) return false;
     si2 = nop2.succ(si2);
    }

   return true;
}


template<class graph_t, typename cell_info_t>
bool node_ordered_partition<graph_t, cell_info_t>::equivalent(const node_ordered_partition& nop2, const leda_cmp_base<cell_info_t>& initial_comp) const
{
  if(base::size() != nop2.size()) return false;

  list_item si1, si2 = nop2.first_item();
  forall_items(si1, *this)
  { cell_t* c1 = base::inf(si1), * c2 = nop2.inf(si2);
    if(c1->size() != c2->size()) return false;
    if(initial_comp(*c1, *c2) != 0) return false;

    node n; 
    node m = c2->first_node();
    forall_nodes(n, *c1)
    { if(n != m) return false;
      m = c2->next_node(m);
     }

    si2 = nop2.succ(si2);
   }

  return true;
}


template<class graph_t, typename cell_info_t>
void node_ordered_partition<graph_t, cell_info_t>::clear()
{ cell_t* c;
  forall(c, *this) delete c;
  list<cell_t*>::clear();
}

template<class graph_t, typename cell_info_t>
compose<cell_info_t, node_disjoint_set<graph_t> >* 
node_ordered_partition<graph_t, cell_info_t>::corr_cell(
                   const node_ordered_partition& p1a, cell_t const* c1a) const
{
  list_item li1b = base::first_item();
  cell_t* c;
  forall(c, p1a)
  { if(c == c1a) return base::inf(li1b);
    li1b = base::succ(li1b);
   }
  return NULL;
}

LEDA_END_NAMESPACE

#endif
