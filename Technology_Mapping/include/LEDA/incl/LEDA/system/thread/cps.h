/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  cps.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:30 $


// class for mutual exclusion using the CPS-library of the CONVEX

#include <cps.h>
#include <LEDA/internal/std/errno.h>

LEDA_BEGIN_NAMESPACE

inline int get_thread_id() { return (int)cps_ktid(); }

class leda_mutex {

  leda_mutex(const leda_mutex& m) 
  { cps_mutex_alloc(&lock_);} 

  leda_mutex operator=(const leda_mutex& m) 
  { cps_mutex_alloc(&lock_); return *this; }

public:
  leda_mutex() { cps_mutex_alloc (&lock_); }
 ~leda_mutex() { cps_mutex_free (&lock_);  }


  void acquire() { cps_mutex_lock (&lock_); }
  void lock()    { cps_mutex_lock (&lock_); }
  void enter()   { cps_mutex_lock (&lock_); }

  void release() { cps_mutex_unlock (&lock_); }
  void unlock()  { cps_mutex_unlock (&lock_); }
  void exit()    { cps_mutex_unlock (&lock_); }

  int trylock()  { return cps_mutex_trylock (&lock_); }
  int tryenter() { return cps_mutex_trylock (&lock_); }
  

private:
  cps_mutex_t lock_;

}; 

LEDA_END_NAMESPACE
