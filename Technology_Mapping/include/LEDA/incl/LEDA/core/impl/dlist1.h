/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  dlist.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2007/02/25 00:48:13 $


#ifndef LEDA_DLIST_H
#define LEDA_DLIST_H

//------------------------------------------------------------------------------
//  doubly linked lists
//------------------------------------------------------------------------------

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

class __exportC dlist; 
class __exportC dlink;

typedef dlink* list_item;

class __exportC dlink {

  dlink* succ;
  dlink* pred;

  dlink(dlink* pre, dlink* suc)
  { succ = suc;
    pred = pre;
  }

  LEDA_MEMORY(dlink)


  friend class __exportC dlist;
  friend dlink* succ_item(dlink* p) { return p->succ; }
  friend dlink* pred_item(dlink* p) { return p->pred; }
};




//------------------------------------------------------------------------------
// dlist: base class for all doubly linked lists
//------------------------------------------------------------------------------

class __exportC  dlist {

protected:

  dlink* h;                     //head
  dlink* t;                     //tail
  int    count;                 //length of list
  bool   def_cmp;               //use default compare for builtin types

virtual int  cmp(GenPtr, GenPtr) const { return 0; }
virtual int  ord(GenPtr) const { return 0; }
virtual void app(GenPtr&) const {}
virtual void read_el(GenPtr&,istream&) const {}
virtual void print_el(GenPtr&,ostream&) const {}
virtual void clear_el(GenPtr&) const {}
virtual void copy_el(GenPtr&)  const {}

virtual int  el_type_id() const { return UNKNOWN_TYPE_ID; }

void gen_quick_sort(GenPtr*,dlink**,int,int);
void gen_insertion_sort(dlink**,dlink**,dlink**);

void recompute_length() const;

public:

// access operations

   int  length() const { if (count < 0) recompute_length(); return count; }
   int  size()   const { return length(); }
   bool empty()  const { return h==nil; }

   // iteration
   dlink* first_item()          const { return h; }
   dlink* last_item()           const { return t; }
   dlink* next_item(dlink* p)   const { return p ? p->succ : 0; }
   dlink* pred_item(dlink* p)   const { return p ? p->pred : 0; }

   dlink* stl_next_item(dlink* p)   const { return p ? p->succ : h; }
   dlink* stl_pred_item(dlink* p)   const { return p ? p->pred : t; }

   dlink* first()               const { return h; }
   dlink* last()                const { return t; }
   dlink* succ(dlink* p)        const { return p->succ; }
   dlink* pred(dlink* p)        const { return p->pred; }
   dlink* cyclic_succ(dlink* p) const { return p->succ? p->succ : h; }
   dlink* cyclic_pred(dlink* p) const { return p->pred? p->pred : t; }
   
   dlink* succ(dlink* l, int i) const; 
   dlink* pred(dlink* l, int i) const;
   dlink* get_item(int = 0)     const; 

   dlink* max() const;
   dlink* min() const;
   dlink* search(GenPtr) const;

   void   remove(GenPtr);

   int    rank(GenPtr) const;

   GenPtr& entry(dlink* l) const { return l->e; }

   GenPtr& head() const 
   { if (h == nil) error_handler(1,"dlist::head(): empty list"); return h->e; }

   GenPtr& tail() const
   { if (h == nil) error_handler(1,"dlist::tail(): empty list"); return t->e; }


// update operations

protected:

   dlink* insert(GenPtr a, dlink* l, int dir=0);

   dlink* push(GenPtr a)   
   { if (count >= 0) count++;
     if (h) h = h->pred = new dlink(a,0,h); 
     else   h = t =  new dlink(a,0,0);
     return h;
    }
   
   dlink* append(GenPtr a)
   { if (count >= 0) count++;
     if (t) t = t->succ = new dlink(a,t,0);
     else   t = h = new dlink(a,0,0);
     return t; 
    } 
   
   void   assign(dlink* l, GenPtr a) { clear_el(l->e); l->e = a; }

   void   apply();
   void   merge(dlist&);
   void   unique();
   void   quick_sort(int);
   void   merge_sort();

public:

   GenPtr del(dlink*);
   void   del(dlink*,dlink*,dlink*); // for unique


   GenPtr pop()    
   { if (h==nil) error_handler(1,"dlist::pop on empty list.");
     dlink* q=h; 
     h = h->succ;
     if (h) h->pred = 0;
     else t = 0;
     GenPtr x = q->e;
     delete q;
     count--;
     return x;
   }
   
   
   GenPtr pop_back()    
   { if (t==nil) error_handler(1,"dlist::pop_back on empty list.");
     dlink* q = t; 
     t = t->pred;
     if (t) t->succ = 0;
     else h = 0;
     GenPtr x = q->e;
     delete q;
     count--;
     return x;
   }


   void  move_to_front(dlink*);
   void  move_to_back(dlink*);

   void  swap(dlist&);
   void  splice(dlink*s, dlist&, dlink*, dlink*);
   void  splice(dlink*s, dlist&, dlink*);
   void  splice(dlink*s, dlist&);
   void  conc(dlist&, int=leda::behind);
   void  split(list_item, dlist&, dlist&, int=leda::before);
   void  extract(list_item it1, list_item it2, dlist& result, bool inclusive = true);
   void  bucket_sort(int,int);
   void  bucket_sort();
   void  reverse_items();
   void  reverse();
   void  reverse_items(list_item,list_item);
   void  reverse(list_item,list_item);
   void  permute(list_item*);
   void  permute();
   void  clear();


// operators

   dlist& operator=(const dlist&); 
   dlist  operator+(const dlist&); 


// I/O

   void print(ostream&,string, char)       const;    
   void print(ostream& out,char space=' ') const;
   void print(string s, char space=' ')    const { print(cout,s,space);  }
   void print(char space=' ')              const { print(cout,space); }   


   void read(istream&,string, int);  

   void read(istream& in,int delim) { read(in,"",delim); }
   void read(istream& in)           { read(in,"",EOF); }

   void read(string s, int delim)   { read(cin,s,delim); }   
   void read(string s)              { read(cin,s,'\n'); }   

   void read(char delim)  { read(cin,"",delim);}  
   void read()            { read(cin,"",'\n');}  


// constructors & destructors

   dlist();    
   dlist(GenPtr a);
   dlist(const dlist&);

#if (__GNUC__ != 2) || (__GNUC_MINOR__ != 8)
   virtual ~dlist()  { clear(); }
#endif


// misc

   int space()  const { return sizeof(dlist) + length() * sizeof(dlink); }

};


LEDA_END_NAMESPACE

#endif
