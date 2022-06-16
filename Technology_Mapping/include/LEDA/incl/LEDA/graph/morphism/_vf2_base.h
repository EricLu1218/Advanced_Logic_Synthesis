/*******************************************************************************
+
+  vf2_base.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  Base class for the vf2 graph morphism implementations
+
*******************************************************************************/
#ifndef VF2_BASE_H
#define VF2_BASE_H

#include <LEDA/system/basic.h>
#include <LEDA/graph/morphism/_graph_morphism_base.h>

LEDA_BEGIN_NAMESPACE


template<class graph_t>
class vf2_base : public graph_morphism_base<graph_t>
{
protected:
  typedef graph_morphism_base<graph_t> base;
  typedef typename base::node node;
  typedef typename base::edge edge;
  typedef typename base::node_morphism node_morphism;
  typedef typename base::edge_morphism edge_morphism;
  typedef typename base::morphism morphism;
  typedef typename base::morphism_list morphism_list;
  typedef typename base::node_compat node_compat;
  typedef typename base::edge_compat edge_compat;
  typedef typename base::callback callback;
  typedef typename base::prep_graph prep_graph;
  typedef typename base::JOB JOB;
};

LEDA_END_NAMESPACE

#endif
