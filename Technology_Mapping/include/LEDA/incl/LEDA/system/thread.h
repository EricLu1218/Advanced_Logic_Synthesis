/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  thread.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.5 $  $Date: 2007/02/28 07:44:16 $

#ifndef LEDA_THREAD_H
#define LEDA_THREAD_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520416
#include <LEDA/internal/PREAMBLE.h>
#endif


LEDA_BEGIN_NAMESPACE


extern __exportF int get_thread_id();


#if !defined(LEDA_MULTI_THREAD)

class leda_mutex { // dummy mutex class
public:
  void lock()     {}
  void unlock()   {}
  void acquire()  {}
  void enter()    {}
  void release()  {}
  void exit()     {}
  int  trylock()  { return 0; }
  int  tryenter() { return 0; }
}; 


#else




class __exportC leda_mutex {

#if defined(__win32__) || defined (__win64__)
  void* mtx;
#else
  pthread_mutex_t mtx;
#endif

  leda_mutex(const leda_mutex&);
  leda_mutex& operator=(const leda_mutex&);

public:
  leda_mutex();
  leda_mutex(const char*);
 ~leda_mutex();

  void lock();
  void unlock();
  int  trylock();

  void acquire() { lock(); }
  void enter()   { lock(); }

  void release() { unlock(); }
  void exit()    { unlock(); }

  int  tryenter() { return trylock(); }

}; 


#endif


class __exportC guard {
  leda_mutex& M;
public:
  guard(leda_mutex& m) : M(m) { M.acquire(); }
 ~guard() { M.release(); }
};


class __exportC rw_lock
{
  leda_mutex lock1, lock2, helplock;
  int number;     
public:
  rw_lock() : number(0) {}
 ~rw_lock() {};
  void acquire_write();
  void writer_enter();

  void acquire_read();
  void reader_enter();

  void release_write();
  void writer_exit();
  void release_read();
  void reader_exit();

};


class __exportC read_guard {

public:
  read_guard (rw_lock& l) : rw_lock_(l) { rw_lock_.acquire_read(); }
 ~read_guard() { rw_lock_.release_read(); }

private:
  rw_lock& rw_lock_;
};
 
      
  


class __exportC write_guard {

public:
  write_guard (rw_lock& l) : rw_lock_(l) { rw_lock_.acquire_write(); }
 ~write_guard() { rw_lock_.release_write(); }

private:
  rw_lock& rw_lock_;
};
 
 
class __exportC semaphore {
  leda_mutex semalock, aclock, numlock;
  int number;
  int maxnr_;

public:
  semaphore (int nr = 1) : maxnr_(nr) {number = 0;}
  ~semaphore () {;}
  int acquire ();
  void wait();
  void p();

  void release ();
  void signal();
  void v();

};

#if LEDA_ROOT_INCL_ID == 520416
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif

