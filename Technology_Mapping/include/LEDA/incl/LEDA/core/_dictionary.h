/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _dictionary.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef __LEDA_DICTIONARY_H
#define __LEDA_DICTIONARY_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520474
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/dictionary.h>

LEDA_BEGIN_NAMESPACE

template <class K, class I, class impl> 
class _dictionary : public dictionary<K,I,impl> {};

LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520474
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif
#endif

