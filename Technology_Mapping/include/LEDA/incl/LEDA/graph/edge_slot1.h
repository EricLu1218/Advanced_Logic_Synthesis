/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  edge_slot1.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:13 $

#ifndef LEDA_EDGE_SLOT1_H
#define LEDA_EDGE_SLOT1_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520266
#include <LEDA/internal/PREAMBLE.h>
#endif


#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE


template <class E, class graph_t=graph, int slot=-1>
class edge_slot1 {


const graph_t* gptr;

public:

typedef typename graph_t::edge edge;
typedef typename graph_t::edge index;
//typedef graph_t graph;
typedef E value_type;


void init(const graph_t& G, const E& x) 
{ gptr = &G;
  edge e; 
  forall_edges(e,G) 
     new((void*)(&(e->data(graph_t::e_base_sz+slot))),0) E(x);
}

void init(const graph_t& G, int, const E& x) // compatibility with edge_array 
{ init(G,x); }

void init(const graph_t& G)
{ gptr = &G;
  if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
  { edge e; 
    forall_edges(e,G) 
       new((void*)(&(e->data(graph_t::e_base_sz+slot))),0) E;
   }
}

edge_slot1()                              { gptr = 0;  }
edge_slot1(const graph_t& G)              { init(G);   }
edge_slot1(const graph_t& G, const E& x)  { init(G,x); }

~edge_slot1()
{ if (gptr)
  { if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
    { edge e; 
      forall_edges(e,*gptr) 
         //DESTROY_WORD(E,((E*)(&(e->data(graph_t::e_base_sz+slot))))); 
         ((E*)(&(e->data(graph_t::e_base_sz+slot))))->~E();
     }
  }
}

const E& operator[](edge e) const
{ return *(const E*)&(e->data(graph_t::e_base_sz+slot)); }

E& operator[](edge e)       
{ return *(E*)&(e->data(graph_t::e_base_sz+slot)); }


bool use_edge_data(const graph_t& G, const E& x) { init(G,x); return true; }
bool use_edge_data(const graph_t&) { return true; }


};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520266
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif


