/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  POSTAMBLE.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:13 $


#if defined(LEDA_STD_IO_HEADERS) && !defined(LEDA_INSTALL) && defined(ostream)

#undef ostream
#undef istream
#undef cout
#undef cin
#undef cerr
#undef endl
#undef ends
#undef hex
#undef dec
#undef setw
#undef ios
#undef setprecision
#undef ofstream
#undef ifstream
#undef istringstream
#undef ostringstream
#undef ios_base
#undef streambuf
#undef streamsize
#undef streampos
#undef streamoff
#undef filebuf

#endif


#if defined(LEDA_COMPRESSION_WA1) && !defined(LEDA_INSTALL)
#undef openmode
#undef seekdir
#undef streamsize
#undef pubseekpos
#undef pubseekoff
#undef pubsync
#endif
