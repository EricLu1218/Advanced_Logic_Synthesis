/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  list.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.7 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_LIST_H
#define LEDA_LIST_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520455
#include <LEDA/internal/PREAMBLE.h>
#endif


/*{\Manpage {list} {E} {Linear Lists} {L}}*/


#include <LEDA/core/impl/dlist.h>
#include <LEDA/core/quicksort.h>

LEDA_BEGIN_NAMESPACE

template<class E> 
class list : public dlist 
{

/*{\Mdefinition
An instance $L$ of the parameterized data type |\Mname| is a sequence
of items (|list_item|). Each item in $L$ contains an element of data
type $E$, called the element type of $L$. The number of items in $L$
is called the length of $L$.  If $L$ has length zero it is called the
empty list. In the sequel $\Litem{x}$ is used to denote a list item
containing the element $x$ and $L[i]$ is used to denote the contents
of list item $i$ in $L$.}*/

const leda_cmp_base<E>* cmp_ptr;

mutable int (*cmp_ptr1)(const E&, const E&);

int  cmp(GenPtr x, GenPtr y) const 
{ if (cmp_ptr1)
     return LEDA_CALL2(*cmp_ptr1,E,x,y);
  else
     return LEDA_CALL2(*cmp_ptr,E,x,y);
}


int  (*ord_ptr)(const E&);           // pointer to user supplied ord function 
void (*app_ptr)(E&);                 // pointer to user supplied apply function 

int  el_type_id()            const { return LEDA_TYPE_ID(E); }

int  ord(GenPtr x)           const { return LEDA_CALL1(ord_ptr,E,x); }
void app(GenPtr& x)          const { (*app_ptr)(LEDA_ACCESS(E,x)); }

void clear_el(GenPtr& x)     const { LEDA_CLEAR(E,x); }
void copy_el(GenPtr& x)      const { LEDA_COPY(E,x); }

//void print_el(GenPtr& x,ostream& out) const { LEDA_PRINT(E,x,out);  }
//void read_el(GenPtr& x,istream& in)   const { LEDA_READ(E,x,in);    }


public:
void set_default_cmp(bool b) const
{ 
  if (b)
    { int (*cmp)(const E&, const E&) = compare;
      cmp_ptr1 = cmp;
     }
  else
    cmp_ptr1 = 0;
  (bool&)dlist::def_cmp = b;
}

GenPtr buf;

public:

/*{\Mtypes 5}*/ 

typedef list_item item;
/*{\Mtypemember the item type.}*/

typedef E value_type;
/*{\Mtypemember the value type.}*/

// additional type definitions ...
typedef value_type*         pointer;
typedef const value_type*   const_pointer;
typedef value_type&         reference;
typedef const value_type&   const_reference;
typedef int                 size_type;
typedef int                 difference_type;


typedef int (*cmp_func)(const E&,const E&);

/*{\Mcreation 3}*/ 

list() { buf=0; cmp_ptr1=0; }
/*{\Mcreate creates  an instance |\Mvar| of type |\Mname| and initializes it to 
            the empty list.}*/

explicit 
list(const E& a) : dlist(leda_copy(a)) { buf=0; cmp_ptr1=0; }

list(const list<E>& a) : dlist(a) { buf=0; cmp_ptr1=0; }

virtual ~list() { clear(); if (buf) LEDA_CLEAR(E,buf); }


/*{\Moperations 2 5 }*/

/*{\Mtext \headerline{Access Operations}}*/

int length() const {return dlist::length();}
/*{\Mop      returns the length of $L$.}*/

int size() const {return dlist::size();}
/*{\Mop      returns $L$.length().}*/

bool empty() const {return dlist::empty();}
/*{\Mop      returns true if $L$ is empty, false otherwise.}*/

list_item first() const {return dlist::first();}
/*{\Mop      returns the first item of $L$ (nil if $L$ is empty).}*/

list_item last() const {return dlist::last();}
/*{\Mop      returns the last item of $L$. (nil if $L$ is empty)}*/

list_item get_item(int i) const {return dlist::get_item(i);}
/*{\Mop      returns the item at position $i$ (the first position is 0).\\
             \precond $0 \le i < L$.length(). {\bf Note} that this takes
             time linear in $i$.}*/

list_item succ(list_item it) const {return dlist::succ(it);}
/*{\Mop      returns the successor item of item $it$, nil if
	     $it=L$.last().\\
	     \precond $it$ is an item in $L$.}*/

list_item pred(list_item it) const {return dlist::pred(it);}
/*{\Mop      returns the predecessor item of item $it$, nil if
	     $it=L$.first().\\
	     \precond $it$ is an item in $L$.}*/

list_item cyclic_succ(list_item it) const {return dlist::cyclic_succ(it);}
/*{\Mop      returns the cyclic successor of item $it$, i.e.,
	     $L$.first() if $it = L$.last(), $L$.succ($it$) otherwise.}*/

list_item cyclic_pred(list_item it) const
{return dlist::cyclic_pred(it);}
/*{\Mop      returns the cyclic predecessor of item $it$, i.e,
             $L$.last() if $it = L$.first(), $L$.pred($it$) otherwise.}*/

const E& contents(list_item it) const 
{ return LEDA_CONST_ACCESS(E,dlist::entry(it)); }
/*{\Mop      returns the contents $L[it]$ of item $it$.\\
	     \precond $it$ is an item in $L$.}*/

const E& inf(list_item it) const { return contents(it); }
/*{\Mop      returns $L$.contents($it$).}*/


const E& front() const { return LEDA_CONST_ACCESS(E,dlist::head()); }
/*{\Mop      returns the first element of $L$, i.e. the contents
	     of $L$.first().\\
	     \precond $L$ is not empty.}*/

const E& head() const { return front(); }
/*{\Mop  same as |L.front()|. }*/

const E& back() const { return LEDA_CONST_ACCESS(E,dlist::tail()); }
/*{\Mop      returns the last element of $L$, i.e. the contents
	     of $L$.last().\\
	     \precond $L$ is not empty.}*/

const E& tail() const { return back(); }
/*{\Mop  same as |L.back()|. }*/


#if defined(EQ_BY_COMPARE)

int rank(const E& x) const   
{ set_default_cmp(true);
  int r = dlist::rank(leda_cast(x)); 
  set_default_cmp(false);
  return r;
}

#else

int rank(const E& x) const
{ list_item p = first();
  int r = 1;
  while (p && !(contents(p) == x)) { p = succ(p); r++; }
  return (p) ? r : 0;
} 
/*{\Mop      returns the rank of $x$ in $L$, i.e. its first
	     position in $L$ as an integer from [1\dots $\Labs{L}$]
	     (0 if $x$ is not in $L$). {\bf Note} that this takes
             time linear in $rank(x)$. \precond {\tt operator==} has to be 
             defined for type $E$. }*/
#endif



/*{\Mtext \headerline{Update Operations}}*/

list_item push(const E& x)   { return dlist::push(leda_copy(x));}
/*{\Mop      adds a new item $\Litem{x}$ at the front of $L$ and 
	     returns it (|L.insert(x,L.first(),leda::before|)).}*/

//for stl compatibility

list_item push_front(const E& x) { return push(x); }
/*{\Mop   same as |L.push(x)|. }*/


list_item append(const E& x) { return dlist::append(leda_copy(x));}
/*{\Mop      appends a new item $\Litem{x}$ to $L$ and returns
             it (|L.insert(x,L.last(),leda::behind|)).}*/

//for stl compatibility

list_item push_back(const E& x)  { return append(x);    }
/*{\Mop   same as |L.append(x)|. }*/


list_item insert(const E& x, list_item pos, int dir=leda::behind)
{ return dlist::insert(leda_copy(x),pos,dir); }
/*{\Mopl     inserts a new item $\Litem{x}$ behind (if |dir=leda::behind|)
	     or in front of (if |dir=leda::before|) item $pos$ into $L$ and
	     returns it (here |leda::behind| and |leda::before|
	     are predefined constants).\\
	     \precond $it$ is an item in $L$.}*/


const E&  pop() 
{ if (buf) LEDA_CLEAR(E,buf); 
  buf = dlist::pop(); 
  return LEDA_CONST_ACCESS(E,buf);
} 
/*{\Mop      deletes the first item from $L$ and returns its contents.\\
	     \precond $L$ is not empty.}*/

//for stl compatibility

const E& pop_front() { return pop(); }
/*{\Mop   same as |L.pop()|. }*/


const E&  pop_back() 
{ if (buf) LEDA_CLEAR(E,buf);
  buf = dlist::pop_back();
  return LEDA_CONST_ACCESS(E,buf);
} 
/*{\Mop      deletes the last item from $L$ and returns its contents.\\
	     \precond $L$ is not empty.}*/


const E& Pop()  { return pop_back(); }
/*{\Mop   same as |L.pop_back()|. }*/



const E&  del_item(list_item it) 
{ if (buf) LEDA_CLEAR(E,buf);
  buf = dlist::del(it);
  return LEDA_CONST_ACCESS(E,buf);
}
/*{\Mop      deletes the item $it$ from $L$ and returns its contents $L[it]$.\\
	     \precond $it$ is an item in $L$.}*/

const E&  del(list_item it) { return del_item(it); }
/*{\Mop      same as |\Mvar.del_item(it)|. }*/


//for stl compatibility

void  erase(list_item it)
{ if (buf) LEDA_CLEAR(E,buf);
  buf = dlist::del(it);
}
/*{\Mop      deletes the item $it$ from $L$.\\
	     \precond $it$ is an item in $L$.}*/

#if defined(EQ_BY_COMPARE)

void remove(const E& x)
{ set_default_cmp(true);
  dlist::remove(leda_cast(x)); 
  set_default_cmp(false);
}

#else

void remove(const E& x)
{ list_item p = first();
  while (p)
  { list_item q = succ(p);
    if (contents(p) == x) del(p);
    p = q;
  }
}
/*{\Mop      removes all items with contents $x$ from $L$.\\
	     \precond {\tt operator==} has to be defined for type $E$. }*/

#endif


void  move_to_front(list_item it) { dlist::move_to_front(it); }
/*{\Mop      moves $it$ to the front end of $L$. }*/
 
void  move_to_rear(list_item it) { dlist::move_to_back(it); }
/*{\Mop      moves $it$ to the rear end of $L$. }*/

void  move_to_back(list_item it) { dlist::move_to_back(it); }
/*{\Mop      same as |\Mvar.move_to_rear(it)|. }*/



void  assign(list_item it, const E& x) { dlist::assign(it,leda_copy(x));}
/*{\Mop      makes $x$ the contents of item $it$.\\
	     \precond $it$ is an item in $L$.}*/

void  splice(list_item pos, list<E>& L1)
{ dlist::splice(pos,(dlist&)L1); }

void  splice(list_item pos, list<E>& L1,list_item it)
{ dlist::splice(pos,(dlist&)L1,it); }

void  splice(list_item pos, list<E>& L1,list_item it1, list_item it2)
{ dlist::splice(pos,(dlist&)L1,it1,it2); }


void  conc(list<E>& L1, int dir = leda::behind) { dlist::conc((dlist&)L1,dir); }
/*{\Mop      appends ($|dir| = |leda::behind|$ or prepends 
             (|dir = leda::before|) list $L_1$ to list $L$ and 
             makes $L_1$ the empty list.\\ 
             \precond: $L \ne L_1$}*/


void  swap(list<E>& L1) { dlist::swap(L1); }
/*{\Mop      swaps lists of items of $L$ and $L1$; }*/


void  split(list_item it, list<E>& L1, list<E>& L2)
                                 { dlist::split(it,(dlist&)L1,(dlist&)L2);}
/*{\Mopl  splits $L$ at item $it$ into lists $L1$ and $L2$. More precisely,
	  if $it \ne nil$ and $ L = x_1,\dots, x_{k-1}, it, x_{k+1},\dots, x_n$
          then $L1 = x_1, \dots, x_{k-1}$ and $L2 = it, x_{k+1}, \dots, x_n$. 
          If $it = nil$ then $L1$ is made empty and $L2$ a copy of $L$. 
          Finally $L$ is made empty if it is not identical to $L1$ or $L2$.\\
	  \precond $it$ is an item of $L$ or $nil$. }*/


void  split(list_item it, list<E>& L1, list<E>& L2, int dir)
                                { dlist::split(it,(dlist&)L1,(dlist&)L2,dir); }
/*{\Mopl     splits $L$ at item $it$ into lists $L1$ and $L2$. Item $it$ 
             becomes the first item of $L2$ if |dir==leda::before| and the 
             last item of $L1$ if |dir=leda::behind|.\\
	     \precond $it$ is an item of $L$. }*/

void extract(list_item it1, list_item it2, list<E>& L1, bool inclusive = true)
                              { dlist::extract(it1,it2,(dlist&)L1,inclusive); }
/*{\Mopl  extracts a sublist $L1$ from $L$. More precisely, if 
          $L = x_1, \dots, x_p, |it1|, \dots, |it2|, x_s, \dots, x_n$ then 
          $L1 = |it1|, \dots, |it2|$ and $L = x_1, \dots, x_p, x_s, \dots, x_n$.
	  (If |inclusive| is false then |it1| and |it2| remain in $L$.)
	  \precond |it1| and |it2| are items of $L$ or $nil$. }*/

void  apply(void (*f)(E& x))   { app_ptr = f; dlist::apply(); }
/*{\Mop      for all items $\Litem{x}$ in $L$ function $f$ is
	     called with argument $x$ (passed by reference).}*/


void reverse_items() { dlist::reverse_items(); }
/*{\Mop      reverses the sequence of items of $L$. }*/

void reverse_items(list_item it1,list_item it2)
{ dlist::reverse_items(it1,it2); }
/*{\Mop      reverses the sub-sequence $it1,\dots,it2$ of items of $L$.\\
             \precond  $it1 = it2$ or $it1$ appears before $it2$ in $L$. }*/

void reverse() { dlist::reverse(); }
/*{\Mop      reverses the sequence of entries of $L$. }*/

void reverse(list_item it1, list_item it2) { dlist::reverse(it1,it2); }
/*{\Mop      reverses the sequence of entries $L[it1] \dots L[it2]$.\\
             \precond  $it1 = it2$ or $it1$ appears before $it2$ in $L$.}*/


void permute() { dlist::permute(); }
/*{\Mop      randomly permutes the items of $L$.}*/

void permute(list_item* I) { dlist::permute(I); }
/*{\Mop      permutes the items of $L$ into the same order as stored in the
             array $I$. }*/


void clear() { dlist::clear(); }
/*{\Mop      makes $L$ the empty list. }*/



/*{\Mtext \headerline{Sorting and Searching}}*/


/*{\Moptions nextwarning=no }*/
void  sort(const leda_cmp_base<E>& cmp, int dd=1)
{ cmp_ptr = &cmp; dlist::quick_sort(dd); }


void  sort(int (*cmp)(const E&,const E&),int dd=1)
{ cmp_ptr1 = cmp; dlist::quick_sort(dd); cmp_ptr1 = 0; }
/*{\Mopl     sorts the items of $L$ using the ordering defined
	     by the compare function $cmp : E\times E \longrightarrow int$,
	     with\\

 	     $$cmp(a,b)\ \ \cases{< 0,  &if $a < b$\cr
		                  = 0,  &if $a = b$\cr
		                  > 0,  &if $a > b$\cr}$$\\

	     More precisely, if $(in_1,\dots,in_n)$ and 
	     $(out_1,\dots,out_n)$ denote the values of $L$
	     before and after the call of sort, then
	     $cmp(L[out_j], L[out_{j+1}]) \le 0$ for
	     $1\le j<n$ and there is a
	     permutation
	     $\pi$ of $[1..n]$ such that $out_i = in_{\pi_i}$ for
	     $1 \le i \le n$
	     . }*/

void  sort() { QUICKSORT_LIST(*this); }
/*{\Mop      sorts the items of $L$ using the default ordering of type $E$,
             i.e., the linear order defined by function 
             $int$ compare$(const\ E\&,\ const\ E\&)$. If $E$ is a user-defined type, 
             you have to provide the compare function (see Section \ref{User Defined Parameter Types}).}*/


void merge_sort(const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; dlist::merge_sort(); }

void  merge_sort(int (*cmp)(const E&,const E&))
{ cmp_ptr1 = cmp; dlist::merge_sort(); cmp_ptr1 = 0; }
/*{\Mop     sorts the items of $L$ using merge sort and the ordering
            defined by $cmp$. The sort is stable, i.e., if $x=y$ and 
            $\Litem{x}$ is before $\Litem{y}$ in $L$ then $\Litem{x}$ is before 
            $\Litem{y}$ after the sort. |L.merge_sort()| is more efficient
            than |L.sort()| if $L$ contains large pre-sorted intervals. }*/


void merge_sort() //{ merge_sort(compare); }
{ set_default_cmp(true);
  dlist::merge_sort(); 
  set_default_cmp(false);
}
/*{\Mopl    as above, but uses the default ordering of type $E$. If $E$ is a user-defined type, 
            you have to provide the compare function (see Section \ref{User Defined Parameter Types}). }*/


void  bucket_sort(int i, int j, int (*b)(const E&))
{ ord_ptr = b; dlist::bucket_sort(i,j); }
/*{\Mopl     sorts the items of $L$ using bucket sort,
         where $b$ maps every element $x$ of $L$ to a bucket $b(x) \in [i..j]$.
		 If $b(x) < b(y)$ then $\Litem{x}$ appears before $\Litem{y}$ after the sort.
		 If $b(x) = b(y)$, the relative order of $x$ and $y$ before the sort is retained, 
         thus the sort is stable.}*/

void  bucket_sort(int (*b)(const E&)) 
{ ord_ptr = b; dlist::bucket_sort(); }
/*{\Mopl     sorts |\Mname| into increasing order as prescribed by |b| \precond |b| is an 
             integer-valued function on $E$. }*/



void  merge(list<E>& L1, const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; dlist::merge(L1); }

void  merge(list<E>& L1, int (*cmp)(const E&,const E&))
{ cmp_ptr1 = cmp; dlist::merge(L1); cmp_ptr1 = 0; }
/*{\Mop      merges the items of $L$ and $L1$ using the ordering defined by 
             $cmp$. The result is assigned to $L$ and $L1$ is made empty.\\
             \precond $L$ and $L1$ are sorted incresingly according to the 
             linear order defined by $cmp$.}*/

void  merge(list<E>& L1)
{ set_default_cmp(true);
  dlist::merge(L1); 
  set_default_cmp(false);
}
/*{\Mop      merges the items of $L$ and $L1$ using the default linear
             order of type $E$. If $E$ is a user-defined type, you have to define the
             linear order by providing the compare function (see Section \ref{User Defined Parameter Types}).}*/


void unique(const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; dlist::unique(); }

void  unique(int (*cmp)(const E&,const E&))
{ cmp_ptr1 = cmp; dlist::unique(); cmp_ptr1 = 0; }
/*{\Mop      removes duplicates from $L$.\\
	     \precond $L$ is sorted incresingly according to the ordering 
             defined by $cmp$.}*/

#if defined(EQ_BY_COMPARE)

void unique()
{ set_default_cmp(true);
  dlist::unique(); 
  set_default_cmp(false);
}

#else

void unique()
{ list_item p = first();
  if (p == 0) return;
  list_item q = succ(p);
  while (q)
  { list_item r = succ(q);
    if (contents(p) == contents(q)) dlist::del(p,q,r);
    else p = q;
    q = r;
   }
}
/*{\Mop  removes duplicates from $L$.\\
	 \precond $L$ is sorted increasingly according to the default 
         ordering of type $E$ and {\tt operator==} is defined for $E$. If $E$ 
         is a user-defined type, you have to define the linear order by 
         providing the compare function (see Section 
         \ref{User Defined Parameter Types}).}*/

#endif


#if defined(EQ_BY_COMPARE)

list_item search(const E& x) const   
{ set_default_cmp(true);
  list_item it = dlist::search(leda_cast(x)); 
  set_default_cmp(false);
  return it;
}

#else

list_item search(const E& x) const
{ list_item it = first();
  while (it && !(contents(it) == x)) it = succ(it);
  return it;
} 
/*{\Mop      returns the first item of $L$ that contains $x$,
	     nil if $x$ is not an element of $L$.\\
	     \precond {\tt operator==} has to be defined for type $E$. }*/

#endif


list_item min(const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; return dlist::min(); }
/*{\Mopl     returns the item with the minimal contents with respect
             to the linear order defined by compare function $cmp$. }*/

list_item min() const
{ set_default_cmp(true);
  list_item it = dlist::min(); 
  set_default_cmp(false);
  return it;
}
/*{\Mop      returns the item with the minimal contents with respect
             to the default linear order of type $E$. }*/



list_item max(const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; return dlist::max(); }
/*{\Mopl     returns the item with the maximal contents with respect
             to the linear order defined by compare function $cmp$. }*/


list_item max() const //{ return max(compare); }
{ set_default_cmp(true);
  list_item it = dlist::max(); 
  set_default_cmp(false);
  return it;
}
/*{\Mop      returns the item with the maximal contents with respect
             to the default linear order of type $E$. }*/



/*{\Mtext \headerline{Input and Output} }*/

void read(istream& I) // { dlist::read(I); }
{ clear();
  for(;;)
  { char c;
    while (I.get(c) && is_space(c));
    if (!I) break;
    I.putback(c); 
    E x = E(); I >> x; append(x);
   }
}
/*{\Mopl     reads a sequence of objects of type $E$ from the 
             input stream $I$ using  |operator>>(istream&,E&)|.
	     $L$ is made a list of appropriate length and the
	     sequence is stored in $L$.}*/


void read(istream& I, char delim) //{ dlist::read(I,delim); }
{ clear();
  for(;;)
  { char c;
    while (I.get(c) && is_space(c) && c!=delim);
    if (!I || c==delim) break;
    I.putback(c); 
    E x = E(); I >> x; append(x);
  }
}
/*{\Mopl     as above but stops reading as soon as the first
             occurence of character |delim| is encountered.}*/

void read(char delim = '\n') //{ dlist::read(delim); }
{ read(cin,delim); }
/*{\Mopl     calls $L$.read($cin$, $delim$) to read $L$ from
	     the standard input stream $cin$.}*/

void read(string prompt, char delim = '\n') //{ dlist::read(prompt,delim); }
{ cout << prompt; read(cin,delim); }
/*{\Mopl     As above, but first writes string |prompt| to |cout|.}*/

void print(ostream& O, char space = ' ') const //{ dlist::print(O,space); }
{ list_item it = first(); if (!it) return;
  O << contents(it); for(it=succ(it); it; it = succ(it)) O << space << contents(it); }
/*{\Mopl     prints the contents of list $L$ to the output
	     tream $O$ using |operator<<(ostream&,const E&)|
	     to print each element. The elements are separated by 
             character $space$.}*/

void print(char space = ' ') const //{ dlist::print(space); }
{ print(cout,space); }
/*{\Mopl     calls $L$.print($cout$, $space$) to print $L$ on
	     the standard output stream $cout$.}*/

void print(string header, char space = ' ') const //{ dlist::print(s,space); }
{ cout << header; E x; forall(x,*this) cout << space << x; }
/*{\Mopl     As above, but first outputs string |header|.}*/


/*{\Mtext \headerline{Operators} }*/

list<E>& operator=(const list<E>& L1) { dlist::operator=(L1); return *this;}
/*{\Mbinop   The assignment operator makes $L$ a copy of
	     list $L_1$. More precisely if $L_1$ is the sequence
	     of items $x_1, x_2, \dots , x_n$ then $L$ is made a
	     sequence of items $y_1, y_2, \dots , y_n$ with
	     $L[y_i] = L_1[x_i]$ for $1 \le i \le n$.}*/


E&  operator[](list_item it) { return LEDA_ACCESS(E,dlist::entry(it)); }
/*{\Marrop   returns a reference to the contents of $it$.}*/

const E&  operator[](list_item it) const 
{ return LEDA_CONST_ACCESS(E,dlist::entry(it)); }

list_item operator[](int i) const   { return get_item(i); }
/*{\Marrop   an abbreviation for |\Mvar.get_item(i)|. }*/


#if !defined(__sgi)
list<E>  operator+(const list<E>& a) 
{ dlist L = *(dlist*)this + *(dlist*)&a; return *(list<E>*)&L; }
#endif


list_item operator+=(const E& x)   { return append(x); }
/*{\Mbinop   same as |\Mvar.append(x)|; returns the new item.}*/


bool operator==(const list<E>& L1) const
{ if (length() != L1.length()) return false;
  list_item it1 = L1.first();
  list_item it;
  forall_items(it,*this)
    if (!(inf(it) == L1.inf(it1))) return false;
  return true;
}



/*{\Moptions nextwarning=no }*/
/*
friend ostream& operator<<(ostream& out, const list<E>& L);
*/
/*{\Mbinopfunc  same as |\Mvar.print(out)|; returns |out|.}*/

/*{\Moptions nextwarning=no }*/
/*
friend istream& operator>>(istream& in,  list<E>& L);
*/
/*{\Mbinopfunc  same as |\Mvar.read(in))|; returns |in|.}*/



#if defined(LEDA_STL_ITERATORS)

E&       stl_item_access(list_item it)       { return operator[](it); }
const E& stl_item_access(list_item it) const { return operator[](it); }

DEFINE_STL_ITERATORS(list<E>,list_item,E)

list(const E* first, const E* end)    
{ while (first != end) append(*first++); }

iterator insert(iterator pos, const E& x) 
{ return iterator(this,insert(x,pos,leda::before)); }

void insert(iterator pos, const E* first, const E* last) 
{ while (first != last) insert(pos, *first++); }

void insert(iterator pos, const_iterator first, const_iterator last) 
{ while (first != last) insert(pos, *first++); }

void insert(iterator pos, int n, const E& x) 
{ while (n--) insert(pos, x); }

void  erase(iterator first, iterator end)  
{ while (first != end) erase(first++); }


#endif


};

