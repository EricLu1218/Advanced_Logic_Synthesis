/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  slist.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_SLIST_H
#define LEDA_SLIST_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520450
#include <LEDA/internal/PREAMBLE.h>
#endif


/*{\Manpage {slist} {E} {Singly Linked Lists}}*/

#include <LEDA/core/impl/_slist.h>

LEDA_BEGIN_NAMESPACE

template <class E>

class slist : public SLIST {


/*{\Mdefinition
An instance $L$ of the parameterized data type |\Mname| is a sequence of items
(|slist_item|). Each item in $L$ contains an element of data type $E$, called
the element type of $L$. The number of items in $L$ is called the length of $L$. If $L$ has length zero it is called the empty list. In the sequel $\Litem{x}$ is
used to denote a list item containing the element $x$ and $L[i]$ is used to
denote the contents of list item $i$ in $L$.}*/

void clear_el(GenPtr& x) const { LEDA_CLEAR(E,x); }
void copy_el(GenPtr& x)  const { LEDA_COPY(E,x); }
int  el_type_id()        const { return LEDA_TYPE_ID(E); }

GenPtr buf;

public:

/*{\Mtypes 5}*/ 

typedef slist_item item;

typedef E value_type;
/*{\Mtypemember the value type.}*/


// additional type definitions ...
typedef value_type*         pointer;
typedef const value_type*   const_pointer;
typedef value_type&         reference;
typedef const value_type&   const_reference;
typedef int                 size_type;
typedef int                 difference_type;


/*{\Mcreation L }*/
 
slist() { LEDA_CREATE(E,buf); }
/*{\Mcreate creates  an instance |\Mvar| of type |\Mname| and initializes it to
            the empty list.}*/
 
slist(const E& x) : SLIST(leda_cast(x)) { LEDA_CREATE(E,buf); }
/*{\Mcreate creates  an instance |\Mvar| of type |\Mname| and initializes it to
            the one-element list $\Litem{x}$. }*/

 slist(const slist<E>& a) : SLIST((SLIST&)a) { LEDA_CREATE(E,buf); }

 virtual ~slist() { SLIST::clear(); LEDA_CLEAR(E,buf); }

 slist<E>& operator=(const slist<E>& a) { SLIST::operator=(a);  return *this; }

 

/*{\Moperations 2 5 }*/

int length() const {return SLIST::length();}
/*{\Mop      returns the length of $L$.}*/

int size() const {return SLIST::length();}
/*{\Mop      returns $L$.length().}*/

bool empty() const {return SLIST::empty();}
/*{\Mop      returns true if $L$ is empty, false otherwise.}*/

slist_item first() const {return SLIST::first();}
/*{\Mop      returns the first item of $L$.}*/

slist_item last() const {return SLIST::last();}
/*{\Mop      returns the last item of $L$.}*/

slist_item succ(slist_item it) const {return SLIST::succ(it);}
/*{\Mop      returns the successor item of item $it$, nil if
	     $it=L$.last().\\
	     \precond $it$ is an item in $L$.}*/

slist_item cyclic_succ(slist_item it) const {return SLIST::cyclic_succ(it);}
/*{\Mop      returns the cyclic successor of item $it$, i.e.,
	     $L$.first() if $it = L$.last(), $L$.succ($it$) otherwise.}*/

const E& contents(slist_item it) const 
{ return LEDA_CONST_ACCESS(E,SLIST::entry(it)); }
/*{\Mop      returns the contents $L[it]$ of item $it$.\\
	     \precond $it$ is an item in $L$.}*/

const E& inf(slist_item it) const { return contents(it); }
/*{\Mop      returns $L$.contents($it$).\\
             \precond $it$ is an item in $L$.}*/


const E& head() const { return LEDA_CONST_ACCESS(E,SLIST::head()); }
/*{\Mop      returns the first element of $L$, i.e. the contents
	     of $L$.first().\\
	     \precond $L$ is not empty.}*/

const E& tail() const { return LEDA_CONST_ACCESS(E,SLIST::tail()); }
/*{\Mop      returns the last element of $L$, i.e. the contents
	     of $L$.last().\\
	     \precond $L$ is not empty.}*/


slist_item push(const E& x)   { return SLIST::push(leda_copy(x));}
/*{\Mop      adds a new item $\Litem{x}$ at the front of $L$ and returns it. }*/
 
slist_item append(const E& x) { return SLIST::append(leda_copy(x));}
/*{\Mop      appends a new item $\Litem{x}$ to $L$ and returns it. }*/

slist_item insert(const E& x, slist_item pos) 
{ return SLIST::insert(leda_copy(x),pos); }
/*{\Mopl     inserts a new item $\Litem{x}$ after item $pos$ into $L$ and
             returns it.\\
	     \precond $it$ is an item in $L$.}*/


const E& pop() 
{ LEDA_CLEAR(E,buf); buf = SLIST::pop(); return LEDA_CONST_ACCESS(E,buf); }
/*{\Mop      deletes the first item from $L$ and returns its
             contents. \\
	     \precond $L$ is not empty.}*/


void del_succ_item(slist_item it) 
{ slist_item sit = succ(it); 
  LEDA_CLEAR(E,entry(sit));
  SLIST::del_succ(it); 
 }
/*{\Mop  deletes the successor of item $it$ from $L$.\\
	 \precond $it$ is an item in $L$ and has a successor. }*/


void conc(slist<E>& L1)      { SLIST::conc((SLIST&)L1); }
/*{\Mop      appends list $L_1$ to list $L$ and makes $L_1$ the empty list.\\
             \precond $L\ !=\ L_1$.}*/


E&  operator[](slist_item it) 
{ return LEDA_ACCESS(E,SLIST::entry(it)); }
/*{\Marrop   returns a reference to the contents of $it$.}*/

const E& operator[](slist_item it) const 
{ return LEDA_CONST_ACCESS(E,SLIST::entry(it)); }


slist_item operator+=(const E& x)  { return append(x); }
/*{\Mbinop      appends a new item $\Litem{x}$ to $L$ and returns it. }*/


};



#if LEDA_ROOT_INCL_ID == 520450
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
