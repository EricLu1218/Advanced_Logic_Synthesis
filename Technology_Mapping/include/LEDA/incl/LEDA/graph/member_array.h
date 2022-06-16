/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  member_array.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.6 $  $Date: 2007/02/28 07:44:13 $

#ifndef LEDA_MEMBER_ARRAY_H
#define LEDA_MEMBER_ARRAY_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520190
#include <LEDA/internal/PREAMBLE.h>
#endif

LEDA_BEGIN_NAMESPACE

template<class T, class array, class data, class T1 = T>
class member_array{ 
  typedef typename array::index index;

  array& A;
  T data::*ptr;
  
public:
  
 member_array(array& arr, T data::*p) : A(arr), ptr(p) {}
  
 T1& operator[](index v) { return (T1&)(((data&)A[v]).*ptr); } 
 const T1& operator[](index v) const { return (const T1&)(((data&)A[v]).*ptr); } 
};


/*

template<class T, class array>
class member_array1 { 
  typedef typename array::value_type data;
  typedef typename array::index index;

  array& A;
  T data::*ptr;
  
public:
  
  member_array1(array& arr, T data::*p) : A(arr), ptr(p) {}
  
  T& operator[](index v) { return A[v].*ptr; } 
  const T& operator[](index v) const { return A[v].*ptr; } 
};




template<class T, class array, T array::value_type::*ptr>
class member_array2 { 
  typedef typename array::value_type data;
  typedef typename array::index index;

  array& A;
  
public:
  
  member_array2(array& arr) : A(arr) {}
  
  T& operator[](index v) { return A[v].*ptr; } 
  const T& operator[](index v) const { return A[v].*ptr; } 
};
  
  
*/

#if LEDA_ROOT_INCL_ID == 520190
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
