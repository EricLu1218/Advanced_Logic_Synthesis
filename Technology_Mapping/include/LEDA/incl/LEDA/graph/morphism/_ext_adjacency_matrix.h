/*******************************************************************************
+
+  _ext_adjacency_matrix.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  An extended adjacency matrix (not binary, but two bits to signify connection 
+  in each direction)
+
*******************************************************************************/

#ifndef EXT_ADJACENCY_MATRIX_H
#define EXT_ADJACENCY_MATRIX_H

#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

typedef int degree_t;
typedef unsigned char adj_t;

template<class graph_t>
class ext_adjacency_matrix
{
  typedef typename graph_t::node node;
  const graph_t& g;
  degree_t count;
  adj_t* a;
  static const adj_t mask_in  = 0x1;
  static const adj_t mask_out = 0x2;

public:

  ext_adjacency_matrix(const graph_t& _g);
 ~ext_adjacency_matrix();

  adj_t& adj(int i, int j) const { return a[j * count + i]; }

  char sym(int i, int j) const 
  { if(a[j * count + i] == 0) return '.'; 
    else return (char)('0' + a[j * count + i]); }

  adj_t& operator()(int i, int j)   const { return a[j * count + i]; }
  adj_t& operator()(node i, node j) const 
                                  { return a[g.index(j) * count + g.index(i)]; }

  int get_count() const { return count; }
};



template<class graph_t>
ext_adjacency_matrix<graph_t>::ext_adjacency_matrix(const graph_t& _g) : g(_g)
{
  count = g.max_node_index() + 1;

  a = new adj_t[count * count];
  for(int i = 0; i < count * count; i++) a[i] = 0;

  typename graph_t::node n;
  typename graph_t::edge e;
  int ne = 0;
  forall_nodes(n, g)
  { forall_out_edges_graph_t(g, e, n)
    { adj(g.index(n), g.index(g.opposite(e, n))) |= mask_out;
      adj(g.index(g.opposite(e, n)), g.index(n)) |= mask_in;
      ne++;
     }
   }
}


template<class graph_t>
ext_adjacency_matrix<graph_t>::~ext_adjacency_matrix() { delete[] a; } 


template<class graph_t>
ostream& operator<<(ostream& o, const ext_adjacency_matrix<graph_t>& am)
{ for(int i = 0; i < am.get_count(); i++)
  { for(int j = 0; j < am.get_count(); j++) o << am.sym(i, j) << " ";
    o << endl;
   }
  return o;
}


LEDA_END_NAMESPACE

#endif
