/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  max_flow.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:14 $

#ifndef _LEDA_MAX_FLOW_T
#define _LEDA_MAX_FLOW_T

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520236
#include <LEDA/internal/PREAMBLE.h>
#endif

//-----------------------------------------------------------------------------
//
// MAX_FLOW
//
// preflow-push + highest level + gap heuristic 
//
//-----------------------------------------------------------------------------


/*
#include <LEDA/graph/graph.h>
*/

#include <LEDA/graph/node_level_queue.h>
#include <LEDA/graph/member_array.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

/*
#define out_cap(e)       cap[e]
#define in_cap(e)        flow[e]
#define total_cap(e)     (cap[e]+flow[e])
#define change_flow(e,x) { flow[e] += x; cap[e] -= x; }
*/

#define out_cap(e)       (cap[e]-flow[e])
#define in_cap(e)        flow[e]
#define total_cap(e)     cap[e]
#define change_flow(e,x) { flow[e] += (x); }


#define residual_cap(e,out) (out ? out_cap(e) : in_cap(e))
#define push_flow(e,x,out)  change_flow(e,(out ? x : -x))



const int mf_node_slots = 3;
const int mf_edge_slots = 0;

template<class NT, class graph>
struct mf_node_data
{
  typedef typename graph::node node;
  typedef typename graph::edge edge;

  node  succ;
  NT    excess;
  int   dist;
};

template<class NT, class graph>
struct mf_edge_data
{ 
  typedef typename graph::node node;
  typedef typename graph::edge edge;
};


template<class NT=int, class graph=int, class nd_array=int, class ed_array=int>
class max_flow;

template<>
class max_flow<int,int,int,int> 
{ public:
  enum { node_slots = mf_node_slots };
  enum { edge_slots = mf_edge_slots };
  //struct node_data { GenPtr A[node_slots]; };
  //struct edge_data { GenPtr A[edge_slots]; };
};


template<class NT, class graph>
class max_flow<NT,graph,int,int>
{ public:
  typedef mf_node_data<NT,graph> node_data;
  typedef mf_edge_data<NT,graph> edge_data;
};



template <bool b> class Temp_Assert;
template<> class Temp_Assert<true> {};
template <class T, int slots>
class check_slots {
Temp_Assert<sizeof(T) <= slots*sizeof(void*)> MAXFLOW_ERROR_SLOT_SIZE_MISMATCH;
};



template<class NT, class graph, class nd_array, class ed_array>
class max_flow  
{

public:

  typedef typename graph::node node;
  typedef typename graph::edge edge;

  typedef mf_node_data<NT,graph> node_data;
  typedef mf_edge_data<NT,graph> edge_data;

  enum { node_slots = mf_node_slots };
  enum { edge_slots = mf_edge_slots };


private:

  check_slots<node_data,node_slots> check_dummy1;
  //check_slots<edge_data,edge_slots> check_dummy2;

  typedef member_array<node, nd_array, node_data> succ_array;
  typedef member_array<NT,   nd_array, node_data> excess_array;
  typedef member_array<int,  nd_array, node_data> dist_array;

  typedef node_level_queue<graph,succ_array> level_queue;


  float h;

  bool run_phase0;

  NT fval;
  NT cut_bound;

  bool dynamic_in_edges;

  // statistics (phase 1, 2, and sum)

  int num_pushes[3];
  int num_f_pushes[3];
  int num_relabels[3];
  int num_updates[3];
  int num_gaps[3]; 
  int num_gap_nodes[3]; 
  int num_inspections[3];

