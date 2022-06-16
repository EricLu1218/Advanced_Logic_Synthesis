/*******************************************************************************
+
+  _callback_base.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  base class for callback functors analogous to leda_cmp_base<>
+ 
*******************************************************************************/

#ifndef CALLBACK_BASE_H
#define CALLBACK_BASE_H

#include <LEDA/system/error.h>

LEDA_BEGIN_NAMESPACE


template <typename T> class leda_callback_base 
{
  bool (*callback_func)(const T&);

public:
  leda_callback_base(bool (*f)(const T&) = NULL) { callback_func = f; }

  virtual bool operator()(const T& x)
  { if (callback_func == NULL) 
       error_handler(1, "leda_callback_base: function undefined");

     return callback_func(x);
   }

   virtual ~leda_callback_base() {}

   LEDA_MEMORY(leda_callback_base<T>)
};


LEDA_END_NAMESPACE

#endif


