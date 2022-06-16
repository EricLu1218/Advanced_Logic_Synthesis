/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  guard.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:30 $

LEDA_BEGIN_NAMESPACE

class guard {

public:
  guard(leda_mutex& m) : M(m) { M.acquire(); }
 ~guard() { M.release(); }

private:
  leda_mutex& M;

};
 
LEDA_END_NAMESPACE
