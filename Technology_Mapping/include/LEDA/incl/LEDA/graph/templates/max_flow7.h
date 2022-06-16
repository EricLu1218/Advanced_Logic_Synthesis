/******************************************************************************r
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

// $Revision: 1.25 $  $Date: 2007/02/28 07:44:14 $

#ifndef _LEDA_MAX_FLOW_T
#define _LEDA_MAX_FLOW_T

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520206
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

#define residual_cap(e) (e.out() ? out_cap(e) : in_cap(e))
#define push_flow(e,x)  change_flow(e,(e.out() ? x : -(x)))



template<class NT>
struct mf_node_data { 
  GenPtr succ; // node
  GenPtr curr; // incident_edge (current edge)
  NT     excess;
  int    dist;
};

template<class NT> 
struct mf_edge_data {
};



template<class NT=int, class graph=int, class nd_array=int, class ed_array=int>
class max_flow;


template<class NT>
class max_flow<NT,int,int,int>
{ public:
  typedef mf_node_data<NT> node_data;
  typedef mf_edge_data<NT> edge_data;
};




template<class NT, class graph, class nd_array, class ed_array>
class max_flow  
{
public:

  typedef typename graph::node node;
  typedef typename graph::edge edge;
  typedef typename graph::incident_edge incident_edge;

  typedef mf_node_data<NT> node_data;
  typedef mf_edge_data<NT> edge_data;


private:

  //check_slots<node_data,node_slots> check_dummy1;
  //check_slots<edge_data,edge_slots> check_dummy2;

 typedef member_array<GenPtr, nd_array, node_data, node> succ_array;
 typedef member_array<GenPtr, nd_array, node_data, incident_edge> current_array;
 typedef member_array<NT,     nd_array, node_data> excess_array;
 typedef member_array<int,    nd_array, node_data> dist_array;

 typedef node_level_queue<graph,succ_array> level_queue;


  float updf;
  int run_phase0;

  NT fval;
  NT cut_bound;

  // statistics (phase 1, 2, and sum)

  unsigned int num_pushes[3];
  unsigned int num_f_pushes[3];
  unsigned int num_relabels[3];
  unsigned int num_updates[3];
  unsigned int num_gaps[3]; 
  unsigned int num_gap_nodes[3]; 
  unsigned int num_inspections[3];

  float cputime[3];
  float update_time[3];

  float init_time;
  float gap_time;
  bool  dense_graph;


template<class T>
T count(T* counter, int i = -1) 
{ if (i < 0) 
    return counter[0] + counter[1] + counter[2];
  else 
    return counter[i];
}

public:

max_flow()
{ updf = 3.0;
  run_phase0 = -1;
  dense_graph = false;
  reset_counters(); 
  cut_bound = 0;
  fval = 0;
}

unsigned int pushes(int i=-1)      { return count(num_pushes,i); }
unsigned int f_pushes(int i=-1)    { return count(num_f_pushes,i); }
unsigned int relabels(int i=-1)    { return count(num_relabels,i); }
unsigned int updates(int i=-1)     { return count(num_updates,i); }
unsigned int gaps(int i=-1)        { return count(num_gaps,i); }
unsigned int gap_nodes(int i=-1)   { return count(num_gap_nodes,i); }
unsigned int inspections(int i=-1) { return count(num_inspections,i); }

float cpu_time(int i=-1) { return count(cputime,i); }
float upd_time(int i=-1) { return count(update_time,i); }


void set_update_f(float f) { updf = f; }

void set_run_phase0(bool b) { run_phase0 = b ? 1 : 0; }


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
   }
 gap_time = 0;
 init_time = 0;
}



template<class cap_array, class flow_array>
bool check_current_edge(const graph& G, node v, 
                                        const cap_array& cap,
                                        const flow_array& flow,
                                        const dist_array& dist,
                                        const current_array& current)

{ int err_count = 0;

  int dv = dist[v];

  incident_edge e_cur = current[v];

  incident_edge e = 0;
  forall_incident_edges(e,v)
  { if (e == e_cur) break;
    node w = G.opposite(e,v);
    if (residual_cap(e) > 0 && dist[w] == dv-1) err_count++;
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
  { incident_edge e;
    forall_incident_edges(e,v)
    { node w = G.opposite(e,v);
      if (residual_cap(e) > 0 && dist[v] > dist[w]+1) err_count++;
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
void initialize(const graph& G, node* queue, node s, node t,
                                                     const cap_array& cap,
                                                     flow_array& flow,
                                                     excess_array& excess,
                                                     dist_array& dist,
                                                     current_array& current,
                                                     level_queue& U,
                                                     int* count)
{ 
  float T = used_time();

  int n = G.number_of_nodes();

  edge e;
  forall_edges(e,G) flow[e] = 0;

  forall_out_edges(e,s) 
  { node u = G.opposite(e,s);
    NT c = out_cap(e);
    change_flow(e,c);
    excess[s] -= c;
    excess[u] += c;
   }

  U.clear();

  num_updates[0]++;


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

  //double max_edge_insp = n*sqrt(double(n));
  double max_edge_insp = (0.1*n)*n;
  int edge_insp = 0;

  while (queue_cur <= queue_last)
  { 
    node v = *queue_cur;
    assert(dist[v] == n+1);

    current[v] = G.first_incident_edge(v);

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
     current[v] = G.first_incident_edge(v);
     *queue_cur++ = v;
    }
  }

 *queue_cur = 0;

 while (++d <= n) count[d] = 0; 

 cut_bound = min_cut;

 update_time[0] += used_time(T);
}




template<class cap_array, class flow_array>
int compute_dist1(const graph& G, node* V, node s, node t,
                                                   const cap_array& cap,
                                                   const flow_array& flow,
                                                   excess_array& excess,
                                                   dist_array& dist,
                                                   current_array& current,
                                                   level_queue& U,
                                                   int* count)
{
  // compute exact distance values by a "backward" bfs in the
  // residual network starting at t

  float T = used_time();

  num_updates[1]++;

  int n = G.number_of_nodes();

  U.clear();

  //double max_edge_insp = n*sqrt(double(n));
  double max_edge_insp = (0.1*n)*n;
  int edge_insp = 0;

  clean_up(G,dist,V,n);
  for(int i=0; count[i] > 0; i++) count[i] = 0;
  
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

  while (queue_cur <= queue_last) 
  { node v = *queue_cur;

    if (excess[v] > 0 || v == t) U.insert(v,d);


    if (queue_last < queue_max && edge_insp < max_edge_insp)
    { 
/*    incident_edge e;
      forall_incident_edges(e,v)
      { incident_edge r = e.reverse();
        if (residual_cap(r) == 0) continue;
        edge_insp++;
        node u = G.opposite(e,v); 
        if (dist[u] == n)
        { dist[u] = d+1;
          *++queue_last = u;
          current[u] = r;
         }
       }
*/
      edge e;
      forall_out_edges(e,v)
      { if (in_cap(e) == 0) continue;
        edge_insp++;
        node u = G.opposite(e,v); 
        if (dist[u] == n)
        { dist[u] = d+1;
          *++queue_last = u;
         }
       }

      forall_in_edges(e,v)
      { if (out_cap(e) == 0) continue;
        edge_insp++;
        node u = G.opposite(e,v); 
        if (dist[u] == n)
        { dist[u] = d+1;
          *++queue_last = u;
         }
       } 
     }

    if (queue_cur == level_last)       // finish level d
    { count[++d] = queue_last - level_last;
      level_last = queue_last;
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
     }
   }

  int sz = 0;
  for(node* p = queue; p < queue_cur; p++)  
  { node v = *p;
    V[sz++] = v;
    G.swap_in_edges(v);
    current[v] = G.first_incident_edge(v);
   }

  V[sz] = 0;

  delete[] queue;

  assert(sz <= initial_sz);
  assert(dist[s] == n);

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

  unsigned int relabel_count = 0;
  unsigned int push_count = 0;
  unsigned int f_push_count = 0;
  unsigned int insp_count = 0;

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
      incident_edge emin = 0;

      incident_edge e;
      forall_incident_edges(e,v)
      { insp_count++;
        NT x = residual_cap(e);
        if (x == 0) continue;
        node w = G.opposite(e,v);
        int dw = dist[w];
        if (dw == dv-1)
        { push_count++;
          NT ew = excess[w];
          if (dw <= dmax && ew == 0) U.insert_non_max(w,dw);
          G.restore_in_edge(e,w);
          if (ev <= x) 
          { push_flow(e,ev); 
            excess[w] += ev;
            ev = 0; 
            break;
           }
          push_flow(e,x); 
          excess[w] += x;
          ev -= x;
         }
        else
         if (v != w && dw < dmin) { dmin = dw; emin = e; }
      }
  
      if (ev == 0) break;
  
      // remaining excess: relabel vertex v 
  
      relabel_count++;
      dv = dmin + 1;
      dist[v] = dv;

      if (dv >= n) break;

      NT rmin = residual_cap(emin);

      if (ev <= rmin)
      { f_push_count++;
        node w = G.opposite(emin,v);
        NT ew = excess[w];
        push_flow(emin,ev)
        G.restore_in_edge(emin,w);
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
                                                     current_array& current,
                                                     int* count,
                                                     level_queue& U)
{ int n = G.number_of_nodes();
  int m = G.number_of_edges();

  int V_sz = n;

  unsigned int delta_update = (updf > 0) ? int(updf*m) : int(-updf*n);
  unsigned int limit_update = delta_update;

  unsigned int relabel_count = 0;
  unsigned int push_count = 0;
  unsigned int f_push_count = 0;
  unsigned int insp_count = 0;

  excess[t] += 1;  // then t will never be inserted into U

  node v;
  while ((v = U.del_max()) != t) 
  {
    assert(G.outdeg(v) > 0);

    incident_edge e_cur = current[v];

    for(;;)
    {
      NT  ev = excess[v]; 
      int dv = dist[v]; 

      incident_edge emin = 0;
      int dmin = n;    
      NT  rmin = 0;

      //incident_edge e;
      //forall_cyclic_incident_edges(e,e_cur,v)

      int i = 0;
      for(incident_edge e = e_cur; e != e_cur || i++ == 0;
                                   e = G.next_cyclic_incident_edge(e,v))
      { insp_count++;
        NT x = residual_cap(e);
        if (x == 0) continue;
        node w = G.opposite(e,v);
        int dw = dist[w];
        if (dw == dv-1)
        { push_count++;
          NT ew = excess[w];
          if (ew == 0) U.insert_non_max(w,dw);
          G.restore_in_edge(e,w);
          if (ev <= x) 
          { push_flow(e,ev); 
            excess[w] += ev;
            excess[v] = 0; 
            e_cur = e;
            goto NEXT_NODE;
           }
          push_flow(e,x); 
          excess[w] += x;
          ev -= x;
         }
        else 
         if (v != w && dw < dmin) { dmin = dw; emin = e; rmin = x; }
      }


      excess[v] = ev;
  
      // remaining excess at v
      // relabel vertex v (i.e. update dist[v]) because all
      // admissible edges in the residual graph have been saturated 

      if (--count[dv] == 0) // gap
      { handle_gap(G,V,v,dist,count);
        goto NEXT_NODE;
       }

      if ((updf > 0 && insp_count  >= limit_update) || 
          (updf < 0 && relabel_count >= limit_update))
      { V_sz = compute_dist1(G,V,s,t,cap,flow,excess,dist,current,U,count);
        limit_update += delta_update;
        goto NEXT_NODE;
       }

      relabel_count++;
      assert(dmin >= dv);

      dv = dmin + 1;
      dist[v] = dv;
      if (dv >= n) goto NEXT_NODE;
      assert(dv < V_sz);
      count[dv]++;
      U.set_max(dv);
      e_cur = emin;

      if (ev <= rmin)   // final push
      { f_push_count++;
        insp_count++;
        node w = G.opposite(emin,v);
        push_flow(emin,ev)
        G.restore_in_edge(emin,w);
        if (excess[w] == 0) U.insert_non_max(w,dmin);
        excess[w]  += ev;
        excess[v]  = 0;
        goto NEXT_NODE;
       }
    }

   NEXT_NODE:;
   current[v] = e_cur;

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
{ int n = G.number_of_nodes();
  int m = G.number_of_edges();
  
  unsigned int delta_update  = (updf > 0) ? int(updf*m) : int(-updf*n);
  unsigned int limit_update = delta_update;

  unsigned int relabel_count = 0;
  unsigned int push_count = 0;
  unsigned int f_push_count = 0;
  unsigned int insp_count = 0;

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
         { change_flow(e,-ev);
           excess[w] += ev;
           ev = 0; 
           break;
          }
         else
         { change_flow(e,-x);
           excess[w] += x;
           ev -= x;
          }
        }
      else
       if (v != w && dw < dmin) dmin = dw;
    } 
    
    excess[v] = ev;
    if (ev == 0) continue;

    if ((updf > 0 && insp_count    >= limit_update) || 
        (updf < 0 && relabel_count >= limit_update))
      { compute_dist2(G,V,s,t,flow,excess,dist,U);
        limit_update += delta_update;
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

  reset_counters();

  nd_array n_data(G);
  ed_array e_data(G);

  succ_array    succ(n_data,   &node_data::succ);
  current_array current(n_data,&node_data::curr);
  excess_array  excess(n_data, &node_data::excess);
  dist_array    dist(n_data,   &node_data::dist);

/*
  cap_array& cap = (cap_array&)cap0;
*/

  level_queue U(G,succ);
  int*  count = new int[n+1];
  node* V = new node[n+1];

  initialize(G,V,s,t,cap,flow,excess,dist,current,U,count);

  if (m > n*std::log(1.0*n))
  { dense_graph = true;
    if (run_phase0 < 0) run_phase0 = 0;
    G.hide_all_in_edges();
    edge e;
    forall_out_edges(e,s) 
    { node u = G.opposite(e,s);
      G.restore_in_edge(e,u);
     }
   }
  else
  { dense_graph = false;
    if (run_phase0 < 0) run_phase0 = 1;
/*
    rand_int.set_seed(7*n+m/3);
    G.permute_edges();
*/
   }

  init_time = used_time()-T;

  if (run_phase0 == 1) 
  { phase0(G,V,s,t,cap,flow,excess,dist,U);
    compute_dist1(G,V,s,t,cap,flow,excess,dist,current,U,count);
   }
  cputime[0] = used_time(T);

  phase1(G,V,s,t,cap,flow,excess,dist,current,count,U);
  cputime[1] = used_time(T);

  phase2(G,V,s,t,cap,flow,excess,dist,U);
  cputime[2] = used_time(T);

/*
  forall_edges(e,G) cap[e] = total_cap(e);
*/

  G.restore_all_edges();

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
    incident_edge e = 0;
    forall_incident_edges(e,v) 
    { node w = G.opposite(e,v);
      if ( !reached[e] && residual_cap(e) > 0) 
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
 out << (dense_graph ? "dense graph   " : "sparse graph  ");
 out << string("updf = %.1f   ",updf);
 out << string("init: %.2f   ",init_time);
 out << string("gap: %.2f   ",gap_time);
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
    incident_edge e = 0;
    forall_incident_edges(e,v) 
    { node w = G.opposite(e,v);
      if ( !reached[w] && residual_cap(e) > 0) 
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




#ifndef LEDA_NO_TEMPL_FCTS_FOR_STD_GRAPH

template<class NT>
NT MAX_FLOW_T(const graph& G, node s, node t, const edge_array<NT>& cap, 
                                              edge_array<NT>& flow, 
                                              int& num_pushes, 
                                              int& num_edge_inspections,
                                              int& num_relabels,
                                              int& num_global_relabels,
                                              int& num_gaps,
                                              float h)
{ max_flow<NT,graph> mf;
  mf.set_heuristic(h);
  NT f = mf.run(G,s,t,cap,flow);
  num_pushes = mf.pushes(0);
  num_relabels = mf.relabels(0);
  num_global_relabels = mf.updates(0);
  num_edge_inspections = mf.inspections(0);
  num_gaps = mf.gap_nodes(0);
  return f;
}



template<class NT>
NT MAX_FLOW_T(const graph& G, node s, node t, const edge_array<NT>& cap, 
                                              edge_array<NT>& flow)
{ max_flow<NT,graph> mf;
  return mf.run(G,s,t,cap,flow);
}


template<class NT>
NT MAX_FLOW_T(const graph& G, node s, node t, const edge_array<NT>& cap, 
              edge_array<NT>& flow, list<node>& st_cut)
{ max_flow<NT,graph> mf;
  NT val = mf.run(G,s,t,cap,flow);
  mf.compute_min_st_cut(G, s, t, cap, flow, st_cut);
  return val;
}


template <class NT>
bool CHECK_MAX_FLOW_T(const graph& G, node s, node t,
                      const edge_array<NT>& cap, const edge_array<NT>& flow)
{ max_flow<NT,graph> mf;
  string msg;
  bool ok = mf.check(G,s,t,cap,flow,msg);
  if (!ok) error_handler(1,string("CHECK_MAX_FLOW_T: ") + msg + ".");
  return ok;
}

#endif // #ifdef LEDA_NO_TEMPL_FCTS_FOR_STD_GRAPH
  

LEDA_END_NAMESPACE


#if !defined(_LEDA_MAX_FLOW_WITH_LCAPS_T)
#include <LEDA/graph/templates/max_flow_with_lcaps.h>
#endif

#if LEDA_ROOT_INCL_ID == 520206
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
