/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  dummy.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:30 $


LEDA_BEGIN_NAMESPACE

inline int get_thread_id() { return 0; }

class __exportC leda_mutex {

public:

  leda_mutex()  {}

  void acquire() {}
  void lock()    {}
  void enter()   {}

  void release() {}
  void unlock()  {}
  void exit() {}

  int trylock()  { return 0; }
  int tryenter() { return 0; }
  
}; 

LEDA_END_NAMESPACE

