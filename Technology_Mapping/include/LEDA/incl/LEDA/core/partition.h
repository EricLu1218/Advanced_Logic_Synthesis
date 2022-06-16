/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  partition.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_Partition_H
#define LEDA_Partition_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520525
#include <LEDA/internal/PREAMBLE.h>
#endif


//------------------------------------------------------------------------------
// partition   (union find)
//------------------------------------------------------------------------------

#include <LEDA/system/basic.h>
#include <LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE


/*{\Manpage {partition} {} {Partitions} }*/

class __exportC partition_node {
/*{\Mdefinition
An instance  $P$ of the data type |partition| consists of a finite set of
items (|partition_item|) and a partition of this set
into blocks.}*/

friend class __exportC partition;

partition_node* father;
partition_node* next;
int size;
GenPtr info;

public:

partition_node(GenPtr x, partition_node* n)  
{ 
  father=0; size=1; info=x; next = n; 
 }

  LEDA_MEMORY(partition_node)

};


// a partition item is a pointer to a partition node:

typedef partition_node* partition_item;



class __exportC partition {

virtual void clear_inf(GenPtr&) const {}

partition_item used_items;                 // List of used partition items
int block_count;

public:

/*{\Mcreation P }*/

partition() { used_items = 0; block_count = 0; }  
/*{\Mcreate creates an instance $P$ of type |partition| and initializes it to 
            the empty partition.}*/

virtual ~partition() { clear(); }

/*{\Moperations 3 4}*/

partition_item make_block(GenPtr x) 
{ block_count++;
  used_items = new partition_node(x,used_items); 
  return used_items; 
 }

partition_item make_block() 
{ return make_block(nil);
}
/*{\Mop      returns a new |partition_item| $it$ and adds
	     the block |{it}| to partition $P$.}*/

partition_item find(partition_item p) const;
/*{\Mopl     returns a canonical item of the block that
	     contains item $p$, i.e., iff |P.same_block(p,q)|
	     then |P.find(p)| and |P.find(q)| return the same item.\\
	     \precond $p$ is an item in $P$. }*/

int size(partition_item p) const
{ return find(p)->size; }
/*{\Mopl returns the size of the block containing $p$.}*/

int number_of_blocks() const { return block_count; }
/*{\Mopl returns the number of blocks in |P|.}*/

bool  same_block(partition_item p, partition_item q) const
{ return find(p) == find(q); }
/*{\Mopl      returns true if $p$ and $q$ belong to the same
	      block of partition $P$.\\
	      \precond $p$ and $q$ are items in $P$.}*/

void  union_blocks(partition_item p, partition_item q);
/*{\Mopl      unites the blocks of partition $P$ containing
	     items $p$ and $q$.\\
	     \precond $p$ and $q$ are items in $P$.}*/

void split(const list<partition_item>& L);
/*{\Mop     turns all items in $L$ to singleton blocks.\\
            \precond     $L$ is a union of blocks. }*/



void clear();                      // deletes all used items


partition_item first_item() const  { return used_items;  }
partition_item next_item(partition_item it) const  { return it ? it->next : 0; }


protected:

GenPtr& inf(partition_item p) const { return find(p)->info; }

void reset(partition_item p);

};


/*{\Mimplementation
Partitions are implemented by the union find algorithm with weighted union
and path compression (cf.~\cite{T83}).  Any sequence of $n$ make\_block and 
$m \ge n$ other operations (except for |split|) takes time $O(m\ \alpha(m,n))$. 
The cost of a split is proportional to the size
of the blocks dismantled.  }*/


/*{\Mexample
    Spanning Tree Algorithms (cf. section \ref{Graph Algorithms}).}*/


/*{\Mtext \newpage}*/

//------------------------------------------------------------------------------
// Partition  (parameterized partitions)
//-----------------------------------------------------------------------------

template <class E>

class Partition : private partition {

/*{\Manpage {Partition} {E} {Parameterized Partitions} }*/

/*{\Mdefinition
An instance  $P$ of the data type |\Mname| consists of a finite set of
items (|partition_item|) and a partition of this set
into blocks. Each item has an associated information of type |E|.}*/

void clear_inf(GenPtr& x) const { LEDA_CLEAR(E,x); }

public:

/*{\Mcreation P }*/

Partition()  {}  
/*{\Mcreate creates an instance $P$ of type |\Mname| and initializes it to 
            the empty partition.}*/

~Partition() { clear(); }


/*{\Moperations 3 4}*/

partition_item make_block(const E& x) 
{ return partition::make_block(leda_copy(x)); }
/*{\Mop      returns a new |partition_item| $it$, adds
	     the block |{it}| to partition $P$, and associates |x| with |it|.}*/

partition_item find(partition_item p) const
{ return partition::find(p); }
/*{\Mopl     returns a canonical item of the block that
	     contains item $p$, i.e., iff |P.same_block(p,q)| 
	     then |P.find(p)| and |P.find(q)| return the same item.\\
	     \precond $p$ is an item in $P$. }*/

int size(partition_item p) const
{ return partition::size(p); }
/*{\Mopl returns the size of the block containing $p$.}*/


bool  same_block(partition_item p, partition_item q) const
{ return partition::same_block(p,q); }
/*{\Mopl      returns true if $p$ and $q$ belong to the same
	      block of partition $P$.\\
	      \precond $p$ and $q$ are items in $P$.}*/

void  union_blocks(partition_item p, partition_item q)
{ partition::union_blocks(p,q); }
/*{\Mopl      unites the blocks of partition $P$ containing
	     items $p$ and $q$.\\
	     \precond $p$ and $q$ are items in $P$.}*/


void  split(const list<partition_item> & L) { partition::split(L); }
/*{\Mopl     turns all items in $L$ to singleton blocks. \\
	     \precond $L$ is a union of blocks}*/


const E& inf(partition_item it)  const
{ return LEDA_CONST_ACCESS(E,partition::inf(it)); }
/*{\Mop      returns the information associated with |it|.}*/



void  change_inf(partition_item it, const E& x) 
{ LEDA_ACCESS(E,partition::inf(it)) = x; }
/*{\Mop      changes the information associates with |it| to |x|.}*/

void clear() { partition::clear(); }


partition_item first_item() const  
{ return partition::first_item();  }

partition_item next_item(partition_item it) const
{ return partition::next_item(it);  }

};



#if LEDA_ROOT_INCL_ID == 520525
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
