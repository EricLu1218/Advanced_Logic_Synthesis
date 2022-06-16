/*******************************************************************************
+
+  _ext_bounded_adjacency_matrix.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  An extended adjacency matrix (not binary, but two bits to signify connection 
+  in each direction), nodes indexed by numbers
+
*******************************************************************************/

#ifndef EXT_BOUNDED_ADJACENCY_MATRIX_H
#define EXT_BOUNDED_ADJACENCY_MATRIX_H

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE


typedef unsigned char adj_t;

//using std::ostream;

template<typename ord_t>
class ext_bounded_adjacency_matrix
{
  ord_t count;
  adj_t* a;

public:
  ext_bounded_adjacency_matrix(ord_t _count);
 ~ext_bounded_adjacency_matrix();
  adj_t& operator()(ord_t i, ord_t j) const { return a[j * count + i]; }

  char sym(int i, int j) const 
  { if(a[j * count + i] == 0) return '.'; 
    else return (char)('0' + a[j * count + i]); }

  ord_t get_count() { return count; }
};



template<class ord_t>
ext_bounded_adjacency_matrix<ord_t>::ext_bounded_adjacency_matrix(ord_t _count)
{
  count = _count;
  a = new adj_t[count * count];
  for(int i = 0; i < count * count; i++) a[i] = 0;
}


template<class ord_t>
ext_bounded_adjacency_matrix<ord_t>::~ext_bounded_adjacency_matrix()
{ delete[] a; }


template<class ord_t>
ostream& operator<<(ostream& o, const ext_bounded_adjacency_matrix<ord_t>& am)
{
  for(int i = 0; i < am.get_count(); i++)
  { for(int j = 0; j < am.get_count(); j++) o << am.sym(i, j) << " ";
    o << endl;
   }
  return o;
}


LEDA_END_NAMESPACE

#endif
