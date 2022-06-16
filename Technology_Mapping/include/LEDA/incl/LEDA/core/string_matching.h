/*******************************************************************************
+
+  LEDA 5.2
+
+
+  string_matching.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 511997
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// string_matching
//------------------------------------------------------------------------------

#include<LEDA/core/string.h>
#include<LEDA/core/list.h>
#include<LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {string_matching} {} {String Matching Algorithms} {M} }*/

#define EOS '\0'
#define ASIZE 256
#define UNDEFINED -1
#define REHASH(a, b, h) ((((h) - (a)*d) << 1) + (b))
#define WORD 32
#define XSIZE 33


enum alg {
	KARP_RABIN = 1,
	MORRIS_PRATT = 2,
	KNUTH_MORRIS_PRATT = 3,
	GALIL_SEIFERAS = 4,
	BOYER_MOORE = 5,
	TURBO_BOYER_MOORE = 6,
	TUNED_BOYER_MOORE = 7,
	ZHU_TAKAOKA = 8,
	BERRY_RAVINDRAN = 9,
	SMITH = 10,
	RAITA = 11,
	HORSPOOL = 12,
	BRUTE_FORCE = 13,
	SHIFT_OR = 14,
	REVERSE_COLUSSI = 15,
	QUICK_SEARCH = 16,
	REVERSE_FACTOR = 17,
	TURBO_REVERSE_FACTOR = 18,
	OPTIMAL_MISMATCH = 19
};

typedef struct patternScanOrder {
   int loc;
   char c;
} pattern;


struct _cell{
    int element; 
    struct _cell *next;
  };
  typedef struct _cell *List;


// the deterministic finite automaton


  struct _graph {
    int vertexNumber, 
        edgeNumber, 
        vertexCounter, 
        initial, 
        *terminal, 
        *target, 
        *suffixLink,
        *length, 
        *position, 
        *shift; 
  };
  typedef struct _graph *Graph; 
  typedef int boolean;
   

class __exportC string_matching {

/*{\Mdefinition
An instance $M$ of the data type |\Mname| is an object maintaining
a pattern and a string. It provides a collection of different algorithms
for computation of the exact string matching problem. Each function computes a list of all starting
positions of occurences of the pattern in the string.\\
The algorithms can be called explicitely by their name or implicitely by making an algorithm the
"current" algorithm (specified by the $setAlgorithm$-method)
and using functions $getFirstOccurence()$, $getNextOccurence()$, and $getAllOccurences()$.\\


Here is the list of the available parameters to specify the algorithm in
the $setAlgorithm(alg)$-method.
For each algorithm, the enumeration index gives the corresponding $int$ 
that can be used in the $setAlgorithm(int)$-method in order to specify
the algorithm.

\begin{enumerate}

\item
$KARP\_RABIN$
\item
$MORRIS\_PRATT$
\item
$KNUTH\_MORRIS\_PRATT$
\item
$GALIL\_SEIFERAS$
\item
$BOYER\_MOORE$
\item
$TURBO\_BOYER\_MOORE$
\item
$TUNED\_BOYER\_MOORE$
\item
$ZHU\_TAKAOKA$
\item
$BERRY\_RAVINDRAN$
\item
$SMITH$
\item
$RAITA$
\item
$HORSPOOL$
\item
$BRUTE\_FORCE$
\item
$SHIFT\_OR$
\item
$REVERSE\_COLUSSI$
\item
$QUICK\_SEARCH$
\item
$REVERSE\_FACTOR$
\item
$TURBO\_REVERSE\_FACTOR$
\item
$OPTIMAL\_MISMATCH$

\end{enumerate} 

Remark: This data type is experimental. It can significantly change
in future versions. }*/

private:
	string x; // the Pattern
	string y; // the String

	int m; // length of x
	int n; // length of y

	int mode; // mode = -1; all occurences; mode = unsigned int i: first occurence from position i;

	list<int> result; // contains list of all occurences
	int next_occurence; // contains result for firstOccurence and nextOccurence

	alg current;

	int kgs, pgs, pgs1, pgs2, qgs, qgs1, qgs2, sgs; // Variables for Galil Seiferas

