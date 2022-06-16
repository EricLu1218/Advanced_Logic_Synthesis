/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  param_types.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.20 $  $Date: 2007/02/28 07:44:15 $

#ifndef LEDA_PARAM_TYPE_H
#define LEDA_PARAM_TYPE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520363
#include <LEDA/internal/PREAMBLE.h>
#endif



LEDA_BEGIN_NAMESPACE


//----------------------------------------------------------------------------
//
// Parameterized data types store copies of values of arbitrary types T
// in a single data field of type void* (GenPtr). There are two cases
// which have to be treated differently here.
//
// If T is a "big type" (sizeof(T) > sizeof(GenPtr)) we make a copy in the 
// dynamic memory using the new operator and store a pointer to this copy. 
// If T is a "small type" (sizeof(T) <= sizeof(GenPtr)) we avoid the 
// overhead of an extra level of indirection by copying the value directly 
// into the void* data field. This is done by using the "in-place" variant 
// of the new operator. 
//
// Both cases are handled by the "leda_copy" function template defined in this 
// file. Similarly, the two cases (small or big type) have to be treated 
// differently,  when creating ("leda_create"), destroying ("leda_clear"),
// and accessing ("leda_access") data of a parameterized type. Note that 
// destruction of small types uses an an explicit destructor call.
//
// More details on the implementation of parameterized data types in LEDA
// will be given in the text book "LEDA: A Platform ..."  by K. Mehlhorn
// and S. N"aher.
//
//----------------------------------------------------------------------------



// type id's

enum { UNKNOWN_TYPE_ID,
       CHAR_TYPE_ID, 
       SHORT_TYPE_ID, 
       INT_TYPE_ID, 
       LONG_TYPE_ID, 
       FLOAT_TYPE_ID, 
       DOUBLE_TYPE_ID, 
       PTR_TYPE_ID,
       INTEGER_TYPE_ID,
       RATIONAL_TYPE_ID,
       REAL_TYPE_ID };



// default input operator for pointers

inline istream& operator>>(istream& in, GenPtr)
{ error_handler(1,"stream input operator (>>) not defined"); return in; }


// and bool

#if !defined(LEDA_STD_IO_HEADERS) && (defined(__aCC__) || defined(_MSC_VER))
inline istream& operator>>(istream& in, bool& b)
{ int x; in >> x; b = (x != 0); return in; }
#endif

LEDA_END_NAMESPACE


// Most c++ implementations provide an in-place new operator in <new.h>.
// However, it is not always declared inline. So, for efficiency reasons,
// we define our own (inline) version. We use a third dummy argument to 
// avoid conflicts with user defined variants.

inline void* operator new(size_t, void* where, int) { return where; }


LEDA_BEGIN_NAMESPACE

#define COPY_INTO_WORD(T,x,p) new((void*)(&p),0) T(x)
#define CONSTRUCT_WORD(T,p)   new((void*)(&p),0) T

//#if defined(__BORLANDC__) || defined(__SUNPRO_CC) || defined(__DMC__)

#if defined(__BORLANDC__) || defined(__DMC__)
#define DESTROY_WORD(T,p)     p->~T();
#else
#define DESTROY_WORD(T,p)     p->T::~T();
#endif


//----------------------------------------------------------------------------
// Function templates for leda_copy, leda_create, leda_clear, leda_cast, 
// leda_access, etc.  Most of these templates distinguish the two cases that 
// the actual type has size greater than the size of a pointer or not, i.e.,
// is a big or small type. Note that this does not cause any overhead at run 
// time since the right case can be determined at compile time. 
//----------------------------------------------------------------------------

#ifdef __volatile
#undef __volatile
#endif

#if defined(__DECCXX) || defined(__xlC__)
#define __volatile volatile
#else
#define __volatile
#endif

#include <assert.h>

template <class T>
inline GenPtr leda_cast(const T& x)
{ __volatile GenPtr p=0;
  if (sizeof(T) == sizeof(GenPtr)) p = *(GenPtr*)&x;
  if (sizeof(T) <  sizeof(GenPtr)) p = leda_copy(x);
  if (sizeof(T) >  sizeof(GenPtr)) p = (GenPtr)&x;

  return p;
 }


