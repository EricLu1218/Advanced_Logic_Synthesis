/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  node_slot1.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.10 $  $Date: 2007/02/28 07:44:13 $

#ifndef LEDA_NODE_SLOT1_H
#define LEDA_NODE_SLOT1_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520192
#include <LEDA/internal/PREAMBLE.h>
#endif


#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE


template <class E, class graph_t=graph, int slot=-1>
class node_slot1 {


const graph_t* gptr;

public:

typedef typename graph_t::node node;
typedef typename graph_t::node index;
//typedef graph_t graph;
typedef E value_type;


void init(const graph_t& G, const E& x) 
{ gptr = &G;
  node v; 
  forall_nodes(v,G) 
     new((void*)(&(v->data(graph_t::n_base_sz+slot))),0) E(x);
}

void init(const graph_t& G, int, const E& x) // compatibility with node_array 
{ init(G,x); }

void init(const graph_t& G)
{ gptr = &G;
  if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
  { node v; 
    forall_nodes(v,G) 
       new((void*)(&(v->data(graph_t::n_base_sz+slot))),0) E;
   }
}

node_slot1()                              { gptr = 0;  }
node_slot1(const graph_t& G)              { init(G);   }
node_slot1(const graph_t& G, const E& x)  { init(G,x); }

~node_slot1()
{ 
  if (gptr)
  { if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
    { node v; 
      forall_nodes(v,*gptr) 
         //DESTROY_WORD(E,((E*)(&(v->data(graph_t::n_base_sz+slot))))); 
         ((E*)(&(v->data(graph_t::n_base_sz+slot))))->~E(); 
         
     }
   }
}

const E& operator[](node v) const
{ return *(const E*)&(v->data(graph_t::n_base_sz+slot)); }

E& operator[](node v)       
{ return *(E*)&(v->data(graph_t::n_base_sz+slot)); }


bool use_node_data(const graph_t& G, const E& x) { init(G,x); return true; }
bool use_node_data(const graph_t&) { return true; }


};


template <class E, class graph_t, int slot, int offset>
class node_slot2 {

const graph_t* gptr;

public:

typedef typename graph_t::node node;
typedef typename graph_t::node index;
//typedef graph_t graph;
typedef E value_type;

void init(const graph_t& G, const E& x) 
{ gptr = &G;
  node v; 
  forall_nodes(v,G) 
     new((void*)(&(v->data(graph_t::n_base_sz+slot))),0) E(x);
}

void init(const graph_t& G, int, const E& x) // compatibility with node_array 
{ init(G,x); }

void init(const graph_t& G)
{ gptr = &G;
  if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
  { node v; 
    forall_nodes(v,G) 
       new((void*)(&(v->data(graph_t::n_base_sz+slot))),0) E;
   }
}

node_slot2()                              { gptr = 0;  }
node_slot2(const graph_t& G)              { init(G);   }
node_slot2(const graph_t& G, const E& x)  { init(G,x); }

~node_slot2()
{ if (gptr)
  { if (LEDA_TYPE_ID(E) != INT_TYPE_ID && LEDA_TYPE_ID(E) != DOUBLE_TYPE_ID)
    { node v; 
      forall_nodes(v,*gptr) 
         //DESTROY_WORD(E,((E*)(&(v->data(graph_t::n_base_sz+slot))))); 
         ((E*)(&(v->data(graph_t::n_base_sz+slot))))->~E(); 
     }
  }
}

const E& operator[](node v) const
{ const unsigned short* p = &(v->data(graph_t::n_base_sz+slot)); 
  return *(const E*)(p+offset);
}


E& operator[](node v)       
{ unsigned short* p = &(v->data(graph_t::n_base_sz+slot)); 
  return *(E*)(p+offset);
}



bool use_node_data(const graph_t& G, const E& x) { init(G,x); return true; }
bool use_node_data(const graph_t&) { return true; }


};



LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520192
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif


