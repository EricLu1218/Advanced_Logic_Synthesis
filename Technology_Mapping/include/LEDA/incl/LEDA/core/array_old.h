/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  array.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_ARRAY_H
#define LEDA_ARRAY_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520445
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// array
//------------------------------------------------------------------------------

#include <LEDA/core/impl/gen_array.h>
#include <LEDA/core/quicksort.h>
#include <LEDA/core/bin_search.h>

LEDA_BEGIN_NAMESPACE


/*{\Manpage {array} {E} {One Dimensional Arrays} {A}}*/


template<class E> 

class array : public gen_array {

/*{\Mdefinition
    An instance $A$ of the parameterized data type |\Mname| is a mapping from 
    an interval $I =[a..b]$ of integers, the index set of $A$, to the set of 
    variables of data type $E$, the element type of $A$. $A(i)$ is called the 
    element at position $i$. The array access operator (|A[i]|) checks its 
    precondition ($a\le i\le b$). The check can be turned off by compiling 
    with the flag \verb+-DLEDA_CHECKING_OFF+. }*/


const leda_cmp_base<E>* cmp_ptr;

mutable int (*cmp_ptr1)(const E&, const E&);

int cmp(GenPtr x, GenPtr y) const 
{ if (cmp_ptr1)
     return LEDA_CALL2(*cmp_ptr1,E,x,y);
  else
     return LEDA_CALL2(*cmp_ptr,E,x,y);
}


int  el_type_id() const { return LEDA_TYPE_ID(E); }

//void print_el(GenPtr& x,ostream& out) const { LEDA_PRINT(E,x,out);}
//void read_el(GenPtr& x,istream& in)   const { LEDA_READ(E,x,in); }

void clear_entry(GenPtr& x)           const { LEDA_CLEAR(E,x); }
void copy_entry(GenPtr& x)            const { LEDA_COPY(E,x); }
void init_entry(GenPtr& x)            const { LEDA_CREATE(E,x); }

void set_default_cmp(bool b) const
{ if (b) 
  { int (*cmp)(const E&,const E&) = compare;
    cmp_ptr1 = compare;
   } 
   else cmp_ptr1 = 0;
  (bool&)gen_array::def_cmp = b;
}

public:

/*{\Mtypes 5}*/ 

typedef GenPtr item;
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

array(int a, int b) : gen_array(a,b) 
{ gen_array::init(); 
  cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with index set $[a..b]$. }*/


explicit 

array(int n) : gen_array(n)
{ gen_array::init(); 
  cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with index set $[0..n-1]$. }*/ 


array()  { cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with empty index set.}*/


/*{\Mtext \headerline{Special Constructors} }*/

array(int low, const E& x, const E& y) : gen_array(low,low+1) 
{ gen_array::init(leda_cast(x),leda_cast(y));  
  cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with index set 
            $[low, low+1]$ initialized to $[x,y]$. }*/ 

array(int low, const E& x, const E& y, const E& w) : gen_array(low,low+2) 
{ gen_array::init(leda_cast(x),leda_cast(y),leda_cast(w)); 
  cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with index set 
            $[low, low+2]$ initialized to $[x,y,w]$. }*/ 

array(int low, const E& x, const E& y, const E& z, const E& w) 
: gen_array(low,low+3) 
{ gen_array::init(leda_cast(x),leda_cast(y),leda_cast(z),leda_cast(w)); 
  cmp_ptr1 = 0; }
/*{\Mcreate  creates an instance |\Mvar| of type |\Mname| with index set 
            $[low, low+3]$ initialized to $[x,y,z,w]$. }*/ 


// copy constructor, destructor, operator=

 array(const array<E>& A) : gen_array(A) { cmp_ptr1 = 0; }

~array()  { clear(); }

