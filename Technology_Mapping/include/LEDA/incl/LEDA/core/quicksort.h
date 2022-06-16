/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  quicksort.t
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.5 $  $Date: 2007/02/25 00:48:07 $

#ifndef LEDA_QUICKSORT_T
#define LEDA_QUICKSORT_T

// this file contains quicksort function templates
// specializations for some builtin-types are declared in <LEDA/core/basic_alg.h> 


#include <LEDA/system/basic.h>
#include <LEDA/core/basic_alg.h>

LEDA_BEGIN_NAMESPACE



#define QS_BODY_P(qsort,E) \
{ if (leda_smaller(D[r],D[l]))\
     { leda_swap(D[l],D[r]); leda_swap(I[l],I[r]); }\
  if (r == l+1) return;\
  int k = (l+r)/2;\
  if (leda_smaller(D[k],D[l]))\
     { leda_swap(D[l],D[k]); leda_swap(I[l],I[k]); }\
  else \
     if (leda_smaller(D[r],D[k]))\
        { leda_swap(D[k],D[r]); leda_swap(I[k],I[r]); }\
  if (r == l+2) return;\
  int i = l+1;\
  int j = r;\
  leda_swap(D[i],D[k]); leda_swap(I[i],I[k]);\
  k = i;\
  const E& s = D[i];\
  for(;;)\
  { while (leda_smaller(D[++i],s));\
    while (leda_smaller(s,D[--j]));\
    if (i<j) { leda_swap(D[i],D[j]); leda_swap(I[i],I[j]); }\
    else break;\
   }\
  leda_swap(D[k],D[j]); \
  leda_swap(I[k],I[j]);\
  if (j > l+1) qsort(D,I,l,j-1);\
  if (r > j+1) qsort(D,I,j+1,r);\
}



#define QS_BODY_C(qsort,E) \
{ if (leda_smaller(*r,*l)) leda_swap(*l,*r);\
  if (r == l+1) return;\
  E* k = l + (r-l)/2;\
  if (leda_smaller(*k,*l)) \
     leda_swap(*l,*k);\
  else\
     if (leda_smaller(*r,*k)) leda_swap(*k,*r);\
  if (r == l+2) return;\
  E* i = l+1;\
  E* j = r;\
  leda_swap(*i,*k); \
  k = i;\
  const E& s = *i;\
  for(;;)\
  { while (leda_smaller(*++i,s));\
    while (leda_smaller(s,*--j));\
    if (i<j) leda_swap(*i,*j);\
    else break;\
   }\
  leda_swap(*k,*j); \
  if (j > l+1) qsort(l,j-1);\
  if (r > j+1) qsort(j+1,r);\
}


#define QS_BODY_C1(qsort,E) \
{ E* j = l + (r-l)/2;\
  leda_swap(*l,*j);\
  E* i = l;\
  j = r+1;\
  E s = *l;\
  for(;;)\
  { while (leda_smaller(*++i,s));\
    while (leda_smaller(s,*--j));\
    if (i<j) leda_swap(*i,*j);\
    else break;\
   }\
  leda_swap(*l,*j);\
  if (j > l+1) qsort(l,j-1);\
  if (r > j+1) qsort(j+1,r);\
}



/*
#define QS_BODY_C1(qsort,E) \
{ if (leda_smaller(*r,*l)) leda_swap(*l,*r);\
  if (r == l+1) return;\
  E* k = l + (r-l)/2;\
  if (leda_smaller(*k,*l)) \
     leda_swap(*l,*k);\
  else\
     if (leda_smaller(*r,*k)) leda_swap(*k,*r);\
  if (r == l+2) return;\
  E  s = *k;\
  E* i = l+1;\
  E* j = r;\
  *k = *i;\
  for(;;)\
  { while (leda_smaller(*++i,s));\
    while (leda_smaller(s,*--j));\
    if (i<j) leda_swap(*i,*j);\
    else break;\
   }\
  *(l+1) = *j; *j = s;\
  if (j > l+1) qsort(l,j-1);\
  if (r > j+1) qsort(j+1,r);\
}
*/




/*
#define QS_BODY_C1(qsort,E) \
{ if (r <= l) return;\
  if (r > l+5) { E* k = l + (r-l)/2; leda_swap(*l,*k); }\
  E x = *l;\
  E* i = r+1;\
  for(E* j = r; j>l; j--)\
      if (leda_smaller(x,*j)) leda_swap(*--i,*j);\
  leda_swap(*--i,*l);\
  qsort(l,i-1);\
  qsort(i+1,r);\
}
*/




// variant using <

#define QS_BODY_C_OPERATOR(qsort,E) \
{ if (*r < *l) leda_swap(*l,*r);\
  if (r == l+1) return;\
  E* k = l + (r-l)/2;\
  if (*k < *l) \
     leda_swap(*l,*k);\
  else\
     if (*r < *k) leda_swap(*k,*r);\
  if (r == l+2) return;\
  E* i = l+1;\
  E* j = r;\
  leda_swap(*i,*k); \
  k = i;\
  const E& s = *i;\
  for(;;)\
  { while (*++i < s);\
    while (s < *--j);\
    if (i<j) leda_swap(*i,*j);\
    else break;\
   }\
  leda_swap(*k,*j); \
  if (j > l+1) qsort(l,j-1);\
  if (r > j+1) qsort(j+1,r);\
}


// using cmp-function

