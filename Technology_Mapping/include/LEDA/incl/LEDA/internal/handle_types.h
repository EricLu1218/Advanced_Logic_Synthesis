/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  handle_types.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.8 $  $Date: 2007/02/28 07:44:15 $

#ifndef LEDA_HANDLE_H
#define LEDA_HANDLE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520352
#include <LEDA/internal/PREAMBLE.h>
#endif



#if defined(OLD_HANDLE_TYPES) || defined(__DMC__)

#include <LEDA/internal/handle_types_old.h>

#else

//------------------------------------------------------------------------------
// handle_base/rep: base classes for handle types string, point, segment,...
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

class __exportC handle_rep  {

private:

 unsigned long count;

#if defined(LEDA_MULTI_THREAD)
 leda_mutex mutex_count;
 void lock()   { mutex_count.lock(); }
 void unlock() { mutex_count.unlock(); }
#else
 void lock()   {}
 void unlock() {}
#endif

public:

 handle_rep() : count(1) {}

 unsigned long ref_num() const { return count; }

 unsigned long ref_plus() 
 { lock();
   count++;
   unlock();
   return count;
  }

 unsigned long ref_minus() 
 { lock();
   count--;
   unlock();
   return count;
  }

 LEDA_MEMORY(handle_rep)

};


class HandleBase {
protected:
void* PTR;

friend unsigned long ID_Number(const HandleBase& x) 
{ //return *((unsigned long*)&x.PTR); 
  union { void* ptr; unsigned long id; } u = {x.PTR};
  return u.id;
}
 
friend bool identical(const HandleBase& x, const HandleBase& y) 
{ return x.PTR == y.PTR; }
};



template <class rep_type> 
class handle_base : public HandleBase {

protected:

rep_type* ptr() const { return (rep_type*)PTR; }

public:

 handle_base() {}

 handle_base(const handle_base& x) 
 { PTR = x.PTR;  
   ptr()->ref_plus();
 }

~handle_base() { if (ptr()->ref_minus() == 0) delete ptr(); }

 handle_base& operator=(const handle_base& x)
 { x.ptr()->ref_plus();
   if (ptr()->ref_minus() == 0) delete ptr();
   PTR = x.PTR;
   return *this;
  }
 
 unsigned long refs() const { return ptr()->ref_num(); }
 
 LEDA_MEMORY(handle_base<T>)
};

#define HANDLE_BASE(T) handle_base<T>

LEDA_END_NAMESPACE

#endif


#if LEDA_ROOT_INCL_ID == 520352
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



#endif
 
