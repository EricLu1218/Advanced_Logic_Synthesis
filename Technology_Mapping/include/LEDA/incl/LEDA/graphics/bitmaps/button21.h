/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  button21.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:12 $

#include <LEDA/graphics/bitmaps/button21/a_annulus_21.xbm>
#include <LEDA/graphics/bitmaps/button21/circle_21.xbm>
#include <LEDA/graphics/bitmaps/button21/dice_21.xbm>
#include <LEDA/graphics/bitmaps/button21/empty_circle_21.xbm>
#include <LEDA/graphics/bitmaps/button21/encl_circle_21.xbm>
#include <LEDA/graphics/bitmaps/button21/exit_21.xbm>
#include <LEDA/graphics/bitmaps/button21/f_triang_21.xbm>
#include <LEDA/graphics/bitmaps/button21/f_voro_21.xbm>
#include <LEDA/graphics/bitmaps/button21/gen_poly_21.xbm>
#include <LEDA/graphics/bitmaps/button21/grid_21.xbm>
#include <LEDA/graphics/bitmaps/button21/help_21.xbm>
#include <LEDA/graphics/bitmaps/button21/hull_21.xbm>
#include <LEDA/graphics/bitmaps/button21/inside_21.xbm>
#include <LEDA/graphics/bitmaps/button21/intersect_21.xbm>
#include <LEDA/graphics/bitmaps/button21/line_21.xbm>
#include <LEDA/graphics/bitmaps/button21/point_21.xbm>
#include <LEDA/graphics/bitmaps/button21/poly_21.xbm>
#include <LEDA/graphics/bitmaps/button21/rect_21.xbm>
#include <LEDA/graphics/bitmaps/button21/tree_21.xbm>
#include <LEDA/graphics/bitmaps/button21/triang_21.xbm>
#include <LEDA/graphics/bitmaps/button21/triangle_21.xbm>
#include <LEDA/graphics/bitmaps/button21/union_21.xbm>
#include <LEDA/graphics/bitmaps/button21/voro_21.xbm>
#include <LEDA/graphics/bitmaps/button21/w_annulus_21.xbm>

#include <LEDA/graphics/bitmaps/button21/down_21.xbm>
#include <LEDA/graphics/bitmaps/button21/left_21.xbm>
#include <LEDA/graphics/bitmaps/button21/right_21.xbm>
#include <LEDA/graphics/bitmaps/button21/up_21.xbm>
#include <LEDA/graphics/bitmaps/button21/ps_21.xbm>
#include <LEDA/graphics/bitmaps/button21/stop_21.xbm>


#define num_xbm_button21  30

static unsigned char* xbm_button21[] = {
a_annulus_21_bits,
w_annulus_21_bits,
circle_21_bits,
dice_21_bits,
empty_circle_21_bits,
encl_circle_21_bits,
exit_21_bits,
f_triang_21_bits,
f_voro_21_bits,
gen_poly_21_bits,
grid_21_bits,
help_21_bits,
hull_21_bits,
inside_21_bits,
intersect_21_bits,
line_21_bits,
point_21_bits,
poly_21_bits,
rect_21_bits,
tree_21_bits,
triang_21_bits,
triangle_21_bits,
union_21_bits,
voro_21_bits,
down_21_bits,
left_21_bits,
right_21_bits,
up_21_bits,
ps_21_bits,
stop_21_bits
};


static const char* name_xbm_button21[] = {
"a_annulus_21",
"w_annulus_21",
"circle_21",
"dice_21",
"empty_circle_21",
"encl_circle_21",
"exit_21",
"f_triang_21",
"f_voro_21",
"gen_poly_21",
"grid_21",
"help_21",
"hull_21",
"inside_21",
"intersect_21",
"line_21",
"point_21",
"poly_21",
"rect_21",
"tree_21",
"triang_21",
"triangle_21",
"union_21",
"voro_21",
"down_21",
"left_21",
"right_21",
"up_21",
"ps_21",
"stop_21"
};


#if defined(__GNUC__)
inline char xbm_button21_unused_warning()
{ return xbm_button21[0][0] + name_xbm_button21[0][0]; }
#endif
