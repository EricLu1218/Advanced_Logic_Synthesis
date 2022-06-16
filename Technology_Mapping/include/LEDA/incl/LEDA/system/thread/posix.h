/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  posix.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/25 00:49:31 $



// class for mutual exclusion using the posix thread library 

#define LEDA_STD_INCLUDE
#include <pthread.h>
#undef LEDA_STD_INCLUDE

#include <LEDA/internal/std/errno.h>

LEDA_BEGIN_NAMESPACE

inline int get_thread_id() { return (int)pthread_self(); }

class leda_mutex {

private:

  pthread_mutex_t lock_;

  leda_mutex(const leda_mutex& m);
  leda_mutex operator=(const leda_mutex& m);

public:
  leda_mutex() { cout << "create mutex " << &lock_ << endl;
                 pthread_mutex_init(&lock_, NULL); 
                }

  leda_mutex(const char* s) 
               { cout << "create mutex " << &lock_ << " s= " << s << endl;
                 pthread_mutex_init(&lock_, NULL); 
                }

 ~leda_mutex() { cout << "destroy mutex " << &lock_ << endl;
                 pthread_mutex_destroy (&lock_); }


  void lock()    { cout << "lock mutex " << &lock_ << endl;
                    pthread_mutex_lock (&lock_); }

  void acquire() { lock(); }
  void enter()   { lock(); }

  void unlock()  { cout << "unlock mutex " << &lock_ << endl;
                   pthread_mutex_unlock (&lock_); }

  void release() { unlock(); }
  void exit()    { unlock(); }

  int trylock()  { return pthread_mutex_trylock (&lock_); }
  int tryenter() { return pthread_mutex_trylock (&lock_); }
  
}; 

LEDA_END_NAMESPACE
