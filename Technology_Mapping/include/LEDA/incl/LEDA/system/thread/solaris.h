/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  solaris.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:31 $



// class for mutual exclusion using the solaris thread library 

#define LEDA_STD_INCLUDE
#include <thread.h>
#undef LEDA_STD_INCLUDE

#include <LEDA/internal/std/errno.h>

LEDA_BEGIN_NAMESPACE

inline int get_thread_id() { return (int)thr_self(); }

class leda_mutex {

  leda_mutex(const leda_mutex& m) { mutex_init(&lock_, 0, NULL); } 

  leda_mutex operator=(const leda_mutex& m) 
  { mutex_init (&lock_, 0, NULL); return *this; }

public:
  leda_mutex() { mutex_init (&lock_, 0 , NULL); }
 ~leda_mutex() { mutex_destroy (&lock_); }


  void acquire() { mutex_lock (&lock_); }
  void lock()    { mutex_lock (&lock_); }
  void enter()   { mutex_lock (&lock_); }

  void release() { mutex_unlock (&lock_); }
  void unlock()  { mutex_unlock (&lock_); }
  void exit()    { mutex_unlock (&lock_); }

  int trylock()  { return mutex_trylock (&lock_); }
  int tryenter() { return mutex_trylock (&lock_); }
  

private:
  mutex_t lock_;

}; 

LEDA_END_NAMESPACE
