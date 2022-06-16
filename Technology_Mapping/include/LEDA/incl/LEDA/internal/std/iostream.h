/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  iostream.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:16 $

#ifndef LEDA_STD_INCLUDE_IOSTREAM
#define LEDA_STD_INCLUDE_IOSTREAM
#define LEDA_STD_INCLUDE

#if defined(LEDA_STD_IO_HEADERS)

#include <iostream>
#include <iomanip>

#if (__BORLANDC__ > 0x520)
using std::cout;
using std::endl;
using std::cin;
#endif

#else

#include <iostream.h>
#if !defined(__DMC__)
#include <iomanip.h>
#endif

#endif

#undef LEDA_STD_INCLUDE
#endif