  float cputime[3];
  float update_time[3];
  float gap_time;


template<class T>
T count(T* counter, int i = -1) 
{ if (i < 0) 
    return counter[0] + counter[1] + counter[2];
  else 
    return counter[i];
}

public:

max_flow() : h(5.0), run_phase0(true), fval(0), cut_bound(0)
{ assert(sizeof(node_data) <= 4*mf_node_slots);
  //assert(sizeof(edge_data) <= 4*mf_edge_slots);
  dynamic_in_edges = false;
  reset_counters(); 
}

int pushes(int i=-1)      { return count(num_pushes,i); }
int f_pushes(int i=-1)    { return count(num_f_pushes,i); }
int relabels(int i=-1)    { return count(num_relabels,i); }
int updates(int i=-1)     { return count(num_updates,i); }
int gaps(int i=-1)        { return count(num_gaps,i); }
int gap_nodes(int i=-1)   { return count(num_gap_nodes,i); }
int inspections(int i=-1) { return count(num_inspections,i); }

float cpu_time(int i=-1) { return count(cputime,i); }
float upd_time(int i=-1) { return count(update_time,i); }


void set_heuristic(float hh) { h = hh; }

void set_run_phase0(bool b) { run_phase0 = b; }


NT flow() { return fval; }


void reset_counters()
{ for(int i=0; i<3; i++) 
  { num_pushes[i] = 0;
    num_f_pushes[i] = 0;
    num_relabels[i] = 0;
    num_updates[i] = 0;
    num_gaps[i] = 0;
    num_gap_nodes[i] = 0;
    num_inspections[i] = 0;
    cputime[i] = 0;
    update_time[i] = 0;
    gap_time = 0;
   }
}



template<class cap_array, class flow_array>
bool check_current_edge(const graph& G, node v, 
                                        const cap_array& cap,
                                        const flow_array& flow,
                                        const dist_array& dist)

{ int err_count = 0;

  int dv = dist[v];

  bool out;
  edge e_cur = G.get_current_edge(v,out);
  edge e;
  forall_incident_edges(e,v,out)
  { if (e == e_cur) break;
    node w = G.opposite(e,v);
    if (residual_cap(e,out) > 0 && dist[w] == dv-1) err_count++;
   }
  assert(e == e_cur);

  return err_count == 0;
}



template<class cap_array, class flow_array>
bool check_in_edges(const graph& G, const cap_array& cap,
                                    const flow_array& flow)
{ int count = 0;
  edge e;
  forall_edges(e,G)
  { if (in_cap(e) == 0) continue;
     if (G.hidden_in_edge(e)) count++;
   }

  return count == 0;
}



template<class cap_array, class flow_array>
bool check_dist_labels(const graph& G, const cap_array& cap,
                                       const flow_array& flow,
                                       dist_array& dist)
{ int err_count = 0;
  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v)
    { node w = G.opposite(e,v);
      if (out_cap(e) > 0 && dist[v] > dist[w]+1) err_count++;
      if (in_cap(e) > 0 && dist[w] > dist[v]+1) err_count++;
    }
  }
  return err_count == 0;
}



template<class cap_array, class flow_array>
void print_edge_statistics(const graph& G, const cap_array& cap, 
                                           const flow_array& flow)
{ int full_count = 0;
  int empty_count = 0;
  edge e;
  forall_edges(e,G)
  { if (out_cap(e) == 0) full_count++;
    if (in_cap(e) == 0) empty_count++;
   }
  cout << endl;
  cout << "empty: " << empty_count << "  full: " << full_count << endl;
}



int clean_up(const graph& G, dist_array& dist, node* V, int d)
{
  // remove all nodes v with dist[v] >= d from V and set dist[v] = n
  // return new size of V

 int n = G.number_of_nodes();

 node* q = V;
 for(node* p = V; *p; p++)
 { node v = *p;
   if (dist[v] >= d)
      dist[v] = n;
   else 
      *q++ = v;
  }
 *q = 0;

 return q-V;
}



template<class cap_array, class flow_array>
void compute_dist0(const graph& G, node* queue, node s, node t,
                                                 const cap_array& cap,
                                                 const flow_array& flow,
                                                 excess_array& excess,
                                                 dist_array& dist,
                                                 level_queue& U,
                                                 int* count)
{ 
  // compute exact distance values by a "backward" bfs in the
  // residual network starting at t

  float T = used_time();

  U.clear();

  num_updates[0]++;

  int n = G.number_of_nodes();

  node v;
  forall_nodes(v,G) dist[v] = n;

  dist[t] = n+1;
  queue[0] = t;
  count[0] = 1;

  node* queue_cur  = queue;
  node* queue_last = queue;
  node* level_last = queue;

  int       d = 0;

  int min_cut = MAXINT;
  double  cut = 0;

  double max_edge_insp = n*sqrt(double(n));
  int edge_insp = 0;

  while (queue_cur <= queue_last)
  { 
    node v = *queue_cur;
    assert(dist[v] == n+1);

    G.reset_current_edge(v);

    dist[v] = d;
    if (excess[v] > 0 || v == t) U.insert(v,d);

    cut += excess[v];

    if ((queue_last-queue+1) < n-1  && edge_insp < max_edge_insp)
    { edge e;
      forall_in_edges(e,v)
      { edge_insp++;
        node u = G.opposite(e,v); 
        int du = dist[u];
        if (du > d) cut += out_cap(e);
        if (du == n && u != s)
        { dist[u] = n+1;
          *++queue_last = u;
         }
       }

      forall_out_edges(e,v)
      { node u = G.opposite(e,v); 
        if (u != v && dist[u] <= d) cut -= out_cap(e);
       }
      if (cut < min_cut) min_cut = (int)cut;

    }

    if (queue_cur == level_last)  // finish level d
    { count[++d] = queue_last - level_last;
      level_last = queue_last;
     }

    queue_cur++;
  }

 assert(count[d] == 0);
 assert(dist[s] == n);
 assert(d < n);


 if ((queue_last-queue+1) < n-1  && edge_insp >= max_edge_insp)
 { d--;
   node v;
   forall_nodes(v,G)
   { if (dist[v] < n || v == s) continue; 
     dist[v] = d;
     count[d]++;
     if (excess[v] > 0) U.insert(v,d);
     G.reset_current_edge(v);
     *queue_cur++ = v;
    }
  }

 *queue_cur = 0;

 cut_bound = min_cut;

 update_time[0] += used_time(T);
}




template<class cap_array, class flow_array>
int compute_dist1(const graph& G, node* V, node s, node t,
                                                   const cap_array& cap,
                                                   const flow_array& flow,
                                                   excess_array& excess,
                                                   dist_array& dist,
                                                   level_queue& U,
                                                   int* count)
{
  // compute exact distance values by a "backward" bfs in the
  // residual network starting at t

  float T = used_time();

  num_updates[1]++;

  int n = G.number_of_nodes();

  U.clear();

  double max_edge_insp = n*sqrt(double(n));
  int edge_insp = 0;

  clean_up(G,dist,V,n);
  
  node* p = V;
  while (*p)
  { node v = *p++;
    dist[v] = n;
    G.swap_in_edges(v);
   }

  int initial_sz = p-V;

  node* queue = new node[n];

  dist[t] = 0;
  queue[0] = t;
  count[0] = 1;

  node* queue_cur  = queue;
  node* queue_last = queue;
  node* queue_max  = queue + initial_sz - 1;
  node* level_last = queue;

  int d = 0;

  G.reset_current_edge(t);

  while (queue_cur <= queue_last) 
  { node v = *queue_cur;
    G.reset_current_edge(v);
    if (excess[v] > 0 || v == t) U.insert(v,d);

    if (queue_last < queue_max && edge_insp < max_edge_insp)
    { edge e;
      bool out;
      forall_incident_edges(e,v,out)
      { edge_insp++;
        if (residual_cap(e,!out) == 0) continue;
        node u = G.opposite(e,v); 
        if (dist[u] == n)
        { dist[u] = d+1;
          //G.set_current_edge(u,e,out);
          *++queue_last = u;
         }
       }
     }

    if (queue_cur == level_last)       // finish level d
    { count[++d] = queue_last - level_last;
      level_last = queue_last;
/*
cout << string("reached: %5d  edge_insp = %8d   %.2f sec",
                 queue_last-queue+1,edge_insp,used_time()-T) << endl;
*/
     }

    queue_cur++;
  }

  assert(d < n);
  assert(count[d] == 0);


  if (queue_last < queue_max && edge_insp >= max_edge_insp)
  { d--;
    node* V_stop = V + initial_sz;
    for(node* p = V; p != V_stop; p++)
    { node v = *p;
      if (dist[v] < n) continue; 
      *queue_cur++ = v;
      dist[v] = d;
      count[d]++;
      if (excess[v] > 0) U.insert(v,d);
      G.reset_current_edge(v);
     }
   }

  while (++d < n && count[d] > 0) count[d] = 0;

  int sz = 0;
  for(node* p = queue; p < queue_cur; p++)  
  { node v = *p;
    V[sz++] = v;
    G.swap_in_edges(v);
   }

  V[sz] = 0;

  delete[] queue;

  assert(sz <= initial_sz);
  assert(dist[s] == n);
  
  //assert(check_dist_labels(G,cap,flow,dist));

  update_time[1] += used_time(T);
  return sz;
}



template<class flow_array>
void compute_dist2(const graph& G, node* V, node s, node t,
                   const flow_array& flow,
                   excess_array& excess,
                   dist_array& dist,
                   level_queue& U)
{ 
  // compute exact distance values by a forward bfs in the
  // residual network starting at s

  float T = used_time();

  num_updates[2]++;

  int n = G.number_of_nodes();

  U.clear();
  U.insert(s,0);

  node v;
  forall_nodes(v,G) dist[v] = n;

  node* queue = V;

  dist[s] = 0;
  queue[0] = s;

  int d = 0;

  node* queue_cur  = queue;
  node* queue_last = queue;
  node* level_last = queue;

  while (queue_cur <= queue_last) 
  { node v = *queue_cur;
    if (v != t && excess[v] > 0) U.insert(v,d);
    edge e;
    forall_out_edges(e,v)
    { if (in_cap(e) == 0) continue;
      node u = G.opposite(e,v); 
      if (dist[u] == n)
      { dist[u] = d+1;
        *++queue_last = u;
       }
     }

    if (queue_cur == level_last)  // finish level d
    { d++;
      level_last = queue_last;
     }

    queue_cur++;
  }

 *queue_cur = 0;

 update_time[2] += used_time(T);

}



void handle_gap(const graph& G, node* V, node v, dist_array& dist, int* count)
{
  float T = used_time();

  int gap_nodes = 1;

  int n = G.number_of_nodes();

  int dv = dist[v];

  for(int i = dv+1; count[i] > 0; i++)  // sentinel: count[n] = 0
  { gap_nodes += count[i];
    count[i] = 0;
   }

  dist[v] = n;

  if (gap_nodes > 1) 
  { num_gaps[1]++;
    num_gap_nodes[1] += gap_nodes;
    clean_up(G,dist,V,dv);
  }

 gap_time += used_time(T);
}
   



template<class cap_array, class flow_array>
void phase0(const graph& G, node* V, node s, node t, cap_array& cap,
                                                     flow_array& flow,
                                                     excess_array& excess,
                                                     dist_array& dist,
                                                     level_queue& U)
{ 
  int n = G.number_of_nodes();

  int relabel_count = 0;
  int push_count = 0;
  int f_push_count = 0;
  int insp_count = 0;

  excess[t] += 1;  // then t will never be inserted into U

  node v;
  while ((v = U.del_max()) != t) 
  { 
    NT  ev = excess[v]; 
    int dv = dist[v]; 
    int dmax = dv;
  
    while (ev > 0 && dv < n)
    {
      int  dmin = n;    
      edge emin = 0;
      bool eout = true;

      bool out;
      edge e;
      forall_incident_edges(e,v,out)
      { insp_count++;
        NT x = residual_cap(e,out);
        if (x == 0) continue;
        node w = G.opposite(e,v);
        int dw = dist[w];
        if (dw == dv-1)
        { push_count++;
          NT ew = excess[w];
          if (dw <= dmax && ew == 0) U.insert_non_max(w,dw);
          G.unhide_in_edge(e,w);
          if (ev <= x) 
          { push_flow(e,ev,out); 
            excess[w] += ev;
            ev = 0; 
            break;
           }
          push_flow(e,x,out); 
          excess[w] += x;
          ev -= x;
         }
        else
         if (v != w && dw < dmin) { dmin = dw; emin = e; eout = out; }
      }
  
      if (ev == 0) break;
  
      // remaining excess: relabel vertex v 
  
      relabel_count++;
      dv = dmin + 1;
      dist[v] = dv;

      if (dv >= n) break;

      NT rmin = residual_cap(emin,eout);

      if (ev <= rmin)
      { f_push_count++;
        node w = G.opposite(emin,v);
        NT ew = excess[w];
        push_flow(emin,ev,eout)
        G.unhide_in_edge(emin,w);
        if (ew == 0) U.insert_non_max(w,dmin);
        excess[w] += ev;
        ev = 0;
       }
    }

    excess[v] = ev;
    dist[v] = dv;
  }

  excess[t] -= 1;

  num_pushes[0] = push_count + f_push_count;
  num_f_pushes[0] = f_push_count;
  num_relabels[0] = relabel_count;
  num_inspections[0] = insp_count;

}




template<class cap_array, class flow_array>
void phase1(const graph& G, node* V, node s, node t, cap_array& cap,
                                                    flow_array& flow,
                                                    excess_array& excess,
                                                    dist_array& dist,
                                                    int* count,
                                                    level_queue& U)
{
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  int V_sz = n;

  int heuristic  = (h > 0) ? int(h*m) : int(-h*n);
  int limit_heur = heuristic;

  int relabel_count = 0;
  int push_count = 0;
  int f_push_count = 0;
  int insp_count = 0;

  excess[t] += 1;  // then t will never be inserted into U

  node v;
  while ((v = U.del_max()) != t) 
  {

    for(;;)
    {
      NT  ev = excess[v]; 
      int dv = dist[v]; 
  
      int  dmin = n;    
      edge emin = 0;
      bool eout = true;
  
      bool out;
      edge e_cur = G.get_current_edge(v,out);

      for(edge e = e_cur; e; e = G.next_incident_edge(e,v,out))
      { insp_count++;
        NT x = residual_cap(e,out);
        if (x == 0) continue; 
        node w = G.opposite(e,v);
        int dw = dist[w];
        if (dw == dv-1)
        { push_count++;
          NT ew = excess[w];
          if (ew == 0) U.insert_non_max(w,dw);
          G.unhide_in_edge(e,w);
          if (ev <= x) 
          { push_flow(e,ev,out); 
            excess[w] += ev;
            excess[v] = 0; 
            G.set_current_edge(v,e,out);
            goto NEXT_NODE;
           }
          push_flow(e,x,out); 
          excess[w] += x;
          ev -= x;
         }
        else 
         if (v != w && dw < dmin) { dmin = dw; emin = e; eout = out; }
      }

      excess[v] = ev;
  
      // remaining excess at v
      // relabel vertex v (i.e. update dist[v]) because all
      // admissible edges in the residual graph have been saturated 

      if (--count[dv] == 0) // gap
      { handle_gap(G,V,v,dist,count);
        goto NEXT_NODE;
       }

      if ((h > 0 && insp_count  >= limit_heur) || 
          (h < 0 && relabel_count >= limit_heur))
      { V_sz = compute_dist1(G,V,s,t,cap,flow,excess,dist,U,count);
        limit_heur += heuristic;
        goto NEXT_NODE;
       }

      int  dmin1 = n;
      edge emin1 = 0;
      bool eout1 = true;

      edge e;
      forall_incident_edges(e,v,out)
      { if (e == e_cur) break;
        NT x = residual_cap(e,out);
        if (x == 0) continue;
        node w = G.opposite(e,v);
        int dw = dist[w];
        if (v != w && dw < dmin1) { dmin1 = dw; emin1 = e; eout1 = out; }
      }
      assert(e == e_cur);

      if (dmin1 <= dmin)
      { emin = emin1;
        dmin = dmin1;
        eout = eout1;
       }

      G.set_current_edge(v,emin,eout);

      relabel_count++;

      assert(dmin >= dv);
      dv = dmin + 1;
      dist[v] = dv;
      if (dv >= n) goto NEXT_NODE;

      assert(dv < V_sz);
      count[dv]++;
      U.set_max(dv);

      NT rmin = residual_cap(emin,eout);

      if (ev <= rmin)
      { // final push
        f_push_count++;
        insp_count++;
        node w = G.opposite(emin,v);
        push_flow(emin,ev,eout)
        G.unhide_in_edge(emin,w);
        if (excess[w] == 0) U.insert_non_max(w,dmin);
        excess[w]  += ev;
        excess[v]  = 0;
        goto NEXT_NODE;
       }

    }

   NEXT_NODE:;

  if (cut_bound == excess[t]-1) break;

  }

  excess[t] -= 1;

  num_pushes[1] = push_count + f_push_count;
  num_f_pushes[1] = f_push_count;
  num_relabels[1] = relabel_count;
  num_inspections[1] = insp_count;

  fval = excess[t];
}



