/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  count_streambuf.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:05 $

#ifndef _LEDA_COUNT_STREAMBUF_H
#define _LEDA_COUNT_STREAMBUF_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520339
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/coding/internal/streambuf_workaround.h>

LEDA_BEGIN_NAMESPACE

/// count_streambuf ////////////////////////////////////////////////////////////////////////

/*{\Manpage {count_streambuf} {} {Memory Streambuffer} {mb}}*/

class __exportC count_streambuf : public LEDA_STREAMBUF {
/*{\Mdefinition
An object |\Mvar| of the class |\Mname| can be used as target stream 
for a coder and allows to calculate the size of an encoding.
The object simply counts how many bytes have been written to it, but the
bytes are not stored, they are all lost.
If you want to encode into memory and you do not know in advance how large
your output buffer should be you can use a two pass approach:
In the first pass use a |\Mtype| to calculate the size of the output. 
And in the second pass the actual encoding is done with a |memory_streambuf| 
with a buffer that is large enough (see the example below).
}*/
public:
	typedef LEDA_STREAMBUF base;

public:
/*{\Mcreation}*/

	count_streambuf()
	 : OutCount( 0 )
	{ setg(0, 0, 0); setp(0, 0); }
	/*{\Mcreate creates an object |\Mvar| with out count zero.}*/

/*{\Moperations}*/

	void reset() { OutCount = 0; }
	/*{\Mop sets the out count to zero.}*/

	streamsize out_count() const { return OutCount; }
	/*{\Mop returns how many characters have been written to |\Mvar| since 
	the last reset.}*/

protected:
	virtual int overflow(int c) { ++OutCount; return c; }
		// called when put-buffer buffer full  (puts the character to the stream)

private:
	streamsize OutCount;
};

/*{\Mexample
Example on how to use |\Mtype|:
\begin{verbatim}
  typedef DeflateCoder Coder;

  memory_streambuf in(256);

  // fill the buffer of in ...

  // pass 1: calculate size for out_buffer
  count_streambuf out1;
  Coder coder1(&in, &out1);
  coder1.encode();

  // allocate out_buffer
  streamsize total = out1.out_count();
  char* out_buffer = new[ total ];

  // pass 2: encode into memory
  in.reset();

  memory_streambuf out2(out_buffer, total);
  Coder coder2(&in, &out2);
  coder2.encode();
\end{verbatim}
}*/

#undef LEDA_STREAMBUF

#if LEDA_ROOT_INCL_ID == 520339
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif
