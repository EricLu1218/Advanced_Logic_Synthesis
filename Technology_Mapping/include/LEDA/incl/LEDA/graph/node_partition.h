/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  node_partition.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:13 $

#ifndef LEDA_NODE_PARTITION_H
#define LEDA_NODE_PARTITION_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520196
#include <LEDA/internal/PREAMBLE.h>
#endif



//------------------------------------------------------------------------------
// node partitions 
//------------------------------------------------------------------------------

#include <LEDA/graph/graph.h>

/*{\Manpage {node_partition} {} {Node Partitions}}*/

#include <LEDA/core/partition.h>

LEDA_BEGIN_NAMESPACE

class __exportC node_partition : private partition
{
/*{\Mdefinition
An instance $P$ of the data type $node\_partition$ is a partition of the nodes
of a graph $G$.}*/

node_array<partition_item> ITEM;

public:

void init(const graph& G);

/*{\Mcreation P }*/

 node_partition(const graph& G) : ITEM(G,nil) { init(G); }
/*{\Mcreate creates a |\Mname| |\Mvar| containing for every node $v$ in $G$ a 
            block $\{v\}$.}*/

~node_partition()               {}   

/*{\Moperations 1.2 5.0}*/

int  same_block(node v, node w)   
{ return partition::same_block(ITEM[v],ITEM[w]); }
/*{\Mopl      returns true if $v$ and $w$ belong to the
              same block of |\Mvar|, false otherwise.}*/


void union_blocks(node v, node w) 
{ partition::union_blocks(ITEM[v],ITEM[w]); } 
/*{\Mopl      unites the blocks of |\Mvar| containing nodes
	      $v$ and $w$.}*/


void split(const list<node>& L);
/*{\Mop       makes all nodes in $L$ to singleton blocks.\\
              \precond     $L$ is a union of blocks. }*/


void make_rep(node v) { partition::inf(ITEM[v]) = v; }
/*{\Mop       makes v the canonical representative of the block 
              containing $v$.}*/

node find(node v) 
{ return node(partition::inf(partition::find(ITEM[v]))); }
/*{\Mop       returns a canonical representative node of 
	      the block that contains node $v$.}*/

int size(node v) 
{ return partition::size(ITEM[v]); }
/*{\Mop       returns the size of  
	      the block that contains node $v$.}*/

int number_of_blocks() 
{ return partition::number_of_blocks(); }
/*{\Mop       returns the number of blocks of |\Mvar|. }*/



node operator()(node v) { return find(v); }
/*{\Mfunop    returns |\Mvar|.find($v$). }*/

};

/*{\Mimplementation
A node partition for a graph $G$ is implemented by a combination of a 
partition $P$ and a node array of $partition\_item$ associating with 
each node in $G$ a partition item in $P$. Initialization takes linear time,
union\_blocks takes time $O(1)$ (worst-case), and same\_block and find take 
time $O(\alpha(n))$ (amortized).  The cost of a split is proportional to the 
cost of the blocks dismantled. The space requirement is $O(n)$, where $n$ 
is the number of nodes of $G$. }*/


#if LEDA_ROOT_INCL_ID == 520196
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
