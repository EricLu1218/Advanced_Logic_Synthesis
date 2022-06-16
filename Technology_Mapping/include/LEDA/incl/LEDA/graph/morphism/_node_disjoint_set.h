/*******************************************************************************
+
+  node_disjoint_set.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  A set of nodes that shares a node_array with other sets of the same graph.
+  All those sets must be disjoint.
+
*******************************************************************************/
#ifndef NODE_DISJOINT_SET_H
#define NODE_DISJOINT_SET_H

#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

//using std::ostream;

template<class graph_t>
class node_disjoint_set
{

public:

  typedef typename graph_t::node node;

  class node_info
  { public:
    node_info() { succ = nil; }
    node succ;
   };


private:
  node_array<node_info, graph_t>& space;
  node head, tail;
  int count;

public:
  node_disjoint_set(node_array<node_info, graph_t>& _space);

  void append(const node& n) 
  { /*assert(!contains(n));*/ 
    if(tail) space[tail].succ = n; 
    else head = n; 
    tail = n; 
    space[n].succ = nil; 
    count++; 
   }

  node first_node()      const { return head; }
  node next_node(node n) const { return space[n].succ; }
  node stop_node()       const { return nil; }
  int size()             const { return count; }

  const graph_t& get_graph() const { return space.get_graph(); }
  node_array<node_info, graph_t>& get_space() { return space; }
  void print(ostream& o) const;

  LEDA_MEMORY(node_disjoint_set)
};


template<class graph_t>
node_disjoint_set<graph_t>::node_disjoint_set(node_array<node_info, graph_t>& _space) : space(_space)
{
  head = nil;
  tail = nil;
  count = 0;
}


template<class graph_t>
void node_disjoint_set<graph_t>::print(ostream& o) const
{ node n;
  forall_nodes(n, *this) o << space.get_graph().index(n) << " ";
}


template<class graph_t>
ostream& operator<<(ostream& o, const node_disjoint_set<graph_t>& nds)
{ nds.print(o);
  return o;
}


LEDA_END_NAMESPACE

#endif