#define QS_BODY_CMP(qsort,E,cmp) \
{ if (cmp(*r,*l) < 0) leda_swap(*l,*r);\
  if (r == l+1) return;\
  E* k = l + (r-l)/2;\
  if (cmp(*k,*l) < 0) \
     leda_swap(*l,*k);\
  else\
     if (cmp(*r,*k) < 0) leda_swap(*k,*r);\
  if (r == l+2) return;\
  E* i = l+1;\
  E* j = r;\
  leda_swap(*i,*k); \
  k = i;\
  const E& s = *i;\
  for(;;)\
  { while (cmp(*++i,s) < 0);\
    while (cmp(s,*--j) < 0);\
    if (i<j) leda_swap(*i,*j);\
    else break;\
   }\
  leda_swap(*k,*j); \
  if (j > l+1) qsort(l,j-1,cmp);\
  if (r > j+1) qsort(j+1,r,cmp);\
}



template<class E, class inf>
__temp_func_inline 
void QUICKSORT_P(E* D, inf* I, int l, int r)
QS_BODY_P(QUICKSORT_P,E)


template <class LIST>
__temp_func_inline 
void QUICKSORT_LIST(LIST& L)
{ typedef typename LIST::value_type E;
  typedef typename LIST::item IT;
  int n = L.length();
  if (n <= 1) return;
  IT* I = LEDA_NEW_VECTOR(IT,n);
  IT* i = I;
  E* D = LEDA_NEW_VECTOR(E,n);
  E* d = D;
  IT it;
  forall_items(it,L)
  { *i++ = it;
    new(d,0) E(L[it]);
    d++;
   }
  QUICKSORT_P(D,(GenPtr*)I,0,n-1);
  L.permute(I);
  E* D_stop = D+n;
  for(d=D; d<D_stop; d++) 
#if defined(__BORLANDC__)
     d->E::~E();
#else
     d->~E();
#endif
  LEDA_DEL_VECTOR(D);
  LEDA_DEL_VECTOR(I);
}




template<class E>
__temp_func_inline 
void QUICKSORT_C(E* l, E* r)
QS_BODY_C(QUICKSORT_C,E)

template<class E>
__temp_func_inline 
void QUICKSORT_C1(E* l, E* r)
QS_BODY_C1(QUICKSORT_C1,E)



template<class E>
__temp_func_inline 
void QUICKSORT_CMP(E* l, E* r, int (*cmp)(const E&,const E&))
QS_BODY_CMP(QUICKSORT_CMP,E,cmp)

template<class E>
__temp_func_inline 
void QUICKSORT_CMP(E* l, E* r, const leda_cmp_base<E>& cmp)
QS_BODY_CMP(QUICKSORT_CMP,E,cmp)


template<class E>
__temp_func_inline 
void QUICKSORT_C_OPERATOR(E* l, E* r)
QS_BODY_C_OPERATOR(QUICKSORT_C_OPERATOR,E)


template <class ARRAY>
__temp_func_inline 
void QUICKSORT_ARR(ARRAY& arr, int l, int n)
{ typedef typename ARRAY::value_type E;
  if (n > 1)
  { if (sizeof(E) == sizeof(GenPtr))
     { E* A = (E*)arr.first_item() + l;
       QUICKSORT_C(A,A+n-1);
      }
    else
     { E* p;
       E* A = LEDA_NEW_VECTOR(E,n);
       E* stop = A+n;
       GenPtr* q = (GenPtr*)arr.first_item() + l;
       for(p=A; p<stop; p++) new(p,0) E(LEDA_ACCESS(E,*q++));
       QUICKSORT_C(A,stop-1);
       q = (GenPtr*)arr.first_item() + l;
       for(p=A; p<stop; p++) 
       { LEDA_ACCESS(E,*q++) = *p;
#if defined(__BORLANDC__)
         p->E::~E();
#else
         p->~E();
#endif
        }
       LEDA_DEL_VECTOR(A);
      }
   }
}



template<class Iterator, class Compare, class E>
__temp_func_inline 
void QUICKSORT_STL0(Iterator l, Iterator r, Compare smaller, E*)
{ if (smaller(*r,*l)) leda_swap(*l,*r);
  if (r == l+1) return;
  Iterator k = l + (r-l)/2;
  if (smaller(*k,*l)) 
     leda_swap(*l,*k);
  else
     if (smaller(*r,*k)) leda_swap(*k,*r);
  if (r == l+2) return;
  Iterator i = l+1;
  Iterator j = r;
  leda_swap(*i,*k); 
  k = i;
  const E& s = *i;
  for(;;)
  { while (smaller(*++i,s));
    while (smaller(s,*--j));
    if (i<j) leda_swap(*i,*j);
    else break;
   }
  leda_swap(*k,*j); 
  //if (j > l+1) QUICKSORT_STL(l,j-1,smaller);
  //if (r > j+1) QUICKSORT_STL(j+1,r,smaller);
  if (j > l+1) QUICKSORT_STL0(l,j-1,smaller,(E*)0);
  if (r > j+1) QUICKSORT_STL0(j+1,r,smaller,(E*)0);
}

template<class Iterator, class Compare>
__temp_func_inline 
void QUICKSORT_STL(Iterator l, Iterator r, Compare smaller)
{ QUICKSORT_STL0(l,r,smaller,value_type(l)); }




LEDA_END_NAMESPACE

#endif