template<class cap_array, class flow_array>
void phase2(const graph& G, node* V, node s, node t, cap_array& cap,
                                                     flow_array& flow,
                                                     excess_array& excess,
                                                     dist_array& dist,
                                                     level_queue& U)
{ 
  int n = G.number_of_nodes();
  int m = G.number_of_edges();
  
  int heuristic  = (h > 0) ? int(h*m) : int(-h*n);
  int limit_heur = heuristic;

  int relabel_count = 0;
  int push_count = 0;
  int f_push_count = 0;
  int insp_count = 0;

  compute_dist2(G,V,s,t,flow,excess,dist,U);

  node v;
  while ((v = U.del_max()) != s) 
  {
    NT  ev = excess[v]; 
    int dv = dist[v]; 
    int dmin = n;

    edge e;
    forall_in_edges(e,v)
    { insp_count++;
      NT x = in_cap(e);
      if (x == 0) continue;
      node w = G.opposite(e,v);
      int dw = dist[w];
      if (dw == dv-1)
       { push_count++;
         NT ew = excess[w];
         if (ew == 0) U.insert_non_max(w,dw);
         if (ev <= x) 
         { push_flow(e,ev,false); 
           excess[w] += ev;
           ev = 0; 
           break;
          }
         else
         { push_flow(e,x,false); 
           excess[w] += x;
           ev -= x;
          }
        }
      else
       if (v != w && dw < dmin) dmin = dw;
    } 
    
    excess[v] = ev;
    if (ev == 0) continue;

    if ((h > 0 && insp_count    >= limit_heur) || 
        (h < 0 && relabel_count >= limit_heur))
      { compute_dist2(G,V,s,t,flow,excess,dist,U);
        limit_heur += heuristic;
       }
    else
      { relabel_count++;
        dv = dmin+1;
        dist[v] = dv;
        U.insert_max(v,dv);
       }
  }

  num_pushes[2] = push_count + f_push_count;
  num_f_pushes[2] = f_push_count;
  num_relabels[2] = relabel_count;
  num_inspections[2] = insp_count;

}






