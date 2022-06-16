/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  memory_mt.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:49:27 $

#ifndef LEDA_MEMORY_MT_H
#define LEDA_MEMORY_MT_H

LEDA_BEGIN_NAMESPACE


class __exportC memory_management {

  friend class __exportC memory_management_init;

  int num;     // number of managers      (default  32)
  int tbl_sz;  // table size of managers  (default 256) 

  memory_manager **mgrs;

#if defined(__win32__)
  void init(int n=1024, int sz=256);
#else
  void init(int n=256, int sz=256);
#endif

  void destroy();

public:

  memory_management();
  memory_management(int n, int sz);
 ~memory_management();

  int max_size()    { return tbl_sz; }
  int num_of_mgrs() { return num;    }
  
  void kill();
  void clear();
  void print_statistics();
  int  used_memory();

  memory_manager& acquire();
  
  void allocate_block(int,int);
  
  void* allocate_words(int);
  void* allocate_bytes(int);
  void* allocate_bytes_with_check(int);
  void* allocate_vector(int);
  
  void deallocate_words(void*,int);
  void deallocate_bytes(void*, int);
  void deallocate_bytes_with_check(void*,int);
  void deallocate_vector(void*);
  void deallocate_list(void*, void*, int);
};


extern memory_management __exportD mt_memory_mgr;

#ifdef std_memory
#undef std_memory
#endif

#define std_memory  mt_memory_mgr


class __exportC memory_management_init
{
  static unsigned count;

public:
  memory_management_init();
 ~memory_management_init();

};


static memory_management_init memory_initialize;


#if defined(__BORLANDC__) || defined(__DMC__) || (__GNUC__ == 2) || defined(__mipspro__)
#define MATCHING_DELETES_MT
#else
#define MATCHING_DELETES_MT \
void  operator delete(void*,void*)\
{ leda_error_handler(1,"delete(void*,void*) should never be called."); }\
\
void  operator delete(void*,void*,int)\
{ leda_error_handler(1,"delete(void*,void*,int) should never be called."); }
#endif

LEDA_END_NAMESPACE


#define LEDA_MEMORY_MT(type)\
\
void* operator new(size_t bytes)\
{ if (bytes < STDMEM_TABLESIZE)\
  { leda_memory_manager &manager = leda_mt_memory_mgr.acquire();\
    leda_memory_elem_ptr* q = manager.free_list+bytes;\
    if (*q==0) manager.allocate_block(bytes,-1);\
    leda_memory_elem_ptr p = *q;\
    *q = p->next;\
    return p; }\
  else return malloc(bytes);\
}\
\
void* operator new(size_t,void* p) { return p; }\
void* operator new(size_t,void* p,int) { return p; }\
\
void  operator delete(void* p, size_t bytes)\
{ if (p != 0)\
  { if (bytes < STDMEM_TABLESIZE)\
    { leda_memory_manager &manager = leda_mt_memory_mgr.acquire();\
      leda_memory_elem_ptr* q = manager.free_list+bytes;\
      leda_memory_elem_ptr(p)->next = *q;\
      *q = leda_memory_elem_ptr(p);\
    }\
    else free((char*)p);\
  }\
}\
\
MATCHING_DELETES_MT




#endif
