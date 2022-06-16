/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  assert.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:16 $

#ifndef LEDA_ASSERT_H
#define LEDA_ASSERT_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520405
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>


extern __exportF 
int assert_error(const char* cond, const char* fname, int line);

#if defined(assert)
#undef assert
#endif

#ifdef	NDEBUG

#define	assert(cond) ((void)0)

#else

#define assert(cond) (void)((cond) || assert_error(#cond,__FILE__,__LINE__))

#endif

#if LEDA_ROOT_INCL_ID == 520405
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


#endif
