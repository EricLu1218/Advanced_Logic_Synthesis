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

// $Revision: 1.7 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_ARRAY2_H
#define LEDA_ARRAY2_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520467
#include <LEDA/internal/PREAMBLE.h>
#endif

//--------------------------------------------------------------------------
// 2 dimensional arrays                                                   
//--------------------------------------------------------------------------


LEDA_BEGIN_NAMESPACE

/*{\Manpage {array2} {E} {Two Dimensional Arrays} }*/


template<class E> 

class array2 {

/*{\Mdefinition
An instance $A$ of the parameterized data type |\Mname| is a mapping from a 
set of pairs $I = [a..b] \times [c..d]$, called the index set of $A$, to the 
set of variables of data type $E$, called the element type of $A$, for two 
fixed intervals of integers $[a..b]$ and $[c..d]$.  $A(i,j)$ is called the 
element at position $(i,j)$.
}*/

  int l1;
  int h1;
  int l2;
  int h2;

  int sz1;
  int sz2;

  E*  vec;


public:

/*{\Mcreation  A }*/

array2(int a, int b, int c, int d) : l1(a), h1(b), l2(c), h2(d)
{ sz1 = b-a+1;
  sz2 = d-c+1;
  vec = new E[sz1*sz2];
 }

/*{\Mcreate      creates an instance |\Mvar| of type |\Mname| with index set 
                 $[a..b]\times [c..d]$.  }*/

array2(int n, int m) : l1(0), h1(n-1), l2(0), h2(m-1)
{ sz1 = n;
  sz2 = m;
  vec = new E[n*m];
 }
/*{\Mcreate      creates an instance |\Mvar| of type |\Mname| with index set 
                 $[0..n-1]\times [0..m-1]$.  }*/


~array2() { delete[] vec; }



/*{\Moperations 1.5 5 }*/


E& operator()(int i, int j)  { 
#if !defined(LEDA_CHECKING_OFF)
  if (i < l1 || i > h1 || j < l2 || j > h2) 
    error_handler(99,"array2: index out of bounds");
#endif
  return vec[i*sz1+j]; 
}

/*{\Mfunop      returns $A(i,j)$.\\
        	\precond $a\le i\le b$ and $c\le j\le d$.}*/

const E& operator()(int i, int j) const {
#if !defined(LEDA_CHECKING_OFF)
  if (i < l1 || i > h1 || j < l2 || j > h2) 
    error_handler(99,"array2: index out of bounds");
#endif
  return vec[i*sz1+j]; 
}


int low1() const { return l1; }
/*{\Mop         returns $a$. }*/

int high1() const {return h1; }
/*{\Mop         returns $b$. }*/

int low2() const {return l2;  }
/*{\Mop         returns $c$. }*/

int high2() const {return h2; }
/*{\Mop         returns $d$. }*/

};

/*{\Mimplementation
Two dimensional arrays are implemented by \CC vectors. All operations
take time $O(1)$, the space requirement is $O(|I|* sizeof(E))$.
}*/


#if LEDA_ROOT_INCL_ID == 520467
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif

