/*******************************************************************************
+
+  LEDA 5.2
+
+
+  distance.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef DISTANCE_H
#define DISTANCE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 511999
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// distance
//------------------------------------------------------------------------------



#include<LEDA/core/string.h>
#include<LEDA/core/array2.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {distance} {} {String Distance Function} {D} }*/

const unsigned int cost_del = 1;
const unsigned int cost_ins = 1;
const unsigned int cost_sub = 1;

enum alg {
		HAMMING = 1,
		LEVENSHTEIN = 2,
		DAMERAU_LEVENSHTEIN = 3,
		GENERAL_LEVENSHTEIN = 4
};

class __exportC distance {

/*{\Mdefinition
An instance $D$ of the data type |\Mname| is an object maintaining
two strings and providing several string distance functions.\\
The Hamming distance between two strings of equal length is the number 
of positions for which the corresponding symbols are different.\\
The Levenshtein distance or edit distance between two strings is given 
by the minimum number of operations needed to transform one string into 
the other, where an operation is an insertion, deletion, or substitution 
of a single character.\\
The General Levenshtein distance is given by the minimum sum of the costs
over a sequence of operations needed to transform one string into the
other, where operation is an insertion, deletion, or substitution and
a cost is assigned to each of the operations.\\
The Damerau-Levenshtein distance is an extension of Levenshtein distance 
that counts transposition as a single edit operation. Strictly speaking, 
the Damerau-Levenshtein distance is equal to the minimal number of 
insertions, deletions, substitutions and transpositions needed to transform 
one string into the other.\\
The algorithms can be called explicitely by their name or implicitely by 
making an algorithm the "current" algorithm (method $setAlgorithm(.)$ and 
using the $run()$-method.\\

Here is the list of the available parameters to specify the algorithm in
the $setAlgorithm(alg)$-method in order to call it by the $run()$-method.
For each algorithm, the enumeration index gives the corresponding $int$ 
that can be used in the $setAlgorithm(int)$-method in order to specify
the algorithm.

\begin{enumerate}

\item
$HAMMING$

\item
$LEVENSHTEIN$

\item
$DAMERAU\_LEVENSHTEIN$

\item
$GENERAL\_LEVENSHTEIN$

\end{enumerate} 

Remark: This data type is experimental. It can significantly change
in future versions. }*/

private:
	string x; // First String
	string y; // Second String

	int m; // length of x
	int n; // length of y

	alg current; // chosen algorithm

public:

/*{\Mcreation}*/

	distance() { }
/*{\Mcreate  creates an instance $D$ of type |\Mname|.}*/

	distance(const string& s1, const string& s2)
	{
		x = s1;
		m = s1.length();
		y = s2;
		n = s2.length();
	}
/*{\Mcreate  creates an instance $D$ of type |\Mname| and initializes the two strings to $s1$ resp. $s2$.}*/

	~distance(){}


/*{\Moperations 1.2 5 }*/

	int hamming();
/*{\Mop        returns the Hamming distance of two strings. If the strings
have different lengths, -1 is returned. }*/
	int levenshtein();
/*{\Mop        returns the Levenshtein distance of two strings. }*/
	int damerau_levenshtein();
/*{\Mop        returns the Damerau Levenshtein distance of two strings. }*/
	unsigned int general_levenshtein();
/*{\Mop        returns the General Levenshtein distance of two strings. }*/

	void readFirstString();

	void readSecondString();

	void setAlgorithm(alg A);
/*{\Mop        sets the current algorithm to $A$ (see table above). }*/
	void setAlgorithm(int A);
/*{\Mop        sets the current algorithm to $A$ by index (see table above). }*/

	int run();
/*{\Mop        executes the current algorithm. }*/

	void setFirstString(const string &str);
/*{\Mop        assigns $str$ to the first string. }*/
	void setSecondString(const string &str);
/*{\Mop        assigns $str$ to the second string. }*/
	string getFirstString();
/*{\Mop        returns first string. }*/
	string getSecondString();
/*{\Mop        returns second string. }*/

};


#if LEDA_ROOT_INCL_ID == 511999
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
