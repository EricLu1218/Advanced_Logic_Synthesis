/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  polygon.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:12 $


#ifndef LEDA_POLYGON_H
#define LEDA_POLYGON_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520051
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/geo/point.h>
#include <LEDA/geo/segment.h>
#include <LEDA/geo/line.h>
#include <LEDA/geo/circle.h>
#include <LEDA/core/list.h>
//#include <LEDA/geo/polygon.h>
//#include <LEDA/geo/polygon.h>


/* AND NO COMES THE PART THAT ADOPTS THE KERNEL */

#include <LEDA/geo/float_kernel_names.h>

#include <LEDA/geo/generic/POLYGON.h>

LEDA_BEGIN_NAMESPACE

inline const char* leda_tname(const polygon*) { return "polygon"; }

LEDA_END_NAMESPACE

#include <LEDA/geo/kernel_names_undef.h>

#if LEDA_ROOT_INCL_ID == 520051
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



#endif 


