/*******************************************************************************
+
+  compose.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  A class that derives from two classes.
+
*******************************************************************************/

#ifndef COMPOSE_H
#define COMPOSE_H


LEDA_BEGIN_NAMESPACE

template<class A, class B>
class compose : public A, public B
{
public:
  compose()                        {}
  compose(A& a) : A(a)             {}
  compose(B& b) : B(b)             {}
  compose(A& a, B& b) : A(a),B(b)  {}

  template<class Bi> 
  compose(Bi& b) : B(b) {}

  template<class Ai, class Bi> 
  compose(Ai& a, Bi& b) : A(a),B(b) {}

  LEDA_MEMORY(compose)
};

LEDA_END_NAMESPACE

#endif
