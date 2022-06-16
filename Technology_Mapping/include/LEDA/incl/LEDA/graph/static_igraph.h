#ifndef STATIC_SGRAPH_H
#define STATIC_SGRAPH_H

#include <LEDA/graph/static_graph.h>

LEDA_BEGIN_NAMESPACE

#undef  forall_out_edges
#define forall_out_edges(e,v)\
LEDA_FORALL_PREAMBLE \
for(void* loop_stop = (e = v->first_out_edge(), v->stop_out_edge());\
    e != loop_stop && e->NextInEdge != e; e = e->next_out_edge())

/*
#define forall_hidden_edges(e,v)\
LEDA_FORALL_PREAMBLE \
for(void* loop_stop = (e = G.stop_out_edge(v), v->stop_out_edge());\
    e != loop_stop; e = e->next_out_edge())
*/


template <class cat, class nd = data_slots<0>, class ed = data_slots<0> >
class static_sgraph : public static_graph<cat,nd,ed>
{
  public:

  typedef static_graph<cat,nd,ed> st_graph;

  typedef typename st_graph::node node;
  typedef typename st_graph::edge edge;

  typedef graph_map<static_sgraph>* map_ptr;
  typedef graph_map<st_graph>* st_map_ptr;

  static_sgraph() {}  


  list_item register_map(map_ptr M, int& slot_index, bool use_slot=false)
  { return st_graph::register_map(st_map_ptr(M),slot_index,use_slot); 
    }

  void unregister_map(map_ptr M) 
  { st_graph::unregister_map(st_map_ptr(M)); }

  edge first_in_edge(node v) const { return (edge)v->FirstInEdge; }
  edge next_in_edge(edge e) const { return (edge)e->NextInEdge; }
  edge stop_out_edge(node v) const { return v->stop_out_edge(); }


  int outdeg(node v) const { return stop_out[v] - v->first_out_edge(); }

  int  index1(node v) const { return (v) ? index(v)+1 : 0; }
  int  index1(edge e) const { return (e) ? index(e)+1 : 0; }

  node get_node1(int i) const { return (i) ? first_node() + (i-1) : 0; }
  edge get_edge1(int i) const { return (i) ? first_edge() + (i-1) : 0; }


  void finish_construction()
  { st_graph::finish_construction();
    node v;
    forall_nodes(v,(*this)) stop_out[v] = v->stop_out_edge();
   }


  void swap_edges(edge x, edge y) const {
    if (x != y)
    { 
#if defined(SWAP_ARRAYS)
const list<st_map_ptr>& L = st_graph::get_map_list(1);
st_map_ptr p;
forall(p,L)
  leda_swap(p->array_access(x),p->array_access(y));
#endif
      typename st_graph::edge_type tmp = *x;
      *x = *y;
      *y = tmp;
     }
  }


  void push_in_edge(node v, edge e) const
  { e->NextInEdge = v->FirstInEdge;
    v->FirstInEdge = e;
   }

  void restore_out_edges(node v) const {
   stop_out[v] = v->stop_out_edge();
  }

  void restore_all_out_edges() const {
   node v;
   forall_nodes(v,(*this)) stop_out[v] = v->stop_out_edge();
  }


  void restore_all_edges() const {
   node v;
   forall_nodes(v,(*this)) 
   { edge s = v->stop_out_edge();
     for(edge e = stop_out[v]; e!=s; e++)
       push_in_edge(opposite(e,v),e);
     stop_out[v] = s;
    }
  }

void rebuild_in_edges() const
{ node v;
  forall_nodes(v,(*this)) v->FirstInEdge = 0;
  forall_nodes(v,(*this))
  { edge e = stop_out[v];
    while (--e >= v->first_out_edge())
    { node u = opposite(e,v);
      push_in_edge(u,e);
     }
   }
 }


#ifdef SORT_SMALLER

template <class Smaller>
void qsort(edge l, edge r, const Smaller& smaller) const
{ if (r <= l) return;
  if (smaller(r,l)) swap_edges(l,r);
  if (r == l+1) return;
  edge k = l + (r-l)/2;
  if (smaller(k,l)) 
     swap_edges(l,k);
  else
     if (smaller(r,k)) swap_edges(k,r);
  if (r == l+2) return;
  edge i = l+1;
  edge j = r;
  swap_edges(i,k); 
  k = i;
  for(;;)
  { while (smaller(++i,k));
    while (smaller(k,--j));
    if (i<j) swap_edges(i,j);
    else break;
   }
  swap_edges(k,j);
  if (j > l+1) qsort(l,j-1,smaller);
  if (r > j+1) qsort(j+1,r,smaller);
}


template <class Smaller>
void qsort_p(edge* l, edge* r, const Smaller& smaller) const
{ if (r <= l) return;
  if (smaller(*r,*l)) swap(*l,*r);
  if (r == l+1) return;
  edge* k = l + (r-l)/2;
  if (smaller(*k,*l))
     swap(*l,*k);
  else
     if (smaller(*r,*k)) swap(*k,*r);
  if (r == l+2) return;
  edge* i = l+1;
  edge* j = r;
  swap(*i,*k); 
  k = i;
  for(;;)
  { while (smaller(*++i,*k));
    while (smaller(*k,*--j));
    if (i<j) swap(*i,*j);
    else break;
   }
  swap(*k,*j);
  if (j > l+1) qsort_p(l,j-1,smaller);
  if (r > j+1) qsort_p(j+1,r,smaller);
}




template<class Smaller>
void sort_in_edges(const Smaller& smaller) const
{ 
  static_sgraph& G = (static_sgraph&)*this;

  int m = G.number_of_edges();

  edge* E = new edge[m];
  edge* p = E;

  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v) 
    { e->NextInEdge = G.opposite(e,v);
      *p++ = e;
     }
    v->FirstInEdge = 0;
  }

  G.qsort_p(E,p-1,smaller);

  while (p-- > E)
  { edge e = *p;
    node u = (node)e->NextInEdge;
    G.push_in_edge(u,e);
   }

  delete[] E;
}
   


