/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  rat_transform.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:12 $


#ifndef LEDA_RAT_TRANSFORM_H
#define LEDA_RAT_TRANSFORM_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520052
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>
#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/numbers/integer_matrix.h>
#include <LEDA/geo/rat_kernel.h>
#include <LEDA/geo/rat_polygon.h>
#include <LEDA/geo/rat_gen_polygon.h>


/* AND NO COMES THE PART THAT ADOPTS THE KERNEL */

#include <LEDA/geo/rat_kernel_names.h>

#include <LEDA/geo/generic/TRANSFORM.h>

LEDA_BEGIN_NAMESPACE

inline const char* leda_tname(const rat_transform*) 
{ return "rat_transform"; }

#include <LEDA/geo/kernel_names_undef.h>

#if LEDA_ROOT_INCL_ID == 520052
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif 

