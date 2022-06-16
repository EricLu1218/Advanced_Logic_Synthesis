/*******************************************************************************
+
+  LEDA 5.2
+
+
+  alignment.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 511998
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// alignment
//------------------------------------------------------------------------------


#include<LEDA/core/string.h>
#include<LEDA/core/score_matrix.h>
#include<LEDA/core/map.h>
#include<LEDA/core/map2.h>
#include<LEDA/core/array.h>
#include<LEDA/core/array2.h>
#include<LEDA/core/tuple.h>
#include<LEDA/core/sortseq.h>
#include<limits.h>

LEDA_BEGIN_NAMESPACE


#define EOS '\0'
#define ASIZE 256
#define fft four_tuple<int, int, int, int>

/*{\Manpage {alignment} {} {Alignment Functions} {A} }*/

class __exportC alignment {

/*{\Mdefinition
An instance $A$ of the data type $alignment$ is an object maintaining
two strings, a $score\_matrix$, a penalty value for starting a gap and a
penalty value for extending an already started gap. It provides several
functions for global and local alignment computation. The 
result of an alignment computation is stored in the object.\\
Remark: This data type is experimental. It can significantly change
in future versions. }*/

private:
	string alignmentA;
	string alignmentB;
	string aligner;

	string inputA;
	string inputB;

	score_matrix scores;

	// Lengths of strings
	int m, n;
	int x_ind, y_ind; //indices for longest common substring

	// quadratic space alignments
	array2<int> *d;
	// linear space alignments and affine penalties
	array2<fft > *td;
	array2<fft > *td1;

	// start and end of the alignment
	int min_m, max_m, min_n, max_n;
	// alignment Score
	int al_score;


	// gap_penalty and incremental gap penalty  ES FEHLEN GET/SET FUNCTIONS
	int gap_penalty;
	int gap_inc_penalty;

	int PENALTY; // constant penalty

	int M_INFTY;
	// negative infinity
	int NEG_INFTY;

	// backtracking for alignment computation
	void compute_alignment(int i, int j);
	void compute_aff_alignment(int i, int j);

public:

/*{\Mcreation}*/

	alignment()
	{ 
		NEG_INFTY = - INT_MAX;
		min_m = 0;
		min_n = 0;
		m = 0;
		n = 0;
		max_m = m;
		max_n = n;
	}
/*{\Mcreate  creates an instance $A$ of type |\Mname|.}*/

	alignment(const score_matrix& score)
	{
		scores = score;

		NEG_INFTY = - INT_MAX;
		min_m = 0;
		min_n = 0;
		m = 0;
		n = 0;
		max_m = m;
		max_n = n;
	}

/*
\Mtext alignment(const score_matrix score);
*/
/*{\Mcreate  creates an instance $A$ of type |\Mname| and initializes the
score matrix to $score$.}*/

	alignment(const score_matrix& score, const string& s1, const string& s2)
	{
		scores = score;
		inputA = s1;
		m = s1.length();
		inputB = s2;
		n = s2.length();

		NEG_INFTY = - INT_MAX;
		min_m = 0;
		min_n = 0;
		max_m = m;
		max_n = n;
	}

/*
\Mtext alignment(const score_matrix score, const string& s1, const string& s2);
*/
/*{\Mcreate  creates an instance $A$ of type |\Mname| and initializes the
score matrix to $score$, and the strings to $s1$ and $s2$.}*/

	alignment(const string& s1, const string& s2)
	{
		inputA = s1;
		m = s1.length();
		inputB = s2;
		n = s2.length();

		NEG_INFTY = - INT_MAX;
		min_m = 0;
		min_n = 0;
		max_m = m;
		max_n = n;
	}

/*
alignment(const string& s1, const string& s2);
*/
/*{\Mcreate  creates an instance $A$ of type |\Mname| and initializes the
strings to $s1$ and $s2$.}*/

	~alignment() {}

/*{\Moperations 1.2 5 }*/

	void readFirstString();

	void readSecondString();

	void setStringA(const string &x);
/*{\Mop        sets the first string to $x$. }*/
	void setStringB(const string &x);
/*{\Mop        sets the second string to $x$. }*/
	string getStringA();
/*{\Mop        returns the first string. }*/
	string getStringB();
/*{\Mop        returns the second string. }*/
	string getAlA();
/*{\Mop        returns the first alignment string, that is the first string 
with its gaps after an alignment computation. }*/
	string getAlB();
/*{\Mop        returns the second alignment string, that is the second string 
with its gaps after an alignment computation. }*/
	string getAlInfo();
/*{\Mop        shows the correspondence information for each position in the two
alignment strings. Its i-th position is "/", if the letters
are identical, "$X$" if the are different but they are matched and " " if there
is a gap in one of the alignment strings. }*/

	bool isInAlphabet(char c);
	bool checkStrings();

	void setGapPenalty(int x);
/*{\Mop        sets the penalty for a gap to $x$. }*/
	void setGapIncPenalty(int x);
/*{\Mop        sets the penalty for extending an existing gap by one position to $x$. }*/
	int getGapPenalty();
/*{\Mop        returns the penalty for a gap. }*/
	int getGapIncPenalty();
/*{\Mop        returns the penalty for a gap extension of one position. }*/




	void global_nw();
/*{\Mop        runs a global alignment algorithm based on Needleman and Wunsch, where there
is a penalty $x$ for each gap proportional to its length. $x$ can be set by $setGapPenalty$.
The algorithm requires quadratic space.}*/

	void local_sw();
/*{\Mop        runs a local alignment algorithm based on Smith and Waterman, where there
is a penalty $x$ for each gap proportional to its length. $x$ can be set by $setGapPenalty$.
The algorithm requires quadratic space.}*/

	void global_aff_nw();
/*{\Mop        runs a global alignment algorithm based on Needleman and Wunsch, where there
is a penalty $x + k * y$ for each gap, where $k$ is the length of the gap. $x$ can be set by $setGapPenalty$
and $x$ by $setGapIncPenalty$. The algorithm requires 
quadratic space.}*/

	void local_aff_sw();
/*{\Mop        runs a local alignment algorithm based on Smith and Waterman, where there
is a penalty $x + k * y$ for each gap, where $k$ is the length of the gap. $x$ can be set by $setGapPenalty$
and $x$ by $setGapIncPenalty$. The algorithm requires quadratic space.}*/


	unsigned int longest_common_substring();
/*{\Mop        returns the length of the longest common substring of the two strings.}*/

	unsigned int longest_common_subsequence();
/*{\Mop        returns the length of the longest common subsequence of the two strings and 
stores them aligned in the alignment strings.}*/
	
};


#if LEDA_ROOT_INCL_ID == 511998
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
