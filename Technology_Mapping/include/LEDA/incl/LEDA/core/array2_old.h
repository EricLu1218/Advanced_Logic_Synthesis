/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  array2.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_ARRAY2_H
#define LEDA_ARRAY2_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520453
#include <LEDA/internal/PREAMBLE.h>
#endif

//--------------------------------------------------------------------------
// 2 dimensional arrays                                                   
//--------------------------------------------------------------------------

#include <LEDA/core/impl/gen_array.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {array2} {E} {Two Dimensional Arrays} }*/


template<class E> 

class array2 : private gen_array2 {

/*{\Mdefinition
An instance $A$ of the parameterized data type |\Mname| is a mapping from a 
set of pairs $I = [a..b] \times [c..d]$, called the index set of $A$, to the 
set of variables of data type $E$, called the element type of $A$, for two 
fixed intervals of integers $[a..b]$ and $[c..d]$.  $A(i,j)$ is called the 
element at position $(i,j)$.
}*/

void clear_entry(GenPtr& x) const { LEDA_CLEAR(E,x);  }
void copy_entry(GenPtr& x)  const { LEDA_COPY(E,x);   }
void init_entry(GenPtr& x)  const { LEDA_CREATE(E,x); }

public:

/*{\Mcreation  A }*/

array2(int a, int b, int c, int d) :gen_array2(a,b,c,d) { init(a,b,c,d);}

/*{\Mcreate      creates an instance |\Mvar| of type |\Mname| with index set 
                 $[a..b]\times [c..d]$.  }*/

array2(int n, int m)             :gen_array2(n,m)     { init(0,n-1,0,m-1);}
/*{\Mcreate      creates an instance |\Mvar| of type |\Mname| with index set 
                 $[0..n-1]\times [0..m-1]$.  }*/


~array2() { clear(); }



/*{\Moperations 1.5 5 }*/


E& operator()(int i, int j)  { return LEDA_ACCESS(E,row(i)->entry(j)); }

/*{\Mfunop      returns $A(i,j)$.\\
        	\precond $a\le i\le b$ and $c\le j\le d$.}*/


const E& operator()(int i, int j) const 
{ return LEDA_CONST_ACCESS(E,row(i)->entry(j)); }


int low1() const { return gen_array2::low1();}
/*{\Mop         returns $a$. }*/

int high1() const {return gen_array2::high1();}
/*{\Mop         returns $b$. }*/

int low2() const {return gen_array2::low2();}
/*{\Mop         returns $c$. }*/

int high2() const {return gen_array2::high2();}
/*{\Mop         returns $d$. }*/

};

/*{\Mimplementation
Two dimensional arrays are implemented by \CC vectors. All operations
take time $O(1)$, the space requirement is $O(|I|* sizeof(E))$.
}*/


#if LEDA_ROOT_INCL_ID == 520453
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif

