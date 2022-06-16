/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _d_array.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef _LEDA_D_ARRAY_H
#define _LEDA_D_ARRAY_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520443
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/d_array.h>

LEDA_BEGIN_NAMESPACE

template <class I, class E, class impl> 
class _d_array : public d_array<I,E,impl> {};


#if LEDA_ROOT_INCL_ID == 520443
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