 array<E>& operator=(const array<E>& A) 
 { gen_array::operator=(A); return *this; }


bool operator==(const array<E>& A)  const
{ if (low() != A.low() || high() != A.high()) return false; 
  for(int i=low(); i<=high(); i++)
      if (operator[](i) != A[i]) return false;
  return true;
}

/*{\Moperations 1.2 5 }*/

/*{\Mtext \headerline{Basic Operations}}*/

#if defined(LEDA_CHECKING_OFF)
const E& operator[](int x) const { return LEDA_CONST_ACCESS(E,v[x-Low]); }
E&       operator[](int x)       { return LEDA_ACCESS(E,v[x-Low]); }
#else
const E& operator[](int x) const { return LEDA_CONST_ACCESS(E,entry(x)); }
E&       operator[](int x)       { return LEDA_ACCESS(E,entry(x)); }
#endif

/*{\Moptions nextwarning=no}*/
/*
E& operator[](int x);
*/
/*{\Marrop     returns $A(x)$.\\
               \precond $a\le x\le b$.  }*/

const E& get(int x ) const { return operator[](x); }

E& get(int x ) { return operator[](x); }
/*{\Mop        returns $A(x)$.\\
               \precond $a\le x\le b$.  }*/

void set(int x, const E& e) { operator[](x) = e; }
/*{\Mop        sets $A(x)=e$.\\
               \precond $a\le x\le b$.  }*/

void copy(int x, const array<E>&B, int y) { operator[](x) = B[y]; }
/*{\Mop        sets $A(x)=B(y)$.\\
               \precond $a\le x\le b$ and $B.low()\le y \le B.high()$.  }*/

void copy(int x, int y) { operator[](x) = operator[](y); }
/*{\Mop        sets $A(x)=A(y)$.\\
               \precond $a\le x\le b$ and $low()\le y \le high()$.  }*/

void resize(int a, int b) { gen_array::resize(a,b); }
/*{\Mop        sets the index set of |\Mvar| to $[a..b]$ such that
               for all $i\in[a..b]$ which are not contained in the old 
               index set $A(i)$ is set to the default value of type |E|. }*/

void resize(int n)        { gen_array::resize(n); }
/*{\Mop        same as |\Mvar.resize(0,n-1)|. }*/


int low()  const { return gen_array::low();}
/*{\Mop        returns the minimal index $a$ of |\Mvar|. }*/


int high() const { return gen_array::high();}
/*{\Mop        returns the maximal index $b$ of |\Mvar|. }*/

int size() const { return gen_array::size();}
/*{\Mop        returns the size ($b-a+1$) of |\Mvar|. }*/

void init(const E& x) { clear(); gen_array::init(leda_cast(x)); }
/*{\Mop  assigns $x$ to |A[i]| for every $i\in \{\ a \dots b\ \}$. }*/

bool C_style() const { return gen_array::low()==0;}
/*{\Mop        returns |true| if the array has ``C-style'', i.e.,
               the index set is $[0..size-1]$. }*/

void swap(int i, int j) { gen_array::swap(i,j); }
/*{\Mop        swaps the values of |A[i]| and |A[j]|. }*/

void permute() { gen_array::permute(); }
/*{\Mop        the elements of |\Mvar| are randomly permuted.}*/



void permute(int l, int h) { gen_array::permute(l,h); }
/*{\Mop        the elements of \Mvar$[l..h]$ are randomly permuted.}*/


/*{\Mtext \headerline{Sorting and Searching}}*/

void sort(const leda_cmp_base<E>& cmp)
{ cmp_ptr = &cmp; gen_array::sort(low(),high(),1); }

void sort(int (*cmp)(const E&, const E&))
{ cmp_ptr1 = cmp; gen_array::sort(low(),high(),1); cmp_ptr1 = 0; }

/*{\Mopl       sorts the elements of |\Mvar|, using function $cmp$
	      to compare two elements, i.e., if $(in_a,\dots,in_b)$
	      and $(out_a,\dots,out_b)$ denote the values of the
	      variables $(A(a),\dots,A(b))$ before and after the
	      call of sort, then $cmp(out_i,out_j) \le 0$ for $i\le j$
	      and there is a permutation $\pi$ of $[a..b]$ such that
	      $out_i=in_{\pi(i)}$ for $a \le i \le b$.}*/


void sort()
{
if (LEDA_TYPE_ID(E) == INT_TYPE_ID && 
    ( sizeof(int) == 4 && (sizeof(int) == sizeof(GenPtr))))
  gen_array::int_sort(low(),high());
else 
  QUICKSORT_ARR(*this,0,size());
}
/*{\Mop  sorts the elements of |\Mvar| according to the linear order
         of the element type $E$. \precond A linear order on $E$
         must have been defined by $compare(const E\&, const E\&)$ if $E$ is a
         user-defined type (see Section \ref{User Defined Parameter Types}).. }*/



void sort(const leda_cmp_base<E>& cmp, int l, int h)
{ cmp_ptr = &cmp; gen_array::sort(l,h,1); }

void sort(int (*cmp)(const E&, const E&), int l, int h)
{ cmp_ptr1 = cmp; gen_array::sort(l,h,1); cmp_ptr1 = 0; }
/*{\Mopl       sorts sub-array \Mvar$[l..h]$ using compare function $cmp$.}*/


void sort(int l, int h)
{
if (l < low() || h > high())
    error_handler(1,"array::sort(l,h): illegal range [l..h]");
if (LEDA_TYPE_ID(E) == INT_TYPE_ID && 
    ( sizeof(int) == 4 && (sizeof(int) == sizeof(GenPtr))))
  gen_array::int_sort(l,h);
else
  QUICKSORT_ARR(*this,l-low(),h-l+1);
}
/*{\Mop  sorts sub-array \Mvar$[l..h]$ using the linear order on $E$. If $E$ 
         is a user-defined type, you have to define the linear order by 
         providing the compare function (see Section \ref{User Defined 
         Parameter Types}).}*/


int unique()
{ if (size() == 0) return low()-1;
  int i = low(); 
  for (int j = low(); j <= high(); j++) {
   if ((*this)[j] != (*this)[i]) (*this)[++i] = (*this)[j];
  }
  return i; 
}
/*{\Mop  removes duplicates from $A$ by copying the unique elements of $A$
         to $A[A.low()],...,A[h]$ and returns $h$ (|A.low()-1| if |A| is 
         empty). \precond $A$ is sorted increasingly according to the default 
         ordering of type $E$. If $E$ is a user-defined type, you have to 
         define the linear order by providing the compare function (see 
         Section \ref{User Defined Parameter Types}).}*/

int binary_search(const leda_cmp_base<E>& cmp, const E& x) const
{ (GenPtr&)cmp_ptr = (GenPtr)&cmp; 
  int i = gen_array::binary_search(leda_cast(x));
  if (i >= low() && cmp(operator[](i),x)==0) return i;
  else return low()-1; 
}

int binary_search(int (*cmp)(const E&,const E&), const E& x) const
{ cmp_ptr1 = cmp; 
  int i = gen_array::binary_search(leda_cast(x));
  cmp_ptr1 = 0; 
  if (i >= low() && cmp(operator[](i),x)==0) return i;
  else return low()-1; 
}
/*{\Mopl      performs a binary search for $x$. Returns an $i$
	      with $A[i] = x$ if $x$ in $A$, $A$.low$()-1$
	      otherwise. Function $cmp$ is used to compare
	      two elements.\\
	      \precond $A$ must be sorted
	      according to $cmp$.}*/

int binary_search(const E& x) const 
{ int i = BINARY_SEARCH_ARR(*this,x);
  if (i >= low() && compare(operator[](i),x)==0) return i;
  else return low()-1; 
}
/*{\Mop       as above but uses the default linear order on $E$. 
              If $E$ is a user-defined type, you have to define the
              linear order by providing the compare 
              function (see Section \ref{User Defined Parameter Types}). }*/



int binary_locate(const leda_cmp_base<E>& cmp, const E& x) const
{ (GenPtr&)cmp_ptr = (GenPtr)&cmp; 
  return gen_array::binary_locate(leda_cast(x)); }

int binary_locate(int (*cmp)(const E&,const E&), const E& x) const
{ cmp_ptr1 = cmp; 
  int i = gen_array::binary_locate(leda_cast(x));
  cmp_ptr1 = 0; 
  return i; }
/*{\Mopl      Returns the maximal $i$ with $A[i] \le x$ or |A.low()|-1 
              if $x < A[low]$. Function $cmp$ is used to compare elements.\\
	      \precond $A$ must be sorted according to $cmp$.}*/


int binary_locate(const E& x) const 
{ cmp_func cmp = compare; return binary_locate(cmp,x); }
/*{\Mop   as above but uses the default linear order on $E$. If $E$ is a 
          user-defined type, you have to define the linear order by providing 
          the compare function (see Section \ref{User Defined Parameter Types}).}*/



/*{\Mtext \headerline{Input and Output} }*/

void read(istream& I)
{ clear();
  for(int i=low(); I && i <= high(); i++)  
  { E x = E(); 
    I >> x;
    operator[](i) = x;
   }
}
/*{\Mop       reads $b-a+1$ objects of type $E$ from the
	      input stream $I$ into the array $A$ using the
	      |operator>>(istream&,E&)|.}*/


void read() { read(cin); }
/*{\Mop       calls $A$.read($cin$) to read $A$ from the
	      standard input stream $cin$.}*/



void read(string s) { cout << s; read(cin); }
/*{\Mop        As above, uses string $s$ as prompt.}*/



void print(ostream& O, char space = ' ') const //{ gen_array::print(O,space);}
{ int i=low(); if (i<=high()) O << space << operator[](i); for(++i; i<=high(); i++) O << space << operator[](i); }
/*{\Mopl      prints the contents of array $A$ to the output
	      stream $O$ using |operator<<(ostream&,const E&)|
	      to print each element. The elements are separated by 
              character $space$.}*/


void print(char space = ' ') const //{gen_array::print(space);}
{ print(cout,space); }
/*{\Mop       calls $A$.print($cout$, $space$) to print $A$ on 
	      the standard output stream $cout$.}*/



void print(string s, char space = ' ') const //{gen_array::print(s,space);}
{ cout << s; for(int i=low(); i<=high(); i++) cout << space << operator[](i); }
/*{\Mopl        As above, uses string $s$ as header.}*/


/*{\Moptions nextwarning=no }*/
/*
friend ostream& operator<<(ostream& out, const array<E>& A);
*/
/*{\Mbinopfunc  same as |\Mvar.print(out)|; returns |out|.}*/


/*{\Moptions nextwarning=no }*/
/*
friend istream& operator>>(istream& in,  array<E>& A);
*/
/*{\Mbinopfunc  same as |\Mvar.read(in))|; returns |in|.}*/


// iteration

const E& inf(item it) const { return LEDA_CONST_ACCESS(E,*(GenPtr*)it); }


#if defined(LEDA_STL_ITERATORS)

E&       stl_item_access(GenPtr it)       { return LEDA_ACCESS(E,*(GenPtr*)it);}
const E& stl_item_access(GenPtr it) const { return LEDA_CONST_ACCESS(E,*(GenPtr*)it); }

DEFINE_STL_ITERATORS(array<E>,GenPtr,E)

//typedef typename array<E>::iterator iterator;

#if !defined(__BORLANDC__ ) && !defined(_MSC_VER) && !defined(__DMC__)
friend iterator operator+(const iterator& iter, int i) 
{ return iterator(iter.L,(E*)iter.it + i); }

friend iterator operator-(const iterator& iter, int i)
{ return iterator(iter.L,(E*)iter.it - i); }

friend int operator-(const iterator& it1, const iterator& it2)
{ return (E*)it1.it - (E*)it2.it; }
#endif


#endif


};

template<class E>
inline ostream& operator<<(ostream& out, const array<E>& A) 
{ A.print(out); 
  return out;
}

template<class E>
inline istream& operator>>(istream& in,  array<E>& A)
{ A.read(in); return in;}


/*{\Mtext \headerline{Iteration}
STL compatible iterators are provided when compiled with 
\texttt{-DLEDA\_STL\_ITERATORS} (see |LEDAROOT/demo/stl/array.c| 
for an example).
}*/

/*{\Mimplementation
Arrays are implemented by \CC vectors. The access operation takes time
$O(1)$, the sorting is realized by quicksort (time $O(n \log n)$) and
the binary\_search operation takes time $O(\log n)$, where $n = b-a+1$.
The space requirement is $O(n* sizeof(E))$.}*/





#if LEDA_ROOT_INCL_ID == 520445
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
