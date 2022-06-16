/*******************************************************************************
+
+  ll_item.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  An item of an ultra lightweight doubly-linked list
+ 
*******************************************************************************/

#ifndef LL_ITEM_H
#define LL_ITEM_H

#include <LEDA/system/basic.h>

// LEDA_BEGIN_NAMESPACE

template<typename T>
class ll_item
{
public:
  ll_item() { clear(); }      //construct self loop

  void clear() { pred = this; succ = this; content = NULL; }

  //extract from list, but remember position
  void draw_back() { pred->succ = succ; succ->pred = pred; }       

  //insert into former position
  void infiltrate() { pred->succ = this; succ->pred = this; }      

  void append(ll_item<T>& i) 
  { //"this" must be list head
    pred->succ = &i; 
    i.pred = pred; 
    i.succ = this; 
    pred = &i; 
   } 

  bool empty() 
  { //"this" must be list head
    return pred == this /*or succ == this */; 
   } 

  void remove() 
  { //remove permanently
    pred->succ = succ; 
    succ->pred = pred; 
    succ = NULL; 
    pred = NULL; 
    content = NULL; 
   }

  ll_item* succ;
  ll_item* pred;
  T content;

  LEDA_MEMORY(ll_item)
};

// LEDA_END_NAMESPACE

#endif
