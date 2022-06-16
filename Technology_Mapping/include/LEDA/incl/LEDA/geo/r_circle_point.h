/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  r_circle_point.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:12 $


#ifndef LEDA_R_CIRCLE_POINT_H
#define LEDA_R_CIRCLE_POINT_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520115
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/real_circle.h>
#include <LEDA/geo/rat_circle.h>
#include <LEDA/geo/rat_line.h>

//#define LEDA_R_CIRCLE_SAVE_MEM

LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// r_circle_point:   real point on rational circle
//------------------------------------------------------------------------------

/// r_circle_point ///////////////////////////////////////////////////////////////////////////

/*{\Manpage {r_circle_point} {} {Point on Rational Circle} {p}}*/

class __exportC r_circle_point : public real_point {
/*{\Mdefinition
An instance |\Mvar| of type |\Mtype| is a point in the two-dimensional plane
that can be obtained by intersecting a rational circle $c$ and a rational 
line $l$ (cf.~Sections \ref{rat_circle} and \ref{rat_line}).
Note that $c$ and $l$ may intersect in two points $p_1$ and $p_2$. Assume that
we order these intersections along the (directed) line $l$.
Then |\Mvar| is uniquely determined by a triple $(c,l,|which|)$, where 
|which| is either |first| or |second|.
Observe that the coordinates of |\Mvar| are in general non-rational numbers 
(because their computation involves square roots). 
Therefore the class |\Mtype| is derived from |real_point| (see 
Section~\ref{real_point}), which means that all operations of |real_point|
are available.
}*/
public:
	typedef real_point base;

/*{\Mtypes}*/
	enum tag { first = 1, second = 2};
	/*{\Menum used for selecting between the two possible intersections of 
	          a circle and a line.}*/

public:
/*{\Mcreation}*/

#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point()
	: base(real_point_for_default_ctor()), 
	  SupportCircle(circle_for_default_ctor()),
	  SupportLine(line_for_default_ctor()),
	  RatPoint(dummy_rat_point())
	{}
#else
	r_circle_point() : base(real_point_for_default_ctor()), RatPoint(dummy_rat_point()) {}
#endif
	/*{\Mcreate creates an instance |\Mvar| initialized to the point (0,0).}*/

	r_circle_point(const rat_point& rat_pnt);
	/*{\Mcreate creates an instance |\Mvar| initialized to the rational 
	point |rat_pnt|.}*/

	r_circle_point(const point& pnt);
	/*{\Mcreate creates an instance |\Mvar| initialized to the  
	point |pnt|.}*/

	/*{\Moptions nextwarning=no }*/
#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point(const rat_circle& c, const rat_line& l, tag which)
	 : base(real_point_for_default_ctor()), SupportCircle(c), SupportLine(l), RatPoint(dummy_rat_point())
	{ intersection(c, l, which, (base&)*this); /*assert(intersection(c, l, which, (base&)*this));*/ }
#else
	r_circle_point(const rat_circle& c, const rat_line& l, tag which)
	 : base(real_point_for_default_ctor()), RatPoint(dummy_rat_point())
	{ intersection(c, l, which, (base&)*this); }
#endif
	/*{\Mcreate creates an instance |\Mvar| initialized to the point 
	determined by $(c,l,|which|)$ (see above).}*/
	//\precond $c$ and $l$ intersect

	/*{\Moptions nextwarning=no }*/
#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point(const real_point& rp, const rat_circle& c, const rat_line& l, tag which)
	 : base(rp), SupportCircle(c), SupportLine(l), RatPoint(dummy_rat_point())
	{ /*assert(r_circle_point(c,l,which) == *this); */ }
#else
	r_circle_point(const real_point& rp, const rat_circle& c, const rat_line& l, tag which)
	 : base(rp), RatPoint(dummy_rat_point())
	{}
#endif
	/*{\Mcreate creates an instance |\Mvar| initialized to the real point 
	|rp|.\\ \precond |rp| is the point described by $(c,l,|which|)$.}*/

#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point(const r_circle_point& p)
	 : base(p), SupportCircle(p.SupportCircle), SupportLine(p.SupportLine), RatPoint(p.RatPoint) {}

	const r_circle_point& operator=(const r_circle_point& p)
	{ base::operator=(p); SupportCircle = p.SupportCircle; SupportLine = p.SupportLine; RatPoint = p.RatPoint; return *this; }
#else
	r_circle_point(const r_circle_point& p) : base(p), RatPoint(p.RatPoint) {}

	const r_circle_point& operator=(const r_circle_point& p)
	{ base::operator=(p); RatPoint = p.RatPoint; return *this; }
#endif

/*{\Moperations}*/
	void normalize();
	/*{\Mop simplifies the internal representation of |\Mvar|.}*/

	bool verify(bool report_errors = true) const;

#ifndef LEDA_R_CIRCLE_SAVE_MEM
	rat_circle supporting_circle() const;
	/*{\Mop returns a rational circle passing through |\Mvar|.}*/

	rat_line supporting_line() const;
	/*{\Mop returns a rational line passing through |\Mvar|.}*/

	tag which_intersection() const // not a constant time operation!
	{ return which_intersection(supporting_circle(), supporting_line(), *this); }
	/*{\Mop returns whether |\Mvar| is the first or the second intersection 
	of the supporting circle and the supporting line.}*/
#endif

	bool is_rat_point() const { return !identical(RatPoint, dummy_rat_point()); }
	/*{\Mop returns true, if |\Mvar| can be converted to |rat_point|.
	(The value false means ``do not know''.)}*/

	const rat_point& to_rat_point() const { return RatPoint; }
	/*{\Mop converts |\Mvar| to a |rat_point|.\\
	\precond |is_rat_point| returns true.}*/

	rat_point approximate_by_rat_point() const;
	/*{\Mop approximates |\Mvar| by a |rat_point|.}*/

	r_circle_point round(int prec = 0) const;
	/*{\Mop returns a rounded representation of |\Mvar|. (experimental) }*/


	r_circle_point translate(rational dx, rational dy) const;
	/*{\Mopl    returns |\Mvar| translated by vector $(dx,dy)$.}*/

	r_circle_point translate(const rat_vector& v) const { return translate(v[0], v[1]); }
	/*{\Mop     returns |\Mvar| translated by vector $v$.}*/

	r_circle_point operator+(const rat_vector& v) const { return translate(v); }
	/*{\Mbinop  returns |\Mvar| translated by vector $v$.}*/

	r_circle_point operator-(const rat_vector& v) const { return translate(-v); }
	/*{\Mbinop  returns |\Mvar| translated by vector $-v$.}*/

	r_circle_point rotate90(const rat_point& q, int i=1) const;
	/*{\Mopl    returns |\Mvar| rotated about $q$ by an angle of $i\times 90$ 
				degrees. If $i > 0$ the rotation is counter-clockwise otherwise
				it is clockwise. }*/

	r_circle_point reflect(const rat_point& p, const rat_point& q) const;
	/*{\Mop     returns |\Mvar| reflected  across the straight line passing
				through $p$ and $q$.}*/

	r_circle_point reflect(const rat_point& p) const;
	/*{\Mop     returns |\Mvar| reflected  across point $p$.}*/

public:

	static bool intersection(const rat_circle& c, const rat_line& l, tag which, real_point& p);
	/*{\Mstatic checks whether $(c,l,|which|)$ is a valid triple, if so the 
	corresponding point is assigned to the |real_point| |p|.}*/

	static bool intersection(const rat_circle& c, const rat_line& l, tag which, r_circle_point& p);
	/*{\Mstatic same as above, except for the fact that |p| is of type |\Mtype|.}*/

	static int intersection(const rat_circle& c, const rat_line& l, 
							two_tuple<r_circle_point,r_circle_point>& inters);

	static tag which_intersection(const rat_circle& c, const rat_line& l, const r_circle_point& p);

	static void write_point(ostream& os, const r_circle_point& p, const rat_circle& c, bool write_circle);
	static void read_point(istream& is, r_circle_point& p, const rat_circle& c);

	static int cmp_xy(const r_circle_point& p1, const r_circle_point& p2)
	{
		return (p1.is_rat_point() && p2.is_rat_point())
			? rat_point::cmp_xy(p1.to_rat_point(), p2.to_rat_point())
			: real_point::cmp_xy(p1, p2);
	}

	static int cmp_x(const r_circle_point& p1, const r_circle_point& p2)
	{
		return (p1.is_rat_point() && p2.is_rat_point())
			? rat_point::cmp_x(p1.to_rat_point(), p2.to_rat_point())
			: real_point::cmp_x(p1, p2);
	}

	static int cmp_y(const r_circle_point& p1, const r_circle_point& p2)
	{
		return (p1.is_rat_point() && p2.is_rat_point())
			? rat_point::cmp_y(p1.to_rat_point(), p2.to_rat_point())
			: real_point::cmp_y(p1, p2);
	}

private:
#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point(const real_point& rp, const rat_circle& c, const rat_line& l)
		: base(rp), SupportCircle(c), SupportLine(l), RatPoint(dummy_rat_point()) {}
		// special constructor for movement operations ...
#else
	r_circle_point(const real_point& rp) : base(rp), RatPoint(dummy_rat_point()) {}
#endif

	static const real_point& real_point_for_default_ctor();
	static const rat_circle& circle_for_default_ctor();
	static const rat_line&   line_for_default_ctor();
	static const rat_point&  dummy_rat_point();

private:
	const real_point& operator=(const real_point&); // forbidden

private:
#ifndef LEDA_R_CIRCLE_SAVE_MEM
	mutable rat_circle SupportCircle; // a rational circle containing the real point
	mutable rat_line   SupportLine;   // a rational line passing through the real point
#endif
	        rat_point  RatPoint;      // if *this is a rat_point RatPoint stores it

public:
	LEDA_MEMORY(r_circle_point);
};

COMPARE_DECL_PREFIX
inline int DEFAULT_COMPARE(const r_circle_point& p1, const r_circle_point& p2)
{
	return (p1.is_rat_point() && p2.is_rat_point())
		? compare(p1.to_rat_point(), p2.to_rat_point())
		: compare((const r_circle_point::base&) p1, (const r_circle_point::base&) p2);
}

inline int orientation(const r_circle_point& a, const r_circle_point& b, const r_circle_point& c)
{
	return (a.is_rat_point() && b.is_rat_point() && c.is_rat_point())
		? orientation(a.to_rat_point(), b.to_rat_point(), c.to_rat_point())
		: orientation((const real_point&) a, (const real_point&) b, (const real_point&) c);
}

__exportF ostream& operator<<(ostream& os, const r_circle_point& p);
__exportF istream& operator>>(istream& is, r_circle_point& p);

inline const char* leda_tname(const r_circle_point*) { return "r_circle_point"; }


#if LEDA_ROOT_INCL_ID == 520115
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
