/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _p_queue.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef _LEDA_P_QUEUE_H
#define _LEDA_P_QUEUE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520522
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/p_queue.h>

LEDA_BEGIN_NAMESPACE

template <class P, class I, class impl> 
class _p_queue : public p_queue<P,I,impl> {
public:
_p_queue() {};
_p_queue(int n) : p_queue<P,I,impl>(n) {};
_p_queue(const _p_queue<P,I,impl>& PQ) : p_queue<P,I,impl>(PQ) {};
};

#if LEDA_ROOT_INCL_ID == 520522
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