template<class  T> 
inline ostream& operator<<(ostream& out, const list<T>& L) 
{ L.print(out); 
  return out;
}

template<class T>
inline istream& operator>>(istream& in,  list<T>& L)
{ L.read(in); return in;}





//------------------------------------------------------------------------------
// Iteration Macros
//------------------------------------------------------------------------------


/*{\Mtext \headerline{Iteration}

{\bf forall\_items}($it,L$)       
$\{$ ``the items of $L$ are successively assigned to $it$'' $\}$

{\bf forall}($x,L$)       
$\{$ ``the elements of $L$ are successively assigned to $x$'' $\}$ 

STL compatible iterators are provided when compiled with 
\texttt{-DLEDA\_STL\_ITERATORS} (see |LEDAROOT/demo/stl/list.c| 
for an example).
}*/


/*{\Mimplementation
The data type list is realized by doubly linked linear lists. Let $c$ be the 
time complexity of the compare function and let $d$ be the time needed to 
copy an object of type |\Mname|. All operations
take constant time except of the following operations: search, revers\_items, permute
and rank take linear time $O(n)$, item($i$) takes time $O(i)$, min, max, and unique
take time $O(c\cdot n)$, merge takes time $O(c\cdot (n1 + n2))$, operator=, apply, 
reverse, read, and print take time $O(d\cdot n)$, sort and merge\_sort take time 
$O(n\cdot c\cdot\log n)$, and bucket\_sort takes time  $O(e\cdot n + j - i)$, 
where $e$ is the time complexity of $f$. 
$n$ is always the current length of the list.}*/


#if LEDA_ROOT_INCL_ID == 520455
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif

