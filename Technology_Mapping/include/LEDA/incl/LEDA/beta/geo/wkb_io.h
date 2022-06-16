/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  wkb_io.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.5 $  $Date: 2007/02/28 07:44:01 $

#ifndef WKB_IO_H
#define WKB_IO_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520434
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/gen_polygon.h>
#include <LEDA/coding/coder_base.h> // for uint32

LEDA_BEGIN_NAMESPACE

/// wkb_io //////////////////////////////////////////////////////////////////////////////////

/*{\Manpage {wkb_io} {} {Parser for well known binary format} {W}}*/

class __exportC wkb_io {
/*{\Mdefinition
The class |\Mname| provides methods for reading and writing geometries in the
well known binary format (wkb).
Every non-trivial generalized polygon from LEDA can be written in wkb format.
The method for reading supports the wkb types |Polygon| and |MultiPolygon|, 
i.e., those types that can be represented by the LEDA type |gen_polygon|.
}*/
public:
	enum wkbGeometryType {
		wkbAny = 0,
		wkbPoint = 1,
		wkbLineString = 2,
		wkbPolygon = 3,
		wkbMultiPoint = 4,
		wkbMultiLineString = 5,
		wkbMultiPolygon = 6,
		wkbGeometryCollection = 7
	};

	enum wkbByteOrder {
		wkbXDR = 0, // Big Endian
		wkbNDR = 1, // Little Endian

	#ifdef LITTLE_ENDIAN_MACHINE
		nativeByteOrder = wkbNDR,
	#else
		nativeByteOrder = wkbXDR,
	#endif
	};

	typedef unsigned char      byte;
	typedef coder_base::uint32 uint32;
	typedef double             float64;

public:
/*{\Mcreation}*/
	wkb_io();
	/*{\Mcreate creates an instance of type |\Mname|.}*/

	~wkb_io();

/*{\Moperations}*/

	bool read(const string& filename, gen_polygon& P);
	/*{\Mop reads the geometry stored in the given file and converts it to a 
	        generalized polygon |P|.}*/

	bool write(const string& filename, const gen_polygon& P);
	/*{\Mop writes the generalized polygon |P| to the given file.}*/

private:
	void clear();
	void error(const string& msg = string());

private:
	void correct_byte_order_uint32(uint32& val);
	void correct_byte_order_float64(float64& val);

	wkbByteOrder    read_wkbByteOrder();
	wkbGeometryType read_wkbType(wkbGeometryType expected_type = wkbAny);
	point           read_wkbPoint();
	polygon         read_wkbLinearRing();
	gen_polygon     read_wkbPolygon();
	gen_polygon     read_wkbMultiPolygon();

	gen_polygon     read_WKBPolygon();
	gen_polygon     read_WKBMultiPolygon();

	gen_polygon     read_WKBPolygonGeometry();

private:
	wkbByteOrder write_wkbByteOrder();
	void         write_wkbType(wkbGeometryType type);
	void         write_wkbPoint(const point& pnt);
	void         write_wkbLinearRing(const polygon& poly);
	void         write_wkbPolygon(const gen_polygon& P);
	void         write_wkbMultiPolygon(const list<gen_polygon>& L);

	void         write_WKBPolygon(const gen_polygon& P);
	void         write_WKBMultiPolygon(const list<gen_polygon>& L);

	void         write_WKBPolygonGeometry(const gen_polygon& P);

private:
	istream* Input;
	bool SwapBytes;

private:
	ostream* Output;
};

#if LEDA_ROOT_INCL_ID == 520434
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
