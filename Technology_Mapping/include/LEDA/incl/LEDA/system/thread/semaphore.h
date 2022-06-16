/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  semaphore.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:31 $

LEDA_BEGIN_NAMESPACE

class semaphore {

public:
  semaphore (int nr = 1) : maxnr_(nr) {number = 0;}
  ~semaphore () {;}
  int acquire ();
  void wait();
  void p();

  void release ();
  void signal();
  void v();
/*
int trywait();
*/

private:
  leda_mutex semalock, aclock, numlock;
  int number;
  int maxnr_;

};

LEDA_END_NAMESPACE
