/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  dxf_io.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:01 $

#ifndef DXF_IO_H
#define DXF_IO_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520435
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE

/// dxf_io //////////////////////////////////////////////////////////////////////////////////

class __exportC dxf_io {
public:
	enum input_mode { IM_only_curved, IM_only_straight, IM_mixed };

public:
	dxf_io() : Verify(false), RespectOrientation(false), 
		       Input(0), DistanceForArcApprox(1e-3), Output(0)
	{ clear(); }

// global
	void clear();

	bool get_verify_flag() const { return Verify; }
	void set_verify_flag(bool flag = true) { Verify = flag; }

	bool get_respect_orientation_flag() const { return RespectOrientation; }
	void set_respect_orientation_flag(bool flag = true) { RespectOrientation = flag; }

	double get_distance_for_arc_approximation() const { return DistanceForArcApprox; }
	void   set_distance_for_arc_approximation(double d);

	bool has_error() const { return Error; }
	string get_error_message() const { return ErrorMsg; }

	bool has_warning() const { return Warning; }
	string get_warning_message() const { return WarningMsg; }

	void get_statistics(unsigned& num_straight_segs, unsigned& num_arc_segs, unsigned& num_circles, unsigned& num_approx_segs) const;
	void print_statistics(ostream& out = cout) const;

	void compute_bounding_box(real& xmin, real& ymin, real& xmax, real& ymax) const;

// reading
	void read(const string& filename, input_mode mode = IM_mixed) { read(filename, mode, mode); }
	void read(const string& filename, input_mode mode_polys, input_mode mode_segs);
	void read(const string& filename, list<r_circle_polygon>& L);
	void read(const string& filename, list<rat_polygon>& L);

	const list<r_circle_polygon>& get_curved_polygons() const { return CurvedPolygons; }
	const list<rat_polygon>&      get_straight_polygons() const { return StraightPolygons; }
	const list<rat_circle>&       get_circles() const { return Circles; }
	const list<r_circle_segment>& get_curved_segments() const { return CurvedSegments; }
	const list<rat_segment>&      get_straight_segments() const { return StraightSegments; }

	void convert_straight_entities_to_curved_entities();
	void convert_curved_entities_to_straight_entities();

	void convert_circles_to_curved_polygons();
	void convert_all_segments_to_polygons(real arc_eps = 0);

	void convert_curved_segments_to_polygons(real arc_eps = 0);
	void convert_straight_segments_to_polygons();

// static conversion
	static void convert_polygons_from_straight_to_curved(const list<rat_polygon>& straight, list<r_circle_polygon>& curved);
	static void convert_polygons_from_straight_to_curved(const list<rat_gen_polygon>& straight, list<r_circle_gen_polygon>& curved);
	static void convert_segments_from_straight_to_curved(const list<rat_segment>& straight, list<r_circle_segment>& curved);

	static int  convert_polygons_from_curved_to_straight(const list<r_circle_polygon>& curved, list<rat_polygon>& straight, double dist);
	static int  convert_polygons_from_curved_to_straight(const list<r_circle_gen_polygon>& curved, list<rat_gen_polygon>& straight, double dist);
	static int  convert_segments_from_curved_to_straight(const list<r_circle_segment>& curved, list<rat_segment>& straight, double dist);

// writing
	void writing_begin(const string& filename, string layer = "0");
	void writing_append(const r_circle_polygon& p, string layer = string());
	void writing_append(const list<r_circle_polygon>& L, string layer = string());
	void writing_append(const r_circle_gen_polygon& gp, string layer = string());
	void writing_append(const rat_polygon& p, string layer = string());
	void writing_append(const list<rat_polygon>& L, string layer = string());
	void writing_append(const rat_gen_polygon& gp, string layer = string());
	void writing_append(const r_circle_segment& s, string layer = string());
	void writing_append(const list<r_circle_segment>& L, string layer = string());
	void writing_end();

	void write(const string& filename, const list<r_circle_polygon>& L, string layer = "0");
	void write(const string& filename, const r_circle_gen_polygon& p, string layer = "0");
	void write(const string& filename, const list<r_circle_gen_polygon>& GPs, string layer = "0");
	void write(const string& filename, const list<rat_polygon>& L, string layer = "0");
	void write(const string& filename, const rat_gen_polygon& p, string layer = "0");
	void write(const string& filename, const list<rat_gen_polygon>& GPs, string layer = "0");

private: // global
	void clear_error();
	void error(const string& msg = string());
	void warn(const string& msg = string());
	void clear_statistics();

private: // input related
	bool has_input() const { return Input->good(); }
	bool input_init(const string& filename);
	bool input_pair();
	bool input_polyline();
	bool input_lwpolyline();
	bool input_line();
	bool input_circle();
	bool input_arc();
    bool input_read(const string& filename);

	static rat_point point_near_circle(const circle& c, double angle_in_deg, bool& on_circle);

private: // output related
	void output_pair(int code, const string& value);
	void output_pair(int code, double value);
	void output_pair(int code, int value);
	void output_pair() { output_pair(Code, Value); }
	void output_layer();

	void output_begin();
	void output_end();

	void output_section_begin();
	void output_section_end();

	void output_entities_begin();
	void output_entities_end();

	void output_polygon(const r_circle_polygon& p);

	void output_closed_polyline(const r_circle_polygon& p);
	void output_closed_polyline(const rat_polygon& p);

	void output_curved_segment(const r_circle_segment& s);

	void output_vertex(const point& p, double arc_parameter = 0.0);
	void output_line(const point& p, const point& q);

	void output_circle(const rat_circle& c);
	void output_arc(const rat_circle& c, double start_angle_rad, double end_angle_rad);

private:
	bool Error;   string ErrorMsg;
	bool Warning; string WarningMsg;
	bool Verify;
	bool RespectOrientation;

	unsigned NumStraightSegments;
	unsigned NumArcSegments;
	unsigned NumCircles;
	unsigned NumApproxSegments;

	int Code;
	string Value;

private: // input related
	input_mode InputModePolygons, InputModeSegments;
	istream* Input;
	list<r_circle_polygon> CurvedPolygons;
	list<rat_polygon>      StraightPolygons;
	list<rat_circle>       Circles;
	list<r_circle_segment> CurvedSegments;
	list<rat_segment>      StraightSegments;
	double                 DistanceForArcApprox;

private: // output related
	ostream* Output;
	string Layer;
};

#if LEDA_ROOT_INCL_ID == 520435
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
