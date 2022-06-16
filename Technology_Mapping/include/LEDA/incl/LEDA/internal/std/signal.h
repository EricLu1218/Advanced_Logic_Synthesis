/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  signal.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

/* $Revision: 1.2 $  $Date: 2007/02/25 00:49:17 $ */

#define LEDA_STD_INCLUDE

#if defined(LEDA_STD_HEADERS)
#include <csignal>

#if (__BORLANDC__ > 0x520) || defined(__SUNPRO_CC)
using std::signal;
using std::raise;
#endif

#else
#include <signal.h>
#endif

#undef LEDA_STD_INCLUDE