template <class T>
inline void leda_clear(T& x) 
{ T* p = &x;
  if (sizeof(T) <= sizeof(GenPtr)) DESTROY_WORD(T,p);
  if (sizeof(T) >  sizeof(GenPtr)) delete p;
}



#if (__GNUC__ >= 4 || (__GNUC__ >= 3 && __GNUC_MINOR__ >= 3))

template<class T>
inline GenPtr leda_copy(const T& x)
{ GenPtr p=0;
  if (sizeof(T) <= sizeof(GenPtr)) 
  { union { GenPtr* ptr; T* q; } u;
    u.q = COPY_INTO_WORD(T,x,p);
    p = *(u.ptr);
   }
  else p = new T(x);
  return p;
 }


template <class T>
inline GenPtr leda_create(const T*) 
{ GenPtr p=0;
  if (sizeof(T) <= sizeof(GenPtr)) 
  { union { GenPtr* ptr; T* q; } u;
    u.q = CONSTRUCT_WORD(T,p);
    p = *(u.ptr);
   }
  else p = new T;
  return p;
 }

template <class T>
inline T& leda_access(const T*, GenPtr& p)
{ if (sizeof(T) <= sizeof(GenPtr))
   { union { GenPtr* ptr; T* q; } u = {&p};
     return *(u.q);
    }
  else
     return *(T*)p;
 }

template <class T>
inline const T& leda_const_access(const T*, const GenPtr& p)
{ if (sizeof(T) <= sizeof(GenPtr))
   { union { const GenPtr* ptr; const T* q; } u = {&p};
     return *(u.q);
    }
  else
     return *(const T*)p;
 }


#else


template<class T>
inline GenPtr leda_copy(const T& x)
{ __volatile GenPtr p=0;
  if (sizeof(T) <= sizeof(GenPtr)) COPY_INTO_WORD(T,x,p);
  if (sizeof(T) >  sizeof(GenPtr)) p = (GenPtr) new T(x);
  return p;
 }

template <class T>
inline GenPtr leda_create(const T*) 
{ __volatile GenPtr p=0;
  if (sizeof(T) <= sizeof(GenPtr)) CONSTRUCT_WORD(T,p);
  if (sizeof(T) >  sizeof(GenPtr)) p = (GenPtr) new T;
  return p;
 }

template <class T>
inline T& leda_access(const T*, GenPtr& p)
{ if (sizeof(T) <= sizeof(GenPtr))
     return *(T*)(&p);
  else
     return *(T*)p;
 }

template <class T>
inline const T& leda_const_access(const T*, const GenPtr& p)
{ if (sizeof(T) <= sizeof(GenPtr))
     return *(const T*)(&p);
  else
     return *(const T*)p;
 }

#endif


template <class T> 
inline int   leda_type_id(const T*)   { return UNKNOWN_TYPE_ID; }

template <class T>
inline const char* leda_tname(const T*) { return "unknown"; }




/*

template<>
inline GenPtr leda_copy(const int& x)
{ long y = x;
  return leda_copy(y);
}

template<>
inline GenPtr leda_cast(const int& x) { return leda_copy(x); }

template <>
inline GenPtr leda_create(const int*) { return 0; }

template <>
inline int& leda_access(const int*, GenPtr& p) 
{ return leda_access((const long*)0,p); }

template <>
inline const int& leda_const_access(const int*, const GenPtr& p)
{ return leda_const_access((const long*)0,p); }
*/




//----------------------------------------------------------------------------
// A specialization for "double" using the LEDA memory management 
// if sizeof(double) > sizeof(GenPtr). 
//----------------------------------------------------------------------------

#if !defined(WORD_LENGTH_64)

inline GenPtr leda_copy(const double& x)
{ GenPtr p = std_memory.allocate_bytes(sizeof(double));
  *(double*)p = x; 
  return p;
 }

