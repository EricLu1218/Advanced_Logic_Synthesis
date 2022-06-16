/*******************************************************************************
+
+  LEDA 5.2
+
+
+  alphabet.cpp
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef ALPHABET_H
#define ALPHABET_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 511996
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// alphabet
//------------------------------------------------------------------------------

#include<LEDA/core/string.h>
#include<LEDA/core/sortseq.h>
#include<float.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {alphabet} {} {Alphabet} {A} }*/

class __exportC alphabet {

/*{\Mdefinition
An instance $A$ of the data type |\Mname| is an object containing
a collection of letters. During a creation phase, letters
can be added to or removed from the alphabet. If the final set
of letters has been created, the alphabet has to be finished. Afterwards
no further changes are accepted. The letters are lexicographically ordered
in the alphabet.\\
Remark: This data type is experimental. It can significantly change
in future versions. }*/

private:
	string letters;
	bool finished;
	sortseq<int, char> letter_set;

public:
/*{\Mcreation}*/
	alphabet() {
		finished = false;
	}
/*{\Mcreate  creates an instance $A$ of type |\Mname|.}*/

	alphabet(const string seq) {
		for (int i = 0; i < seq.length(); i++)
			if (letter_set.lookup(int(seq[i])) == nil)
				letter_set.insert(int(seq[i]), seq[i]);
		setFinished();
	}
/*{\Mcreate  creates an instance $A$ of type |\Mname| and initializes $A$ to
be the set of all different letters in $seq$. The alphabet is set to be 
finished by this constructor.}*/

	~alphabet() {
	}

/*{\Moperations 1.2 5 }*/

	void setStandard();
/*{\Mop        sets the standard alphabet consisting of the upper case letters A-Z
and finishes the alphabet construction phase. }*/
	bool isStandard() const;
/*{\Mop        returns true if the alphabet consists of the upper case letters A-Z, false otherwise. }*/
	void setDNA();
/*{\Mop        sets the alphabet to the set consisting of the letters A,C,T, and G 
and finishes the alphabet construction phase. }*/
	bool isDNA() const;
/*{\Mop        returns true if the alphabet consists of the letters A,C,T, and G. }*/
	void addLetter(char ch);
/*{\Mop        adds letter $ch$ to $A$.\\
\precond $A$ is not yet finished. If it is finished, an exception is thrown. }*/
	void delLetter(char ch);
/*{\Mop        deletes letter $ch$ from $A$ if contained; otherwise, nothing happens.\\
\precond $A$ is not yet finished. If it is finished, an exception is thrown. }*/
	char getLetter(unsigned i);
/*{\Mop        returns the letter at position $i$ of the lexicographically ordered
set of letters, starting with position $0$. If $i$ is too big, an 
exception is thrown.\\
\precond $A$ is finished. Otherwise an exception is thrown.}*/
	void setFinished();
/*{\Mop        finishes the alphabet.}*/
	bool getFinished() const;
/*{\Mop        returns $true$ if $A$ is finished, $false$ otherwise.}*/
	bool isFinished() const;
/*{\Mop        returns $true$ if $A$ is finished, $false$ otherwise.}*/
	string getLetters();
/*{\Mop        returns a string containing all letters of $A$ in lexicographical order. \\
\precond $A$ is finished. Otherwise an exception is thrown.}*/
	int getLength() const;
/*{\Mop        returns the size of $A$, that is the number of different letters in A.\\
\precond $A$ is finished. Otherwise an exception is thrown.}*/
	int getSize() const;
/*{\Mop        same as $getLength()$.}*/


	int getIndex(char ch) const; // nur Umwandlung 
	bool isValid(); //all letters different
};

#if LEDA_ROOT_INCL_ID == 511996
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