template<class cap_array, class flow_array>
NT run(const graph& G, node s, node t, const cap_array& cap, flow_array& flow)
{ 
  float T = used_time();
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  if (s == t) error_handler(1,"MAXFLOW: source == sink");

  nd_array n_data(G);
  ed_array e_data(G);

  succ_array   succ(n_data,     &node_data::succ);
  excess_array excess(n_data,   &node_data::excess);
  dist_array   dist(n_data,     &node_data::dist);

/*
  cap_array& cap = (cap_array&)cap0;
*/

  level_queue U(G,succ);

  int* count = new int[n+1];
  for(int i=0; i<=n; i++) count[i] = 0; // sentinel: count[n] = 0

  node* V = new node[n+1];

  reset_counters();

  edge e;
  forall_edges(e,G) flow[e] = 0;

  cout << endl;
  cout << string("init1:   %.2f sec",used_time()-T) << endl;

  forall_out_edges(e,s) 
  { node u = G.opposite(e,s);
    NT c = out_cap(e);
    push_flow(e,c,true);
    excess[s] -= c;
    excess[u] += c;
   }

  compute_dist0(G,V,s,t,cap,flow,excess,dist,U,count);

  if (m > 10*n) 
  { dynamic_in_edges = true;
float tt = used_time();
    G.hide_all_in_edges();
cout << string("hide:   %.2f sec",used_time()-tt) << endl;
    forall_out_edges(e,s) 
    { node u = G.opposite(e,s);
      G.unhide_in_edge(e,u);
     }
   }

  cout << string("init2:   %.2f sec",used_time()-T) << endl;

  if (run_phase0) 
  { phase0(G,V,s,t,cap,flow,excess,dist,U);
    compute_dist1(G,V,s,t,cap,flow,excess,dist,U,count);
   }
  cputime[0] = used_time(T);

  phase1(G,V,s,t,cap,flow,excess,dist,count,U);
  cputime[1] = used_time(T);

  phase2(G,V,s,t,cap,flow,excess,dist,U);
  cputime[2] = used_time(T);

/*
  forall_edges(e,G) cap[e] = total_cap(e);
*/

  G.restore_all_in_edges();

  delete[] count;
  delete[] V;

  return fval;
}


template<class cap_array, class flow_array>
NT operator()(const graph& G, node s, node t, 
              const cap_array& cap, flow_array& flow) 
{ 
  return run(G,s,t,cap,flow); 
 }



template<class cap_array, class flow_array>
bool compute_min_st_cut(const graph& G, node s, node t, const cap_array& cap, 
                        const flow_array& flow, list<node>& cut)
{
  node_array<bool,graph> reached(G,false);
  reached[s] = true;

  list<node> Q;
  Q.append(s); 
  cut.append(s);

  while ( !Q.empty() )
  { node v = Q.pop(); 
    bool out;
    edge e;
    forall_incident_edges(e,v,out) 
    { node w = G.opposite(e,v);
      if ( !reached[e] && residual_cap(e,out) > 0) 
      { reached[w] = true; 
        Q.append(w); 
        cut.append(w);
       }
    }
  }

  if (reached[t]) { cut.clear(); return false; } // error
  return true;
}


void stat_line(ostream& out,int i)
{ 
  out << string("%10d %8d %10d %10d %7d %4d %7d  %.2f   (%.2f)",
                 pushes(i), f_pushes(i),relabels(i), inspections(i),
                 updates(i), gaps(i), gap_nodes(i), cpu_time(i),upd_time(i));
  out << endl;
 }


void statistics(ostream& out)
{ 
 out << endl;
 out << "    pushes f_pushes   relabels   inspects updates gaps g-nodes  time (update)" << endl;
 out << "-----------------------------------------------------------------------------";
 out << endl;
 for(int i=0; i<=2; i++) stat_line(out,i);
 out << "-----------------------------------------------------------------------------";
 out << endl;
 stat_line(out,-1);
 out << endl;
 out << string("gap time: %.2f   ",gap_time);
 out << string("cut upper bound: %d", cut_bound);
 out << endl;
}