inline GenPtr leda_create(const double*)
{ GenPtr p = std_memory.allocate_bytes(sizeof(double));
  *(double*)p = 0; 
  return p;
 }

inline void leda_clear(double& x) 
{ std_memory.deallocate_bytes(&x,sizeof(double)); }

inline GenPtr leda_cast(const double& x) 
{ return GenPtr(&x); }

inline double& leda_access(const double*, GenPtr& p) 
{ return *(double*)p; }

inline const double& leda_const_access(const double*, const GenPtr& p) 
{ return *(const double*)p; }

#endif


//----------------------------------------------------------------------------
// type id's
//----------------------------------------------------------------------------

inline int leda_type_id(const char*)   { return CHAR_TYPE_ID; }
inline int leda_type_id(const short*)  { return SHORT_TYPE_ID; }
inline int leda_type_id(const int*)    { return INT_TYPE_ID; }
inline int leda_type_id(const long*)
{ return (sizeof(long) == sizeof(int)) ? INT_TYPE_ID : LONG_TYPE_ID; }
inline int leda_type_id(const float*)  { return FLOAT_TYPE_ID; }
inline int leda_type_id(const double*) { return DOUBLE_TYPE_ID; }


//----------------------------------------------------------------------------
// type names
//----------------------------------------------------------------------------

inline const char* leda_tname(const char*  ) { return "char";   }
inline const char* leda_tname(const short* ) { return "short";  }
inline const char* leda_tname(const int*   ) { return "int";    }
inline const char* leda_tname(const long*  ) { return "long";   }
inline const char* leda_tname(const float* ) { return "float";  }
inline const char* leda_tname(const double*) { return "double"; }
inline const char* leda_tname(const bool*)   { return "bool"; }



//----------------------------------------------------------------------------
// id numbers
//----------------------------------------------------------------------------

inline unsigned long ID_Number(const void* x)// { return *(unsigned long*)&x; }
{ union { const void* ptr; unsigned long y; } u = {x};
  return u.y;
}

inline unsigned long ID_Number(char   x) { return (unsigned long)x; }
inline unsigned long ID_Number(short  x) { return (unsigned long)x; }
inline unsigned long ID_Number(int    x) { return (unsigned long)x; }
inline unsigned long ID_Number(long   x) { return (unsigned long)x; }
inline unsigned long ID_Number(unsigned int  x) { return (unsigned long)x; }
inline unsigned long ID_Number(unsigned long x) { return (unsigned long)x; }




//----------------------------------------------------------------------------
// pre-defined linear orders  (compare)
//----------------------------------------------------------------------------

#define DEFAULT_COMPARE  compare

#if defined(__aCC__)
#define LEDA_COMPARE_TEMPLATE
#endif


#if defined(LEDA_COMPARE_TEMPLATE)

#define COMPARE_DECL_PREFIX template<>

template <class T> 
inline int DEFAULT_COMPARE(const T& x, const T& y) 
{ if (x < y) return -1;
  else if (x == y) return 0;
  else return 1;
}

#else

#define COMPARE_DECL_PREFIX

#define DEFINE_DEFAULT_COMPARE(T)                    \
inline int DEFAULT_COMPARE(const T& x, const T& y)   \
{ if (x < y) return -1;                              \
  else if (x > y) return 1;                          \
  else return 0;                                     \
}\
inline bool leda_smaller(const T& x, const T& y) { return x < y; }

DEFINE_DEFAULT_COMPARE(char)
DEFINE_DEFAULT_COMPARE(unsigned char)
DEFINE_DEFAULT_COMPARE(int)
DEFINE_DEFAULT_COMPARE(unsigned int)
DEFINE_DEFAULT_COMPARE(long)
DEFINE_DEFAULT_COMPARE(unsigned long)
DEFINE_DEFAULT_COMPARE(float)
DEFINE_DEFAULT_COMPARE(double)

#endif

