/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  sstream.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:18 $

#define LEDA_STD_INCLUDE

#if defined(LEDA_STD_IO_HEADERS)

#include <sstream>

#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
typedef ostrstream ostringstream;
typedef istrstream istringstream;
typedef strstream  stringstream;
#endif

#undef LEDA_STD_INCLUDE
