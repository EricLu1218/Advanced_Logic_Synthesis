/*******************************************************************************
+
+  _node_comparator.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  Comparator class to sort nodes by out-degree asc, in-degree asc.
+ 
*******************************************************************************/
#ifndef NODE_COMPARATOR_H
#define NODE_COMPARATOR_H


#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE


//compare two nodes by out-degree, in-degree
template<class graph_t>
class node_comparator : public leda_cmp_base<typename graph_t::node>
{
  const graph_t* g;

public:
  node_comparator(const graph_t* _g) : g(_g) {}

  int operator()(const typename graph_t::node& n1, 
                 const typename graph_t::node& n2) const
  {
    if (g->outdeg(n1) < g->outdeg(n2)) return -1;
    else if (g->outdeg(n1) > g->outdeg(n2)) return +1;
    else if (g->indeg(n1) < g->indeg(n2)) return -1;
    else if (g->indeg(n1) > g->indeg(n2)) return +1;
    else return 0;
   }
};


LEDA_END_NAMESPACE

#endif
