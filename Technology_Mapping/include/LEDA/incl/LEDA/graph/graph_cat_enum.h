/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  graph_cat_enum.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/25 00:48:37 $

#ifndef GRAPH_CATEGORY
#define GRAPH_CATEGORY

LEDA_BEGIN_NAMESPACE

enum 
{
  leda_graph_category,         
  undirected_graph_catecory, 
  directed_graph_category,      
  bidirectional_graph_category,
  opposite_graph_category,     
  bidirected_graph_category,
  filter_opposite_graph_category
};

LEDA_END_NAMESPACE

#endif