/*

// if you want a predefined compare for all pointer types, you can use the
// following code:

COMPARE_DECL_PREFIX
inline int DEFAULT_COMPARE(const GenPtr& x, const GenPtr& y)
{
	ptrdiff_t delta = ((const char*) x) - ((const char*) y);
	if (delta < 0) return -1;
	else if (delta > 0) return +1;
	else return 0;
}

*/

template<class T>
inline bool leda_smaller(const T& x, const T& y) 
{ return compare(x,y) < 0; }




template <class T> class leda_cmp_base {

int (*cmp_func)(const T&,const T&);

public:

leda_cmp_base(int (*f)(const T&,const T&)=0) { cmp_func = f; }

virtual int operator()(const T& x, const T& y) const
{ if (cmp_func == 0) error_handler(1,"leda_cmp_base: compare undefined");
  return cmp_func(x,y); 
 }

virtual ~leda_cmp_base() {}

LEDA_MEMORY(leda_cmp_base<T>)

};



//----------------------------------------------------------------------------
// pre-defined hash functions (Hash)
//----------------------------------------------------------------------------

inline int Hash(void* x) 
{ //return int(*(long*)&x); 
  union { const void* ptr; int y; } u = {x};
  return u.y;
}

inline int Hash(const signed char&    x) { return int(x); } 
inline int Hash(const signed short&   x) { return int(x); }
inline int Hash(const signed int&     x) { return int(x); }
inline int Hash(const signed long&    x) { return int(x); }
inline int Hash(const unsigned char&  x) { return int(x); } 
inline int Hash(const unsigned short& x) { return int(x); }
inline int Hash(const unsigned int&   x) { return int(x); }
inline int Hash(const unsigned long&  x) { return int(x); }
inline int Hash(const float&          x) { return int(x); }
inline int Hash(const double&         x) { return int(x); }


//----------------------------------------------------------------------------
// default values 
//----------------------------------------------------------------------------

/*
inline void leda_init_default(char&   x) { x=0; }
inline void leda_init_default(short&  x) { x=0; }
inline void leda_init_default(int&    x) { x=0; }
inline void leda_init_default(long&   x) { x=0; }
inline void leda_init_default(float&  x) { x=0; }
inline void leda_init_default(double& x) { x=0; }
template <class T>
inline void leda_init_default(T&) { }
*/

template <class T>
inline void leda_init_default(T& x) { x = T(); }



//----------------------------------------------------------------------------
// some useful macros 
// (used in data type templates)
//----------------------------------------------------------------------------

#define LEDA_ACCESS(T,p)       leda_access((T*)0, p)
#define LEDA_CONST_ACCESS(T,p) leda_const_access((T*)0, p)
#define LEDA_TYPE_ID(T)        leda_type_id((T*)0)
#define LEDA_TYPE_NAME(T)      leda_tname((T*)0)
#define LEDA_CREATE(T,x)       x=leda_create((T*)0)
#define LEDA_COPY(T,x)         x=leda_copy(LEDA_CONST_ACCESS(T,x))
#define LEDA_CLEAR(T,x)        leda_clear(LEDA_ACCESS(T,x))
#define LEDA_CAST(x)           leda_cast(x)

#define LEDA_CALL1(f,T,x)     (f)(LEDA_CONST_ACCESS(T,x))
#define LEDA_CALL2(f,T,x,y)   (f)(LEDA_CONST_ACCESS(T,x),LEDA_CONST_ACCESS(T,y))

#define LEDA_COMPARE(T,x,y)    LEDA_CALL2(compare,T,x,y)
#define LEDA_HASH(T,x)         LEDA_CALL1(Hash,T,x)

#define LEDA_EQUAL(T,x,y)      (LEDA_CONST_ACCESS(T,x)==LEDA_CONST_ACCESS(T,y))

#define LEDA_PRINT(T,x,out)    out << LEDA_CONST_ACCESS(T,x)
#define LEDA_READ(T,x,in)      LEDA_CREATE(T,x); in >> LEDA_ACCESS(T,x)



#if LEDA_ROOT_INCL_ID == 520363
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
