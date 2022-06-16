/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  geo_global_enums.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:48:21 $


#ifndef LEDA_GEO_GLOBAL_ENUMS
#define LEDA_GEO_GLOBAL_ENUMS

LEDA_BEGIN_NAMESPACE

enum delaunay_edge_info{ DIAGRAM_EDGE = 0, DIAGRAM_DART = 0,
                         NON_DIAGRAM_EDGE = 1, NON_DIAGRAM_DART = 1,
                         HULL_EDGE = 2, HULL_DART = 2
                       };

enum delaunay_voronoi_kind { NEAREST, FURTHEST };

enum region_kind { BOUNDED_REGION, ON_REGION, UNBOUNDED_REGION };


LEDA_END_NAMESPACE

#endif
