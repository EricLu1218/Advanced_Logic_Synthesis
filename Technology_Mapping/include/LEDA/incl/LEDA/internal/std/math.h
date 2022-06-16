/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  math.h
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
#include <cmath>
#if (__BORLANDC__ > 0x520)
using std::sin;
using std::cos;
#endif
#else
#include <math.h>
#endif

#if defined(__BORLANDC__) && !defined(RINT_DEFINED)
inline int rint(double x) { return int(x+0.5); }
#define RINT_DEFINED
#endif

#undef LEDA_STD_INCLUDE
