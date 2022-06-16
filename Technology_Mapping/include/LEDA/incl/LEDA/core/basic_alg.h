/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  basic_alg.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef LEDA_BASIC_ALG_H
#define LEDA_BASIC_ALG_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520457
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

extern __exportF void QUICKSORT_P(int*    D, GenPtr* I, int l, int r);
extern __exportF void QUICKSORT_P(float*  D, GenPtr* I, int l, int r);
extern __exportF void QUICKSORT_P(double* D, GenPtr* I, int l, int r);

extern __exportF void QUICKSORT_C(int*    l, int*    r);
extern __exportF void QUICKSORT_C(float*  l, float*  r);
extern __exportF void QUICKSORT_C(double* l, double* r);


/*
extern __exportF void QUICKSORT_P0(int*    D, GenPtr* I, int l, int r);
extern __exportF void QUICKSORT_P0(float*  D, GenPtr* I, int l, int r);
extern __exportF void QUICKSORT_P0(double* D, GenPtr* I, int l, int r);

extern __exportF void QUICKSORT_C0(int* l, int* r);
extern __exportF void QUICKSORT_C0(float* l, float* r);
extern __exportF void QUICKSORT_C0(double* l, double* r);

inline void QUICKSORT_P(int*    D, GenPtr* I, int l, int r)
{ QUICKSORT_P0(D,I,l,r); }

inline void QUICKSORT_P(float*  D, GenPtr* I, int l, int r)
{ QUICKSORT_P0(D,I,l,r); }

inline void QUICKSORT_P(double* D, GenPtr* I, int l, int r)
{ QUICKSORT_P0(D,I,l,r); }

inline void QUICKSORT_C(int* l, int* r)       { QUICKSORT_C0(l,r); }
inline void QUICKSORT_C(float* l, float* r)   { QUICKSORT_C0(l,r); }
inline void QUICKSORT_C(double* l, double* r) { QUICKSORT_C0(l,r); }
*/



extern __exportF int    SELECT(int* l,int* r,int k); 
extern __exportF double SELECT(double*,double*,int);


extern __exportF void INTSORT(int* A, int n);


#if LEDA_ROOT_INCL_ID == 520457
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif

