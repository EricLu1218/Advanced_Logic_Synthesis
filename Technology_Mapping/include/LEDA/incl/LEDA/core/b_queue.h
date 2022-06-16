/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  b_queue.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.5 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_BQUEUE_H
#define LEDA_BQUEUE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520448
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// bounded queues
//------------------------------------------------------------------------------

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {b_queue} {E} {Bounded Queues}}*/

template<class E> 

class b_queue  
{
/*{\Mdefinition
An instance $Q$ of the parameterized data type |\Mname| is a queue
(see section \ref{Queues}) of bounded size.}*/


E* first;     // first element of array
E* stop;      // one position behind last element of array
E* start;     // current start of queue (wraps around)
E* end;       // one position behind end of queue (wraps around)

public:										

typedef E* item;


/*{\Mcreation Q}*/


b_queue(int n) 
/*{\Mcreate creates an instance |\Mvar| of type |\Mname| that can hold up to $n$ 
           elements.  |\Mvar| is initialized with the empty queue.}*/
{
#if !defined(LEDA_CHECKING_OFF)
  if (n<1) error_handler(88,"_b_queue: bad size");
#endif
  first = new E[n+1];
  if (first==0) error_handler(88,"_b_queue: out of memory");
  stop  = first+n+1;
  start = end = first; 
}



virtual ~b_queue() { delete [] first; }

/*{\Moperations 2 5}*/

const E& top() const
{
#if !defined(LEDA_CHECKING_OFF)
  if (start==end) error_handler(88, "_b_queue empty");
#endif
  return *start;
}

/*{\Mop  returns the front element of |\Mvar|.\\
         \precond $Q$ is not empty.}*/


const E& pop()
{
#if !defined(LEDA_CHECKING_OFF)
 if (start==end) error_handler(88, "_b_queue underflow");
#endif
  E* p = start++;
  if (start == stop) start = first;
  return *p;
}

/*{\Mop  deletes and returns the front element of |\Mvar|.\\
         \precond $Q$ is not empty.}*/

void del_top()
{
#if !defined(LEDA_CHECKING_OFF)
 if (start==end) error_handler(88, "_b_queue underflow");
#endif
  if (++start == stop) start = first;
}
/*{\Mop  deletes the front element of |\Mvar|.\\
         \precond $Q$ is not empty.}*/

void append(const E& x)
{ *end++ = x;
  if (end == stop) end = first;
#if !defined(LEDA_CHECKING_OFF)
  if (start==end) error_handler(88, "_b_queue overflow");
#endif
}

/*{\Mop   appends $x$ to the rear end of |\Mvar|.\\
	  \precond $Q$.size()$ < n$.}*/

void push(const E& x)
{ if (start == first) start = stop;
  *--start = x;
#if !defined(LEDA_CHECKING_OFF)
  if (start==end) error_handler(88, "_b_queue overflow");
#endif
}
/*{\Mop   inserts $x$ at the front end of |\Mvar|.\\
	  \precond $Q$.size()$ < n$.}*/


void clear() { start = end = first; }
/*{\Mop    makes |\Mvar| the empty queue.}*/


int size() const 
{ int s = int(end-start);
  return (s<0) ? int(stop-first+s) : s;
}
/*{\Mop    returns the size of |\Mvar|.}*/

int length() const { return size(); }
/*{\Mop    returns the size of |\Mvar|.}*/


bool empty() const { return start == end; }
/*{\Mop    returns true if |\Mvar| is empty, false otherwise.}*/


// iteration (s.n. 2006)

const E& inf(item p) const { return *p; }

item first_item() const { return (start == end) ? 0 : start; }

item next_item(item p) const
{ if (p ==0) return 0;
  if (++p == stop) p = first;
  return (p == end) ? 0 : p;
}

/*{\Mtext \headerline{Iteration}

{\bf forall}($x,Q$)       
$\{$ ``the elements of $Q$ are successively assigned to $x$'' $\}$ 

}*/


};


/*{\Mimplementation
Bounded queues are implemented by circular arrays. All operations take 
time $O(1)$. The space requirement is $O(n)$.}*/

#if LEDA_ROOT_INCL_ID == 520448
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