template<class Smaller>
void sort_edges(const Smaller& smaller) const
{ 
  static_sgraph& G = (static_sgraph&)*this;

  node v;
  forall_nodes(v,(*this))
  { edge first = v->first_out_edge();
    edge last  = stop_out[v]-1;
    G.qsort(first,last,smaller);
  }

  sort_in_edges(smaller);
}

#else

template <class cap_array>
void cap_qsort(edge l, edge r, const cap_array& cap) const
{ if (r <= l) return;
  if (cap[r] > cap[l]) swap_edges(l,r);
  if (r == l+1) return;
  edge k = l + (r-l)/2;
  if (cap[k] > cap[l]) 
     swap_edges(l,k);
  else
     if (cap[r] > cap[k]) swap_edges(k,r);
  if (r == l+2) return;
  edge i = l+1;
  edge j = r;
  swap_edges(i,k); 
  k = i;
  int s  = cap[i];
  for(;;)
  { while (cap[++i] > s);
    while (s > cap[--j]);
    if (i<j) swap_edges(i,j);
    else break;
   }
  swap_edges(k,j);
  if (j > l+1) cap_qsort(l,j-1,cap);
  if (r > j+1) cap_qsort(j+1,r,cap);
}


template <class cap_array>
void cap_qsort_p(edge* l, edge* r, const cap_array& cap) const
{ if (r <= l) return;
  if (cap[*r] > cap[*l]) swap(*l,*r);
  if (r == l+1) return;
  edge* k = l + (r-l)/2;
  if (cap[*k] > cap[*l]) 
     swap(*l,*k);
  else
     if (cap[*r] > cap[*k]) swap(*k,*r);
  if (r == l+2) return;
  edge* i = l+1;
  edge* j = r;
  swap(*i,*k); 
  k = i;
  int s  = cap[*i];
  for(;;)
  { while (cap[*++i] > s);
    while (s > cap[*--j]);
    if (i<j) swap(*i,*j);
    else break;
   }
  swap(*k,*j);
  if (j > l+1) cap_qsort_p(l,j-1,cap);
  if (r > j+1) cap_qsort_p(j+1,r,cap);
}


template<class cap_array>
void sort_in_edges(const cap_array& cap) const
{ 
  static_sgraph& G = (static_sgraph&)*this;

  int m = G.number_of_edges();

  edge* E = new edge[m];
  edge* p = E;

  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v) 
    { e->NextInEdge = G.opposite(e,v);
      *p++ = e;
     }
    v->FirstInEdge = 0;
  }

  G.cap_qsort_p(E,p-1,cap);

  while (p-- > E)
  { edge e = *p;
    node u = (node)e->NextInEdge;
    G.push_in_edge(u,e);
   }

  delete[] E;
}
   


template<class cap_array>
void sort_edges(const cap_array& cap) const
{ 
  static_sgraph& G = (static_sgraph&)*this;

  node v;
  forall_nodes(v,(*this))
  { edge first = v->first_out_edge();
    edge last  = stop_out[v]-1;
    G.cap_qsort(first,last,cap);
  }

  sort_in_edges(cap);
}

#endif


template<class edge_filter>
int filter_out_edges(edge_filter& F) const
{ int count = 0;
  node v;
  forall_nodes(v,(*this)) 
  { edge stop = stop_out[v];
    edge x = v->first_out_edge();
    for(edge e = x; e < stop; e++)
       if (!F(e,v)) swap_edges(e,x++);
    count += (stop-x);
    (edge&)stop_out[v] = x;
   }
  return count;
}


template<class edge_filter>
int hide_edges(edge_filter& F) const
{ int count = filter_out_edges(F); 
  if (count > 0) rebuild_in_edges();
  return count;
}

   

template<class edge_filter, class Smaller>
int hide_edges(edge_filter& F, const Smaller& smaller) const
{ int count = filter_out_edges(F); 
  if (count > 0) sort_in_edges(smaller);
  return count;
}

void restore_edge(edge e, node v) const
{ node u = opposite(e,v);
  edge x = stop_out[v];
  swap_edges(e,x);
  stop_out[v] = x+1;
  push_in_edge(u,e);
}

};


template <class cat, class nd = data_slots<0>, class ed = data_slots<0> >
class static_igraph : public static_sgraph<cat,nd,ed>
{

public:

  typedef static_sgraph<cat,nd,ed> st_graph;

  typedef typename st_graph::node node;
  typedef typename st_graph::edge edge;

  edge_slot<int,st_graph,0> edge_id;


  void finish_construction()
  { st_graph::finish_construction();
    int i = 0;
    edge e;
    forall_edges(e,(*this)) edge_id[e] = i++;
   }

  int index(edge e) const { return edge_id[e]; }
};


   
LEDA_END_NAMESPACE

#endif
