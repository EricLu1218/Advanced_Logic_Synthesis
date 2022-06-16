/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  win32.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:31 $

#ifndef LEDA_WIN32_MUTEX_H
#define LEDA_WIN32_MUTEX_H

#include <LEDA/internal/system.h>
#include <LEDA/internal/std/errno.h>

LEDA_BEGIN_NAMESPACE

extern __exportF int get_thread_id();


// class for mutual exclusion using the Win32 thread library 

typedef void *HANDLE;

class __exportC leda_mutex {

  leda_mutex(const leda_mutex& m);

  leda_mutex operator=(const leda_mutex& m);

public:
  leda_mutex();
 ~leda_mutex();


  void acquire();
  void lock();
  void enter();

  void release();
  void unlock();
  void exit();

  int trylock();
  int tryenter();
  

private:

  HANDLE lock_;

}; 

LEDA_END_NAMESPACE

#endif
