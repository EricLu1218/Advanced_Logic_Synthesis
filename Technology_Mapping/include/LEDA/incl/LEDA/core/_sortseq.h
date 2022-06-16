/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _sortseq.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef _LEDA_SORTSEQ_H
#define _LEDA_SORTSEQ_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520473
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/sortseq.h>

LEDA_BEGIN_NAMESPACE

template <class K, class I, class impl> 
class _sortseq : public sortseq<K,I,impl> {};

#if LEDA_ROOT_INCL_ID == 520473
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
