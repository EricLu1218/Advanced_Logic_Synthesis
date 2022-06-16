/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  node_pq0.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:13 $

#ifndef LEDA_NODE_PQ_H
#define LEDA_NODE_PQ_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520267
#include <LEDA/internal/PREAMBLE.h>
#endif


//------------------------------------------------------------------------------
// node priority queues
//------------------------------------------------------------------------------

#include <LEDA/graph/graph.h>
#include <LEDA/core/impl/bin_heap.h>

LEDA_BEGIN_NAMESPACE

#define PRIO_IMPL bin_heap


/*{\Manpage {node_pq0} {P} {Node Priority Queues}}*/

template <class P,class graph_t=graph,class prio_impl=bin_heap,
                                      class ItemArray=node_array<GenPtr,graph_t> >
class node_pq0 : private prio_impl {

/*{\Mdefinition
An instance $Q$ of the parameterized data type |\Mname| is a partial
function from the nodes of a graph $G$ to a linearly ordered type $P$
of priorities. The priority of a node is sometimes called the information
of the node. For every graph $G$ only one |\Mname| may be used and every node
of $G$ may be contained in the queue at most once (cf. section 
\ref{Priority Queues} for general priority queues). }*/

typedef typename graph_t::node node;
typedef typename prio_impl::item prio_item;


const graph_t& gr;

int  key_type_id() const { return LEDA_TYPE_ID(P); }

int  cmp(GenPtr x, GenPtr y) const { return LEDA_COMPARE(P,x,y); }

//void print_key(GenPtr x)     const { LEDA_PRINT(P,x,cout);    }
//void print_inf(GenPtr x)     const { gr.print_node(node(x)); }

void clear_key(GenPtr& x)    const { LEDA_CLEAR(P,x); }
void copy_key(GenPtr& x)     const { LEDA_COPY(P,x);  }

public:

/*{\Mcreation Q }*/

node_pq0(const graph_t& G, int sz) : prio_impl(sz),gr(G) { }
 
node_pq0(const graph_t& G) : prio_impl(G.number_of_nodes()),gr(G) { }
/*{\Mcreate creates an instance $Q$ ot type |\Mname| for the nodes of graph $G$
    with $dom(Q)=\emptyset$.}*/

~node_pq0() { clear(); }


/*{\Moperations 1.3 4.7}*/

void insert(node v, const P& x)
{ prio_impl::insert(leda_cast(x),v);}
/*{\Mop          adds the node $v$ with priority $x$ to |\Mvar|.\\
                 \precond $v \not\in dom(Q)$.}*/


node find_min() const
{ prio_item it = prio_impl::find_min();
  return (it) ? (node)prio_impl::inf(it) : (node)0; 
 }
/*{\Mop          returns a node with minimal priority 
                 (nil if |\Mvar| is empty).}*/

node del_min()   
{ node v = find_min(); 
  if (v) prio_impl::del_min(); 
  return v;
}
/*{\Mop          removes a node with minimal priority from |\Mvar| 
                 and returns it (nil if |\Mvar| is empty).}*/

node del_min(P& x)   
{ prio_item it = prio_impl::find_min();
  if (it == nil) return nil;
else
{ node v = (node)prio_impl::inf(it);
  x = LEDA_CONST_ACCESS(P,prio_impl::key(it));
  prio_impl::del_min(); 
  return v;
}
}
/*{\Mop          as above, in addition the priority of the removed
                 node is assigned to |x|. }*/



void clear()  { prio_impl::clear(); }
/*{\Mop          makes $Q$ the empty node priority queue.}*/


int size()   const { return prio_impl::size(); }
/*{\Mop          returns $\Labs{dom(Q)}$. }*/

int empty()  const { return prio_impl::empty(); }
/*{\Mop          returns true if |\Mvar| is the empty node
		 priority queue, false otherwise.}*/


};

/*{\Mimplementation
Node priority queues are implemented by binary heaps and node arrays.
Operations insert, del\_node, del\_min, decrease\_p take time $O(\log m)$, 
find\_min and empty take time $O(1)$ and clear takes time $O(m)$, where 
$m$ is the size of $Q$. The space requirement is $O(n)$, where $n$ is the 
number of nodes of $G$.}*/


#if LEDA_ROOT_INCL_ID == 520267
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