void statistics1(ostream& out)
{ 
  out << endl;
  
  out << string("Maxflow statistics:\n\n");
  out << string("  flow          %8.2f\n", double(fval));
  out << string("  time          %8.2f\n", cpu_time(-1));
  out << string("  update_time   %8.2f\n", upd_time(-1));
  out << string("  gap_time      %8.2f\n", gap_time);
  out << string("  pushes        %8.2f\n", double(pushes(-1)));
  out << string("  relabels      %8.2f\n", double(relabels(-1)));
  out << string("  updates       %8.2f\n", double(updates(-1)));
  out << string("  inspections   %8.2f\n", double(inspections(-1)));
  out << string("  gaps          %8.2f\n", double(gaps(-1)));
  out << string("  gap_nodes     %8.2f\n", double(gap_nodes(-1)));
  
  out << endl;

  out << string("phase 0:\n");
  out << string("  p0_time        %8.2f\n", cputime[0]);
  out << string("  p0_pushes      %8.2f\n", double(pushes(0)));
  out << string("  p0_relabels    %8.2f\n", double(relabels(0)));
  out << string("  p0_updates     %8.2f\n", double(updates(0)));
  out << string("  p0_inspections %8.2f\n", double(inspections(0)));
  out << string("  p0_gaps        %8.2f\n", double(gaps(0)));
  out << string("  p0_gap_nodes   %8.2f\n", double(gap_nodes(0)));
  
  out << endl;
  
  out << string("phase 1:\n");
  out << string("  p1_time        %8.2f\n", cputime[1]);
  out << string("  p1_pushes      %8.2f\n", double(pushes(1)));
  out << string("  p1_relabels    %8.2f\n", double(relabels(1)));
  out << string("  p1_updates     %8.2f\n", double(updates(1)));
  out << string("  p1_inspections %8.2f\n", double(inspections(1)));
  out << string("  p1_gaps        %8.2f\n", double(gaps(1)));
  out << string("  p1_gap_nodes   %8.2f\n", double(gap_nodes(1)));
  
  out << endl;
  
  out << string("phase 2:\n");
  out << string("  p2_time        %8.2f\n", cputime[2]);
  out << string("  p2_pushes      %8.2f\n", double(pushes(2)));
  out << string("  p2_relabels    %8.2f\n", double(relabels(2)));
  out << string("  p2_updates     %8.2f\n", double(updates(2)));
  out << string("  p2_inspections %8.2f\n", double(inspections(2)));
  out << string("  p2_gaps        %8.2f\n", double(gaps(2)));
  out << string("  p2_gap_nodes   %8.2f\n", double(gap_nodes(2)));

  out << endl;
}




template<class cap_array, class flow_array>
bool check(const graph& G, node s, node t, 
           const cap_array& cap, const flow_array& flow,
           string& msg)
{
  edge e;
  forall_edges(e,G) 
  { if (flow[e] < 0 || flow[e] > cap[e]) 
    { msg = string("illegal flow value for edge %d",G.index(e));
      return false;
     }
   }
  
  node_array<NT,graph> excess(G);

  node v;
  forall_nodes(v,G) excess[v] = 0;

  forall_nodes(v,G) 
  { edge e;
    forall_out_edges(e,v) 
    { node w = G.opposite(e,v);
      excess[v] -= flow[e]; 
      excess[w] += flow[e];
     }
  }

  forall_nodes(v,G) 
  { if (v == s  || v == t || excess[v] == 0) continue;
    msg = "node with non-zero excess";
    return false;
  }

/*
  if (fval != excess[t])
  { msg = "fval != excess[t]";
    return false;
   }
*/
  
  node_array<bool,graph> reached(G,false);
  reached[s] = true;

  list<node> Q;
  Q.append(s); 

  while ( !Q.empty() )
  { node v = Q.pop(); 
    bool out;
    edge e;
    forall_incident_edges(e,v,out) 
    { node w = G.opposite(e,v);
      if ( !reached[w] && residual_cap(e,out) > 0) 
      { reached[w] = true; 
        Q.append(w); 
       }
    }
  }

  if (reached[t])
  { msg = "t is reachable in G_f";
    return false;
   }

  forall_nodes(v,G)
  { if (v != s && v != t && excess[v] != 0)
    { msg = string("node %d has non-zero excess",G.index(v));
      return false;
     }
   }

  return true;
}


};


LEDA_END_NAMESPACE


#if !defined(_LEDA_MAX_FLOW_WITH_LCAPS_T)
#include <LEDA/graph/templates/max_flow_with_lcaps.h>
#endif

#if LEDA_ROOT_INCL_ID == 520236
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
