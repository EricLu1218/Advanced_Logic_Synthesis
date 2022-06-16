/*******************************************************************************
+
+  LEDA 5.2
+
+
+  score_matrix.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef SCORE_MATRIX_H
#define SCORE_MATRIX_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 511995
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// score_matrix
//------------------------------------------------------------------------------


#include<LEDA/core/string.h>
#include<LEDA/core/alphabet.h>
#include<LEDA/core/map2.h>
#include<float.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {score_matrix} {} {Score Matrix} {S} }*/

class __exportC score_matrix {

/*{\Mdefinition
An instance $S$ of the data type |\Mname| is an object required by 
alignment computations. The object contains an alphabet representing residues and 
maintains a table that assigns a score to any pair of aligned residues. In addition it
maintains the gap penalty values.\\
Remark: This data type is experimental. It can significantly change
in future versions. }*/

private:
	map2<int, int, int> score;
	int gap_penalty, gap_inc_penalty;
	alphabet *alpha;


public:
/*{\Mcreation}*/
	score_matrix(int defmatch = 1, int defmismatch = -1)
	{
		alpha = new alphabet("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		for (int i = int('A'); i < int('Z'); i++)
			for (int j = int('A'); j < int('Z'); j++)
				if (i==j)
					score(i, j) = 1;
				else
					score(i, j) = -1;
	}
/*{\Mcreate  creates an instance $S$ of type |\Mname|. The alphabet is set to the
standard alphabet consisting of the capital letters A - Z. The scores are initialized
to $defmatch$ if the letters are idendical (1 by default), to $defmismatch$ otherwise
(-1 by default).}*/

	score_matrix(alphabet& alt, int defmatch = 1, int defmismatch = -1) {
	alpha = &alt;
	int length = alt.getLength();
	for (int i = 0; i < length; i++)
	for (int j = 0; j < length; j++)
	if (i==j)
			score(int(alt.getLetter(i)), int(alt.getLetter(j))) = defmatch;
		else
			score(int(alt.getLetter(i)), int(alt.getLetter(j))) = defmismatch;
}
	
/*{\Mcreate  creates an instance $S$ of type |\Mname|. The alphabet is set to $alt$.
The scores are initialized
to $defmatch$ if the letters are idendical (1 by default), to $defmismatch$ otherwise
(-1 by default).}*/

	~score_matrix() {
		score.clear();
	}

/*{\Moperations 1.2 5 }*/	

	void setScore(char a, char b, int sc);
/*{\Mop        sets the score for $a$ and $b$ to $sc$.\\
\precond $a$ and $b$ are in the score alphabet. Otherwise an exception is thrown.}*/

	int getScore(char a, char b) const;
/*{\Mop        returns the score for $a$ and $b$.\\
\precond $a$ and $b$ are in the score alphabet. Otherwise an exception is thrown.}*/

	bool hasScore(char a, char b) const;
/*{\Mop        returns $bool$ if score is defined for $a$ and $b$, otherwise $false$.}*/

	void setScoreMatch(int sc);
/*{\Mop        sets the score for all pairs $a$ and $a$ to $sc$.}*/
	void setScoreMismatch(int sc);
/*{\Mop        sets the score for all pairs $a$ and $b$ with $a$ != $b$ to $sc$.}*/

	void setGapPenalty(int pen);
/*{\Mop        sets the penalty for starting a new gap to $pen$.}*/
	void setGapIncPenalty(int incpen);
/*{\Mop        sets the penalty for extending a gap by one position to $incpen$.}*/
	void setGapPenalty(int pen, int incpen);
/*{\Mop        sets the penalty for starting a new gap to $pen$ and for extending a gap by 1 position to $incpen$.}*/

	int getGapPenalty() const;
/*{\Mop        returns the penalty for starting a new gap.}*/
	int getGapIncPenalty() const;
/*{\Mop        returns the penalty for extending a gap by one position.}*/
	int& operator()(int i, int j);

};


#if LEDA_ROOT_INCL_ID == 511995
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
