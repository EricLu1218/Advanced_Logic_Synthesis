/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  stdlib.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

/* $Revision: 1.2 $  $Date: 2007/02/25 00:49:18 $ */

#define LEDA_STD_INCLUDE

#if defined(LEDA_STD_HEADERS)

#include <cstdlib>

#if (__BORLANDC__ > 0x520)
using std::_streams;
using std::exit;
using std::getenv;
using std::system;
using std::abort;
using std::strtol;
using std::strtod;
using std::atoi;
#endif

#else

#include <stdlib.h>

#endif

#undef LEDA_STD_INCLUDE
