/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  lock.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:30 $

LEDA_BEGIN_NAMESPACE

class rw_lock
{

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

/*
int tryenter();
*/

private:
  leda_mutex lock1, lock2, helplock;
  int number;     

};



class read_guard {

public:
  read_guard (rw_lock& l) : rw_lock_(l) { rw_lock_.acquire_read(); }
 ~read_guard() { rw_lock_.release_read(); }

private:
  rw_lock& rw_lock_;
};
 
      
  


class write_guard {

public:
  write_guard (rw_lock& l) : rw_lock_(l) { rw_lock_.acquire_write(); }
 ~write_guard() { rw_lock_.release_write(); }

private:
  rw_lock& rw_lock_;
};
 

LEDA_END_NAMESPACE
