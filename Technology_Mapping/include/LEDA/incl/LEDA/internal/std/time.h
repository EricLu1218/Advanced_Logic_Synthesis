/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  time.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

/* $Revision: 1.2 $  $Date: 2007/02/25 00:49:19 $ */

#define LEDA_STD_INCLUDE

#if defined(LEDA_STD_HEADERS)

#include <ctime>

#if (__BORLANDC__ > 0x520)
using std::time;
using std::time_t;
using std::clock;
using std::clock_t;
using std::ctime;
using std::tm;
using std::localtime;
#endif

#else

#include <time.h>

#endif

#undef LEDA_STD_INCLUDE
