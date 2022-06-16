/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  memory_sys.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/25 00:49:27 $

#ifndef LEDA_MEMORY_SYS_H
#define LEDA_MEMORY_SYS_H

LEDA_BEGIN_NAMESPACE

class __exportC sys_memory_manager {

public:

 sys_memory_manager() {}
 sys_memory_manager(int) {}
 sys_memory_manager(int, const char*) {}
~sys_memory_manager() {}


 void* allocate_bytes(size_t sz)  { return malloc(2*sz); }
 void* allocate_vector(size_t sz) { return malloc(2*sz); }

 void deallocate_bytes(void* p, size_t) { free(p); }
 void deallocate_vector(void* p)        { free(p); }

 void* allocate_words(size_t sz)  
 { return allocate_bytes(sz*sizeof(void*)); }

 void  deallocate_words(void* p, size_t sz) 
 { deallocate_bytes(p,sz*sizeof(void*)); }

 void  deallocate_list(void* first, void* last, size_t sz);

 
 void clear()              {}
 void kill()               {}
 void print_statistics()   {}
 void print_used_memory()  {}
 void check_free_list(int) {}

 int  used_memory() { return 0; }
};



extern __exportD sys_memory_manager sys_memory_mgr;

#ifdef std_memory
#undef std_memory
#endif


#if defined(__BORLANDC__) || defined(__DMC__) || (__GNUC__ == 2) || defined(__mipspro__)
#define MATCHING_DELETES_SYS
#else
#define MATCHING_DELETES_SYS \
void  operator delete(void*,void*)\
{ leda_error_handler(1,"delete(void*,void*) should never be called."); }\
\
void  operator delete(void*,void*,int)\
{ leda_error_handler(1,"delete(void*,void*,int) should never be called."); }
#endif

LEDA_END_NAMESPACE

#define std_memory sys_memory_mgr


#define LEDA_MEMORY_SYS(type)\
\
void* operator new(size_t bytes)\
{ return leda_sys_memory_mgr.allocate_bytes(bytes); }\
\
void* operator new(size_t, void* p)      { return p; }\
void* operator new(size_t, void* p, int) { return p; }\
\
void  operator delete(void* p, size_t bytes)\
{ if (p != 0) leda_sys_memory_mgr.deallocate_bytes(p,bytes); }\
\
MATCHING_DELETES_SYS



#endif

