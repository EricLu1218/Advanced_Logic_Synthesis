/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  b_heap.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:48:11 $

#ifndef LEDA_BHEAP_H
#define LEDA_BHEAP_H

//------------------------------------------------------------------------------
// b_heap: bounded heaps with integer keys in [a..b]
//------------------------------------------------------------------------------

#include <LEDA/system/basic.h>
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

class __exportC b_heap_node {

friend class __exportC b_heap;
friend void print_b_heap_item(b_heap_node*);

int key;
GenPtr info;
list_item loc;

b_heap_node(int k, GenPtr i ) 
{ 
  key = k; info = i; loc = 0; }

  LEDA_MEMORY(b_heap_node)

};

typedef b_heap_node* b_heap_item;

typedef list<b_heap_item>* b_heap_bucket;


class __exportC b_heap {

    int min;
    int max;
    int low;
    int high;
    
    array<b_heap_bucket>  T;


public:

b_heap(int a, int b);
~b_heap() { clear(); }
b_heap_item insert(int key, GenPtr info) ;

b_heap_item find_min();
b_heap_item find_max();
GenPtr del_min();
GenPtr del_max();
b_heap_item decrease_key(b_heap_item it, int k);
b_heap_item increase_key(b_heap_item it, int k);

void delete_item(b_heap_item it);
void clear();

GenPtr inf(b_heap_item it) { return it->info; }
int key(b_heap_item it)  { return it->key; }
int empty()              { return (find_min()==0) ? true : false; }

void print();

};

LEDA_END_NAMESPACE

#endif

