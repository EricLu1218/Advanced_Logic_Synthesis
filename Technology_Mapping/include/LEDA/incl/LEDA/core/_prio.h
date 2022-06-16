/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _prio.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:11 $

#ifndef _LEDA_PRIO_H 
#define _LEDA_PRIO_H 

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520517
#include <LEDA/internal/PREAMBLE.h>
#endif
#include <LEDA/core/prio.h>

LEDA_BEGIN_NAMESPACE

template <class ktype, class I, class impl> 
class _priority_queue : public priority_queue<ktype,I,impl> {};


#if LEDA_ROOT_INCL_ID == 520517
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
