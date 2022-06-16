/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  max_flow.t
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:14 $

#ifndef _LEDA_MAXIMIZE_FLOW_T
#define _LEDA_MAXIMIZE_FLOW_T

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520241
#include <LEDA/internal/PREAMBLE.h>
#endif


/*
#include <LEDA/graph/node_level_queue.h>
*/

LEDA_BEGIN_NAMESPACE

#define NULL_NODE  ((node)0xFFFFFFFF)

template <class graph, class succ_array>
class node_level_queue {

typedef typename graph::node node;
typedef typename graph::edge edge;

node*  T;
node*  head;
node*  max;

succ_array& NEXT;

public:



node_level_queue(const graph& G, succ_array& succ) : NEXT(succ)
{ 
  int sz = G.number_of_nodes();
  T = new node[sz+1];
  for(node* p = T+sz; p >= T;  p--) *p = nil; 
  head = T+1;
  max = T;
  T[0] = NULL_NODE; 
 }

~node_level_queue() { delete[] T; }

void set_max(int i) { max = head + i; }

void insert(node v, int i)
{ // insert on level i
  node* h = head + i;
  if (h > max) max = h;
  NEXT[v] = *h;
  *h = v;
 }


void insert_non_max(node v, int i)
{ // do not update max pointer
  node* h = head + i;
  NEXT[v] = *h;
  *h = v;
 }


void insert_max(node v, int i)
{ // new max at level i
  max = head + i;
  *max = v;
  NEXT[v] = 0;
 }

node find_max()
{ while (*max == nil) max--;
  return *max;
}

node del_max()
{ node v = *max;
  *max = NEXT[v];
  return v; 
}

void del_max(node v) { *max = NEXT[v]; }


void clear() { while (max >= head) *max-- =  nil; }


};



#define RCOST(e,u,v) (cost[e] + pot[u] - pot[v])

typedef int NT;


template<class NT, class graph>
class max_flow  
{
  typedef typename graph::node node;
  typedef typename graph::edge edge;

  float h;

  NT fval;
  NT delta;
  float cputime;

  node source(const graph& G, edge e, node t)
  { if (graph::category() == opposite_graph_category)
      return G.opposite(e,t);
    else
      return G.source(e);
  }
  