	void preKmp(char* x, array<int> &kmpNext);
	void preAut(char* x, Graph aut);
	int preSo(char* x, unsigned int S[]);
	void preMp(char *x, array<int> &mpNext);
	void preRc(char *x, array<int> &h, int rcBc[ASIZE][XSIZE], array<int> &rcGs);
	void preBmBc(char *x, int bmBc[]);
	void preBmGs(char *x, array<int> &bmGs);
	void preZtBc(char *x, int ztBc[ASIZE][ASIZE]);
	void preBrBc(char *x, int brBc[ASIZE][ASIZE]);
	void preQsBc(char *x, int qsBc[]);
 	int matchShift(char *x, int ploc, int lshift, pattern *pat);
	void preAdaptedGs(char *x, array<int> &adaptedGs, pattern *pat);
	void computeMinShift(char *x);
	void suffixes(char *x, array<int> &suff);
	char *reverse(char *x);
	void oracle(char *x, array<char> &T, array<List> &L);
	int maxSuf(char *x, int *p);
	int maxSufTilde(char *x, int *p);
	int attempt(char *y, char *x, int start, int wall);
	void search(char* xad, char* yad);
	void parse(char* xad, char* yad);
	void newP2(char* xad, char* yad);
	void newP1(char* xad, char* yad);



public:

/*{\Mcreation}*/

	string_matching() {
	mode = -1;
	}
/*{\Mcreate  creates an instance $M$ of type |\Mname|.}*/

	string_matching(const string& s1, const string& s2)
	{
		x = s1;
		m = s1.length();
		y = s2;
		n = s2.length();
		mode = -1;
	}
/*{\Mcreate  creates an instance $M$ of type |\Mname| and initializes the pattern
to $s1$ and the string to $s2$.}*/

	~string_matching() {}

/*{\Moperations 1.2 5 }*/


	void readPattern();

	void readString();

	void setPattern(const string& str);
/*{\Mop        sets the pattern to $str$. }*/
	void setString(const string& str);
/*{\Mop        sets the string to $str$. }*/
	string getPattern();
/*{\Mop        returns the pattern. }*/
	string getString();
/*{\Mop        returns the string. }*/
	void setStrings(const string& str1, const string& str2);
/*{\Mop        sets the pattern to $str1$ and the string to $str2$. }*/

	void reportResult();
	void setAlgorithm(alg A);
/*{\Mop        sets the current algorithm to $A$ (see table above). }*/
	void setAlgorithm(int A);
/*{\Mop        sets the current algorithm to $A$ by index (see table above). }*/
	list<int> getAllOccurences();
/*{\Mop        executes the current algorithm reporting all occurences of the pattern in the string. }*/
	int getFirstOccurence();
/*{\Mop        executes the current algorithm reporting the first occurence of the pattern in the string. If there is none, -1 is returned. }*/
	int getNextOccurence(int i);
/*{\Mop        executes the current algorithm reporting the first occurence after position $i$ of the pattern in the string. 
If there is none, -1 is returned. String positions start with $0$.}*/
	list<int> getResult() const;
/*{\Mop        returns the list of results after a computation. }*/
	void karp_rabin();
/*{\Mop        applies the algorithm of Karp and Rabin. }*/
	void morris_pratt();
/*{\Mop        applies the algorithm of Morris and Pratt. }*/
	void knuth_morris_pratt();
/*{\Mop        applies the algorithm of Knuth, Morris and Pratt. }*/
	void galil_seiferas();
/*{\Mop        applies the algorithm of Galil and Seiferas. }*/
	void boyer_moore();
/*{\Mop        applies the algorithm of Boyer and Moore. }*/
	void turbo_boyer_moore();
/*{\Mop        applies the turbo algorithm of Boyer and Moore. }*/
	void tuned_boyer_moore();
/*{\Mop        applies the tuned algorithm of Boyer and Moore. }*/
	void zhu_takaoka();
/*{\Mop        applies the algorithm of Zhu and Takaoka. }*/
	void berry_ravindran();
/*{\Mop        applies the algorithm of Berry and Ravindran. }*/
	void smith();
/*{\Mop        applies the algorithm of Smith. }*/
	void raita();
/*{\Mop        applies the algorithm of Raita. }*/
	void horspool();
/*{\Mop        applies the algorithm of Horspool. }*/
	void brute_force();
/*{\Mop        applies the naive brute force algorithm. }*/
	void shift_or();
/*{\Mop        applies the shift-or algorithm. }*/
	void reverse_colussi();
/*{\Mop        applies the reverse colussi algorithm. }*/
	void quick_search();
/*{\Mop        applies the quick search algorithm. }*/
	void reverse_factor();
/*{\Mop        applies the reverse factor algorithm. }*/
	void turbo_reverse_factor();
/*{\Mop        applies the turbo reverse colussi algorithm. }*/
	void optimal_mismatch();	
/*{\Mop        applies the optimal mismatch algorithm. }*/
	void maximal_shift();	
	void skip_search();
	void kmp_skip_search();
	void automaton();
	void forward_dawg_matching();
	void backward_oracle_matching();
	void two_way();
	void on_ordered_alphabet();
	void not_so_naive();
	void backward_nondeterministic_dawg_matching();
};


#if LEDA_ROOT_INCL_ID == 511997
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
