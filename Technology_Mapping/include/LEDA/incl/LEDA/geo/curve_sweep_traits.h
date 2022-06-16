/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  curve_sweep_traits.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:12 $

#ifndef LEDA_CURVE_SWEEP_TRAITS_H
#define LEDA_CURVE_SWEEP_TRAITS_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520124
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/r_circle_segment.h>

#include <LEDA/geo/rat_segment.h>

#ifdef LEDA_CURVE_SWEEP_VISUAL_DEBUG
#include <LEDA/graphics/rat_window.h> // for window-trace
#include <LEDA/graphics/real_window.h> // for window-trace
#endif

#include <LEDA/core/tuple.h>
#include <LEDA/core/map.h>

#if defined(LEDA_CURVE_SWEEP_TIME)
#include <LEDA/system/timer.h>
#include <LEDA/system/counter.h>
#endif

// For a description of the requirements of a traits for curve_sweep 
// see _curve_sweep_traits.c!

LEDA_BEGIN_NAMESPACE

/// curve_pair //////////////////////////////////////////////////////////////////////////////

/*
	It may look a little clumsy to declare curve_pair outside of 
	curve_sweep_traits_r_circle_segment, but otherwise xlC (on PowerPC)
	cannot compile it.
*/

struct curve_pair {
	curve_pair(int _id1, int _id2) : id1(_id1), id2(_id2)
	{ if (id1 > id2) { id1 = _id2; id2 = _id1; } }

	int id1, id2;

	LEDA_MEMORY(curve_pair);
};

COMPARE_DECL_PREFIX
inline int DEFAULT_COMPARE(const curve_pair& p1, const curve_pair& p2)
{ 
	int cmp = p1.id1 - p2.id1;
	return cmp ? cmp : p1.id2 - p2.id2;
}

LEDA_END_NAMESPACE

#include <LEDA/core/dictionary.h>

LEDA_BEGIN_NAMESPACE

/// curve_sweep_traits_r_circle_segment /////////////////////////////////////////////////////

class __exportC curve_sweep_traits_r_circle_segment {
public:
	typedef r_circle_segment CURVE;
	typedef r_circle_point   CRV_PNT;

public:
	static int compare_intersections_with_sweepline_at(const CRV_PNT& p_sweep, 
		                                               const CURVE& A, const CURVE& B);
protected:
	void reset();
	void finish();

	void update_sentinels(const CURVE& crv);
	CURVE get_lower_sentinel() const;
	CURVE get_upper_sentinel() const;

	static void add_x_monotonous_parts(const CURVE& crv, list<CURVE>& x_monotonous_parts);

	static CURVE make_endpoints_of_curve_identical(const CURVE& crv, 
		                                           const CRV_PNT& src, const CRV_PNT& tgt);

	CURVE make_trivial_curve(const CRV_PNT& p) const { return CURVE(p); }

	static bool have_same_direction(const CURVE& x_mon_part, const CURVE& orig_crv)
	{ return x_mon_part.orientation() == orig_crv.orientation(); }

	bool intersect_right_of_pswp_given_A_leq_pswp_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const;

	bool A_touches_B_at_sweeppos_given_that_A_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);

	void mark_as_touching_at_p(const CRV_PNT& p, const CURVE& A, const CURVE& B);

	static bool A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B);

	static bool A_overlaps_B_given_they_share_three_points(const CURVE& A, const CURVE& B)
	{ return true; }
	
	list<CRV_PNT> intersections(const CURVE& A, const CURVE& B) const;

	static int compare_intersections_with_vertical_line_through_sweeppos
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);

	static void print_curve(ostream& os, const CURVE& crv);
	static void print_point(ostream& os, const CRV_PNT& p); 

private:
	bool x_mon_curve_wedge_contains_p(const CURVE& crv, const CRV_PNT& p) const;
	bool x_mon_curve_wedges_contain_p(const CURVE& A, const CURVE& B, const CRV_PNT& p) const
	{ return x_mon_curve_wedge_contains_p(A, p) && x_mon_curve_wedge_contains_p(B, p); }

	int  compute_intersection_curve_line
		(const CURVE& A, const rat_line& l, const CRV_PNT& sweeppos, const CURVE& B, 
		 bool not_strictly_right, CRV_PNT inters[2]) const;

	static curve_pair make_pair(const CURVE& c1, const CURVE& c2)
	{ return curve_pair(ID_Number(c1), ID_Number(c2)); }

private:
	typedef rational COORD;
	COORD Infinity;

	typedef two_tuple<CRV_PNT, CRV_PNT> intersect_result;

	mutable dictionary<curve_pair, intersect_result*> IntersectionCache;
	CRV_PNT TouchingIndicator;

#ifdef LEDA_CURVE_SWEEP_TIME
private:
	mutable unsigned TotalIntersections, CacheMiss;
	mutable timer IntersectionTimer, TimerA, TimerB;
#endif
}; // eof curve_sweep_traits_r_circle_segment

/// colour_sweep_traits_mc_r_circle_segment /////////////////////////////////////////////////