  node target(const graph& G, edge e, node s)
  { if (graph::category() == opposite_graph_category)
      return G.opposite(e,s);
    else
      return G.target(e);
  }

public:

max_flow() : h(5.0),fval(0),delta(1) {}

void set_heuristic(float hh) { h = hh; }
void set_delta(int d) { delta = d; }

NT flow() { return fval; }

float cpu_time() const { return cputime; }


template<class cap_array, class flow_array, class excess_array,
                                            class n_level_queue,
                                            class dist_array,
                                            class succ_array,
                                            class cost_array,
                                            class pot_array>
void compute_dist1(const graph& G, list<node>& A, list<node>& B,
                   const cap_array& cap,
                   const flow_array& flow,
                   excess_array& excess,
                   succ_array& succ,
                   dist_array& dist,
                   n_level_queue& U,
                   const cost_array& cost,
                   const pot_array& pot,
                   int* count)
{ 
  // compute exact distance values by a "backward" bfs in the
  // residual network starting at t

  int n = G.number_of_nodes();
  dist.init(G,n);

  node level_last  = 0;
  node queue_last  = 0;
  node queue_first = 0;
  node v;

  forall(v,B)
  {
    if (excess[v] >= 0)  continue;

    dist[v] = 0;
    if (queue_last == 0)
      { queue_first= v;
        queue_last = v;
        level_last = v;
       }
    else
      { succ[queue_last] = v;
        queue_last = v;
        level_last = v;
       }
   }

  if (!queue_first) return;

  int d = 0;
  int level_sz = 0;

  v = queue_first;

  for(;;)
  { 
    level_sz++;

    edge e;
    forall_out_edges(e,v)
    { if (flow[e] < delta) continue;
      node u = target(G,e,v); 
      if (RCOST(e,v,u) != 0) continue;
      int& du = dist[u];
      if (du == n)
      { du = d+1;
        succ[queue_last] = u;
        queue_last = u;
       }
     }

    forall_in_edges(e,v)
    { if (cap[e] - flow[e] < delta) continue;
      node u = source(G,e,v); 
      if (RCOST(e,u,v) != 0) continue;
      int& du = dist[u];
      if (du == n)
      { du = d+1;
        succ[queue_last] = u;
        queue_last = u;
       }
    }

    node next = succ[v];

    if (excess[v] > 0) U.insert(v,d);

    if (v == level_last)             
    { // finish level d
      count[d++] = level_sz;
      level_sz = 0;
      if (queue_last == level_last) break; // no level d+1: stop
      level_last = queue_last;
     }
    v = next;
  }

 while (d < n) count[d++] = 0;

/*
 forall(v,B)
   assert(dist[v] < n);
*/

}



template <class cap_array, class flow_array, class cost_array, 
          class pot_array,class dist_array, class succ_array>

void handle_gap(const graph& G, node v, const cap_array& cap,
                                        const flow_array& flow,
                                        dist_array& dist, 
                                        succ_array& succ, 
                                        const cost_array& cost,                                                         const pot_array& pot, 
                                        int* count)
{ 
  int gap_nodes = 0;

  int n  = G.number_of_nodes();
  int dv = dist[v];
  for (int i = dv; i < n && count[i] > 0; i++) count[i] = 0;

  dist[v] = n;

  node queue_last = v;
  succ[v] = v;

  do { gap_nodes++;

       v = succ[v];

       edge e;
       forall_out_edges(e,v)
       { if (cap[e]-flow[e] < delta) continue;
         node u = target(G,e,v);
         if (RCOST(e,v,u) != 0) continue;
         int& du = dist[u];
         if (du < n && du > dv)
         { succ[queue_last] = u;
           queue_last = u; 
           du = n;
          }
       }

       forall_in_edges(e,v)
       { if (flow[e] < delta) continue;
         node u = source(G,e,v);
         if (RCOST(e,u,v) != 0) continue;
         int& du = dist[u];
         if (du < n && du > dv)
         { succ[queue_last] = u;
           queue_last = u;
           du = n;
          }
        }

   } while (v != queue_last);

}



template<class cap_array, class flow_array, class excess_array,
                                            class dist_array,
                                            class succ_array,
                                            class cost_array, 
                                            class pot_array>
void phase1(const graph& G, list<node>& A, list<node>& B, 
                                           const cap_array& cap,
                                           flow_array& flow,
                                           excess_array& excess,
                                           dist_array& dist,
                                           succ_array& succ,
                                           const cost_array& cost,
                                           const pot_array& pot)
{ 
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  int heuristic  = int(h*m);
  int limit_heur = heuristic;

  int insp_count = 0;

  node_level_queue<graph,succ_array>  U(G,succ);
  int* count = new int[n];

  compute_dist1(G,A,B,cap,flow,excess,succ,dist,U,cost,pot,count);

  node v;
  while ((v = U.find_max()) != NULL_NODE) 
  {

    U.del_max(v);


    NT  ev = excess[v]; 
    int dv = dist[v]; 

//cout << string("v = %d  ev = %d   dv = %d", G.index(v), ev,dv) << endl; 

    int  dmin = n;    
    edge emin = 0;
    NT   rmin = 0;

    edge e;
    forall_out_edges(e,v)
    { insp_count++;
      NT fe = flow[e];
      NT rc = cap[e] - fe;
      if (rc < delta) continue;
      node w = target(G,e,v);
      if (RCOST(e,v,w) != 0) continue;
      int dw = dist[w];
      if (dw == dv-1)
      { NT ew = excess[w];
        if (ev <= rc) 
        { flow[e] = fe + ev; 
          excess[w] = ew + ev;
          excess[v] = 0; 
          if (ew <= 0 && excess[w] > 0) U.insert_non_max(w,dw);
          goto NEXT_NODE;
         }
        flow[e] = fe + rc;
        excess[w] = ew + rc;
        ev -= rc;
        if (ew <= 0 && excess[w] > 0) U.insert_non_max(w,dw);
        continue;
       }
      if (v != w && dw < dmin) { dmin = dw; emin = e; rmin = rc; }
    }

    forall_in_edges(e,v)
    { insp_count++;
      NT fe = flow[e];
      if (fe < delta) continue;
      node w = source(G,e,v);
      if (RCOST(e,w,v) != 0) continue;
      int dw = dist[w];
      if (dw == dv-1)
      { NT ew = excess[w];
        if (ev <= fe) 
        { flow[e] = fe - ev; 
          excess[w] = ew + ev;
          excess[v] = 0; 
          if (ew <= 0 && excess[w] > 0) U.insert_non_max(w,dw);
          goto NEXT_NODE;
         }
        flow[e] = 0;
        excess[w] = ew + fe;
        ev -= fe;
        if (ew <= 0 && excess[w] > 0) U.insert_non_max(w,dw);
        continue;
       }
      if (v != w && dw < dmin) { dmin = dw; emin = e; rmin = -fe; }
    } 

    excess[v] = ev;

    // remaining excess at v
    // relabel vertex v (i.e. update dist[v]) because all
    // admissible edges in the residual graph have been saturated 

    if (--count[dv] == 0) // gap
    { handle_gap(G,v,cap,flow,dist,succ,cost,pot,count);
      goto NEXT_NODE;
     }

    dv = dmin + 1;
    dist[v] = dv;

    if (dv >= n) goto NEXT_NODE;

    count[dv]++;

    if (ev <= rmin || ev <= -rmin)
    { node w = G.opposite(emin,v);
      if (excess[w] == 0) U.insert(w,dmin);
      excess[w]  += ev;
      flow[emin] += (rmin > 0) ? ev : -ev;
      excess[v]  = 0;
      goto NEXT_NODE;
     }

    if (insp_count >= limit_heur)
    { U.clear();
      compute_dist1(G,A,B,cap,flow,excess,succ,dist,U,cost,pot,count);
      limit_heur += heuristic;
      goto NEXT_NODE;
     }

   U.insert_max(v,dv);

NEXT_NODE:;
  }

  delete[] count;

}


template<class cap_array, class flow_array, class excess_array,
                                            class dist_array, 
                                            class succ_array,
                                            class cost_array, 
                                            class pot_array>

int run(const graph& G, const cap_array& cap, flow_array& flow, 
                                               excess_array& excess,
                                               dist_array& dist,
                                               succ_array& succ,
                                               const cost_array& cost, 
                                               const pot_array& pot) 
{ 
  float T = used_time();

  list<node> A;
  list<node> B;

  int total_excess = 0;

  node v;
  forall_nodes(v,G)
  { NT ex = excess[v];
    if (ex >= delta) { A.append(v); total_excess += ex; }
    if (ex <= -delta) B.append(v);
  }

  //cout << "|A| = " << A.length() << endl;
  //cout << "|B| = " << B.length() << endl;

  phase1(G,A,B,cap,flow,excess,dist,succ,cost,pot);
  phase1(G,B,A,cap,flow,excess,dist,succ,cost, pot);

  forall_nodes(v,G)
  { NT ex = excess[v];
    if (ex >= delta) { A.append(v); total_excess -= ex; }
   }

  cputime = used_time(T);

  return total_excess;
}

};

LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520241
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