/*
	Two curves A and B are considered to have a common colour iff (Colour[A] & Colour[B]) != 0.
	Thus curve A could have two colours, e.g. 1 and 2; and B could have only colour 2.
	Then A and B have the common colour 2. 
	Two curves A and B with a common colour may not have common points except for common 
	endpoints.
	(Note: Colour[A] == 0 means that A has no colour at all. Two colourless segments may 
	intersect.)

	NOTE: "mc" in the class means "multi colour", i.e. every curve can have several colours.

	NOTE: We could turn this class into a template class which derives from a 
	      colourless sweep traits class.
*/

class __exportC colour_sweep_traits_mc_r_circle_segment : public curve_sweep_traits_r_circle_segment {
public:
	typedef curve_sweep_traits_r_circle_segment base;

public:
	colour_sweep_traits_mc_r_circle_segment() : Colour(0) {}

	void          set_colours(const map<CURVE, unsigned long>& colour_map) { Colour = colour_map; }
	void          set_colour(const CURVE& A, unsigned long col) { Colour[A] = col; }
	unsigned long get_colour(const CURVE& A) const { return Colour[A]; }

protected:
	CURVE get_lower_sentinel();
	CURVE get_upper_sentinel();

	void add_x_monotonous_parts(const CURVE& crv, list<CURVE>& x_monotonous_parts);

	CURVE make_endpoints_of_curve_identical(const CURVE& crv, 
	                                        const CRV_PNT& src, const CRV_PNT& tgt);

	bool intersect_right_of_pswp_given_A_leq_pswp_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const;

	bool A_touches_B_at_sweeppos_given_that_A_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);

	bool A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B) const;

private:
	map<CURVE, unsigned long> Colour;
}; // eof colour_sweep_traits_mc_r_circle_segment

/// colour_sweep_traits_sc_r_circle_segment /////////////////////////////////////////////////

/*
	Every curve has one colour. Two curves A and B with the same colour may not have common 
	points except for common endpoints.

	NOTE: "sc" in the class means "single colour", i.e. every curve has exactly one colour.

	NOTE: We could turn this class into a template class which derives from a 
	      colourless sweep traits class.
*/

class __exportC colour_sweep_traits_sc_r_circle_segment : public curve_sweep_traits_r_circle_segment {
public:
	typedef curve_sweep_traits_r_circle_segment base;

public:
	colour_sweep_traits_sc_r_circle_segment() : Colour(0) {}

	void set_colours(const map<CURVE, int>& colour_map) { Colour = colour_map; }
	void set_colour(const CURVE& A, int col) { Colour[A] = col; }
	int  get_colour(const CURVE& A) const { return Colour[A]; }

protected:
	void add_x_monotonous_parts(const CURVE& crv, list<CURVE>& x_monotonous_parts);

	CURVE make_endpoints_of_curve_identical(const CURVE& crv, 
	                                        const CRV_PNT& src, const CRV_PNT& tgt);

	bool intersect_right_of_pswp_given_A_leq_pswp_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const;

	bool A_touches_B_at_sweeppos_given_that_A_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);

	bool A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B) const;

private:
	map<CURVE, int> Colour;
}; // eof colour_sweep_traits_sc_r_circle_segment

/// curve_sweep_traits_rat_segment //////////////////////////////////////////////////////////

class __exportC curve_sweep_traits_rat_segment {
public:
	typedef rat_segment CURVE;
	typedef rat_point   CRV_PNT;

public:
	static int compare_intersections_with_sweepline_at(const CRV_PNT& p_sweep, 
		                                               const CURVE& A, const CURVE& B);
protected:
	void reset();
	void finish() {}

	void update_sentinels(const CURVE& crv);
	CURVE get_lower_sentinel() const;
	CURVE get_upper_sentinel() const;

	static void add_x_monotonous_parts(const CURVE& crv, list<CURVE>& x_monotonous_parts);

	static CURVE make_endpoints_of_curve_identical(const CURVE& crv, 
		                                           const CRV_PNT& src, const CRV_PNT& tgt);

	CURVE make_trivial_curve(const CRV_PNT& p) const { return CURVE(p,p); }

	static bool have_same_direction(const CURVE& x_mon_part, const CURVE& orig_crv)
	{ return x_mon_part.source() == orig_crv.source(); }

	static bool intersect_right_of_pswp_given_A_leq_pswp_leq_B
		(const CRV_PNT& sweeppos, const CURVE& A, const CURVE& B, CRV_PNT& inter);

	bool A_touches_B_at_sweeppos_given_that_A_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B) const
	{ return false; }

	void mark_as_touching_at_p(const CRV_PNT& p, const CURVE& A, const CURVE& B) {}

	static bool A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B);

	static bool A_overlaps_B_given_they_share_three_points(const CURVE& A, const CURVE& B)
	{ return true; }
	
	static list<CRV_PNT> intersections(const CURVE& A, const CURVE& B);

	static int compare_intersections_with_vertical_line_through_sweeppos
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);

	static void print_curve(ostream& os, const CURVE& crv);
	static void print_point(ostream& os, const CRV_PNT& p);

private:
	typedef rational COORD;
	COORD Infinity;
}; // eof curve_sweep_traits_rat_segment

#if LEDA_ROOT_INCL_ID == 520124
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
