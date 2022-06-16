/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  mcf_cap_scaling3.t
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.4 $  $Date: 2007/02/28 07:44:14 $

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520220
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_pq22.h>
#include <LEDA/core/b_stack.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

#define out_cap(e)       cap[e]
#define in_cap(e)        flow[e]
#define total_cap(e)     (cap[e]+flow[e])
#define change_flow(e,x) (flow[e] += x, cap[e] -= x)

/*
#define out_cap(e)       (cap[e]-flow[e])
#define in_cap(e)        flow[e]
#define total_cap(e)     cap[e]
#define change_flow(e,x) (flow[e] += x)
*/


template <class NT, class graph_t      = graph,
                    class pot_array    = node_array<NT,graph_t>,
                    class excess_array = node_array<NT,graph_t>,
                    class dist_array   = node_array<NT,graph_t>,
                    class pred_array   = node_array<edge,graph_t>,
                    class mark_array   = node_array<NT,graph_t>,
                    class node_pq_t    = node_pq22<NT,graph_t> >

class mcf_cap_scaling 
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  typedef node_pq_t node_pq;
  
  float T;

  double total_c;

  bool write_log;

  int zero_count;


node source(const graph_t& G, edge e, node t)
{ if (graph_t::category() == opposite_graph_category)
    return G.opposite(e,t);
  else
    return G.source(e);
}
  
node target(const graph_t& G, edge e, node s)
{ if (graph_t::category() == opposite_graph_category)
    return G.opposite(e,s);
  else
    return G.target(e);
}


template<class cost_array, class cap_array, class flow_array>
node BFS_IN_RESIDUAL(const graph_t& G, node s, int delta, 
                                       const cost_array&   cost,
                                       const cap_array&    cap,
                                       const flow_array&   flow,
                                       const excess_array& excess,
                                       const pot_array&  pot,
                                       dist_array&  dist,
                                       pred_array& pred,
                                       mark_array& mark, 
                                       node* V,
                                       int& count)
 
{
  node* first = V;
  node* last  = V;
  V[0] = s;

  while (first <= last)
  {
    node u = *first++;
    NT pu = pot[u];

    edge e;
    forall_out_edges(e,u)
    { if (out_cap(e) < delta)  continue;
      node v = target(G,e,u);
      if ((mark[v] & 0x7fffffff) == count) continue;
      if (pot[v]-pu != cost[e]) continue; 
      *++last = v;
      dist[v] = 0;
      pred[v] = e;
      mark[v] = count;
      if (excess[v] <= -delta) return v;
    }

    forall_in_edges(e,u)
    { if (in_cap(e) < delta)  continue;
      node v = source(G,e,u);
      if ((mark[v] & 0x7fffffff) == count) continue;
      if (pu-pot[v] != cost[e]) continue; 
      *++last = v;
      dist[v] = 0;
      pred[v] = e;
      mark[v] = (count | 0x80000000);
      if (excess[v] <= -delta) return v;
    }

  }

  return 0;
}



template<class cost_array, class cap_array, class flow_array>
node DFS_IN_RESIDUAL(const graph_t& G, node s, int delta, 
                                       const cost_array&   cost,
                                       const cap_array&    cap,
                                       const flow_array&   flow,
                                       const excess_array& excess,
                                       const pot_array&  pot,
                                       dist_array&  dist,
                                       pred_array& pred,
                                       mark_array& mark, 
                                       node* V,
                                       int& count)
 
{
  node* top = V;
  V[0] = s;
  pred[s] = 0;

  while (top >= V)
  {
    node u = *top--;

    NT pu = pot[u];

    edge e;
    forall_out_edges(e,u)
    { if (out_cap(e) < delta)  continue;
      node v = target(G,e,u);
      if ((mark[v] & 0x7fffffff) == count) continue;
      if (pot[v]-pu != cost[e]) continue; 
      *++top = v;
      dist[v] = 0;
      pred[v] = e;
      mark[v] = count;
      if (excess[v] <= -delta) return v;
    }

    forall_in_edges(e,u)
    { if (in_cap(e) < delta)  continue;
      node v = source(G,e,u);
      if ((mark[v] & 0x7fffffff) == count) continue;
      if (pu-pot[v] != cost[e]) continue; 
      *++top = v;
      dist[v] = 0;
      pred[v] = e;
      mark[v] = (count | 0x80000000);
      if (excess[v] <= -delta) return v;
    }

  }

  return 0;
}



template<class cost_array, class cap_array, class flow_array>
node DIJKSTRA_IN_RESIDUAL(const graph_t& G, node s, int delta, 
                             const cost_array&   cost,
                             const cap_array&    cap,
                             const flow_array&   flow,
                             const excess_array& excess,
                             pot_array&  pot,
                             dist_array& dist,
                             pred_array& pred,
                             mark_array& mark,
                             node_pq&  PQ, 
                             node* V,
                             int count)

{
  node t = 0;
  NT  dt = MAXINT;

  V[0] = s;
  dist[s] = 0;
  mark[s] = count;
  pred[s] = nil;

  node* top = V;

  while (top >= V || !PQ.empty())
  { 
    node u;

    if (top >= V)
      u = *top--;
    else
      u = PQ.del_min(dist);

    if (u == t) break;

    NT  du = dist[u];
    if (du == -count) continue;

    PQ.push(u);   // label u permanently  (u != t)
    dist[u] = -count;

    NT pu = du + pot[u];

    pot[u] = pu;

    edge e;
    forall_out_edges(e,u) 
    { 
      if (out_cap(e) < delta)  continue;

      node v = target(G,e,u);
      if (dist[v] == -count) continue; // v permanently labeled

      NT c = pu - pot[v] + cost[e];

      if (c >= dt) continue;
      if (excess[v] <= -delta) { t = v; dt = c; }

      if (c == du) 
      { *++top = v;
        dist[v] = c;
        pred[v] = e;
        mark[v] = count;
        if (v == t) goto finish;
        continue;
       }

      if ((mark[v] & 0x7fffffff) != count || c < dist[v])
      { PQ.insert(v,c); 
        dist[v] = c;
        pred[v] = e;
        mark[v] = count;
       }
     }


    forall_in_edges(e,u) 
    { 
      if (in_cap(e) < delta) continue;

      node v = source(G,e,u);

      if (dist[v] == -count) continue; // v permanently labeled

      NT c = pu - pot[v] - cost[e];

      if (c >= dt) continue;
      if (excess[v] <= -delta) { t = v; dt = c; }

      if (c == du) 
      { *++top = v;
        dist[v] = c;
        pred[v] = e;
        mark[v] = (count | 0x80000000);
        if (v == t) goto finish;
        continue;
       }

      if ((mark[v] & 0x7fffffff) != count || c < dist[v])
      { PQ.insert(v,c); 
        dist[v] = c;
        pred[v] = e;
        mark[v] = (count | 0x80000000);
       }
     }

   }

finish:

if (dt == 0) zero_count++;

  if (t && dt > 0)
  { for(node* p = PQ.Top(); p != PQ.Stop(); p++)  pot[*p] -= dt;

/*
    for(node* p = PQ.Top(); p != PQ.Stop(); p++) 
    { node u = *p;
      if (u == s) continue;
      edge e = pred[u];
      if (mark[u]&0x80000000)
         assert(cost[e] + pot[u] - pot[target(G,e,u)] == 0); 
      else
         assert(cost[e] + pot[source(G,e,u)] - pot[u] == 0); 
    }
*/

  }

  PQ.clear(); 

  return t;
}




public:

mcf_cap_scaling() 
{ T = 0; 
  write_log = false; 
} 

template <class cap_array, class cost_array, class flow_array>
bool run(const graph_t& G, cap_array& cap, 
                           const cost_array& cost,
                           excess_array& excess,
                           flow_array& flow, int f = 2)
{
  float t0 = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  node v;
  edge e;

  double U = 0;		// maximal excess or capacity

  forall_edges(e, G)  
  { flow[e] = 0;
    if (cap[e] > U) U = cap[e];
   }

  pot_array     pot(G,0);
  dist_array    dist(G,MAXINT);
  pred_array    pred(G,0);
  mark_array    mark(G,0);

  node*         V = new node[n];

  node_pq PQ(n+m);

  int count = 0;

  forall_nodes(v, G) 
  { NT ex = excess[v];
    if ( ex > U) U =  ex; 
    if (-ex > U) U = -ex;
   }

  int delta = 1;

/*
  while (U > delta) delta *= 2;
  delta /= f;
*/

  double U1 = (U*n)/m;
  while (U1 >= delta) delta *= 2;
  delta /= f;


/*
  //double U1 = (U*m)/(n*n);
  //while (U1 >= delta) delta *= 2;
  //delta *= f;

*/


  if (write_log)
  { cout << string("U = %.2f   delta = %d  f = %.2f", U, delta, U/delta);
    cout << endl;
   }


  while (delta > 0) // delta scaling phase
  { 
    float t = used_time();
    int augment_count = 0;
    int excess_count = 0;
    int correct_count = 0;
    int sum_len = 0;
    int sum_f = 0;

    zero_count = 0;


    // saturate all edges entering the delta residual network which have
    // a negative reduced edge cost. Then only the reverse edge (with positive
    // reduced edge cost) will stay in the residual network. 

    G.clear_in_edges();

    node u;
    forall_nodes(u,G)
    { 
      mark[u] = 0;

      NT pot_u = pot[u];

      edge e;
      forall_out_edges(e,u)
      { node v = target(G,e,u);

        NT x = in_cap(e);
        if (x >= delta && cost[e] + pot_u - pot[v] > 0)
        { excess[u] += x;
          excess[v] -= x;
          change_flow(e,-x);
          continue;
         }

        x = out_cap(e);
        if (x >= delta && cost[e] + pot_u - pot[v] < 0)
        { excess[u] -= x;
          excess[v] += x;
          change_flow(e,x);
         }

        if (in_cap(e) >= delta) G.push_in_edge(v,e);
       }
     }




    // As long as there is a node s with excess >= delta compute a minimum
    // cost augmenting path from s to a sink node t with excess <= -delta in 
    // the delta-residual network and augment the flow by at least delta units 
    // along P (if there are nodes with excess > delta and excess < -delta
    // respectively

    node s;
    forall_nodes(s,G)
    { 
      NT es = excess[s];
      if (es < delta) continue;
  
      excess_count++;
  
      while (es >= delta)
      { augment_count++;
        node t = 0;
/*
        t = DFS_IN_RESIDUAL(G,s,delta,cost,cap,flow, excess,pot,
                                             dist,pred,mark,V,++count);
*/
        if (t == nil)
          t = DIJKSTRA_IN_RESIDUAL(G,s,delta,cost,cap,flow, excess,pot,
                                             dist,pred,mark,PQ,V,++count);

        if (t == nil) break; 

  
        // compute maximal amount f of flow that can be pushed through P

        NT f = min(es,-excess[t]);
  
        // add f units of flow along the augmenting path 

        int len = 0;
        NT fmax = f;
        edge e;

        node v = t; 
        while (v != s)
        { e = pred[v];
          len++;
          NT rc;
          if (mark[v]&0x80000000)
            { rc =  in_cap(e);
              change_flow(e,-f);
              v = source(G,e,v);
             }
          else
            { rc = out_cap(e);
              change_flow(e,f);
              if (!G.test_in_edge(e)) G.push_in_edge(v,e);
              v = target(G,e,v);
             }
           if (rc < fmax) fmax = rc;
         }

        if (fmax < f) // correct flow
        { correct_count++;
          NT d = f - fmax;
          f = fmax;
          node v = t;  
          while (v != s)
          { e = pred[v];
            if (mark[v]&0x80000000)
              { change_flow(e,d);
                v = source(G,e,v);
               }
            else
              { change_flow(e,-d);
                v = target(G,e,v);
               }
          }
        }

        sum_len += len;
        sum_f   += f;
  
        es -= f;
        excess[t] += f;
      }

      excess[s] = es;
  
    } // end of delta-phase
  
  if (write_log)
  { cout << string("%5d ", delta);
    cout << string("E = %4d   ", excess_count);
    cout << string("D = %5d   ", augment_count);
    cout << string("C = %4d   ", correct_count);
    cout << string("L = %7.2f ", float(sum_len)/augment_count);
    cout << string("A = %8.2f ", float(sum_f)/augment_count);
    cout << string("%.2f sec",used_time(t)) << endl;
  }
  
    delta /= 2;

  } // end of scaling


  bool feasible = true;

  forall_nodes(v,G) 
    if (excess[v]  != 0) feasible = false;

  total_c = 0;
  forall_edges(e,G) 
  { total_c += double(cost[e])*flow[e];
    cap[e] = total_cap(e);
   }

  delete[] V;

  T = used_time(t0);

  return feasible;
}


template <class lcap_array, class ucap_array, class cost_array, 
          class supply_array, class flow_array>
bool run(const graph_t& G, const lcap_array& lcap,
                           const ucap_array& ucap,
                           const cost_array& cost,
                           const supply_array& supply, 
                           flow_array& flow, int f = 2)
{ 

  int lc_count = 0;             // number of low-cap edges

  ucap_array& cap = (ucap_array&)ucap;

  excess_array  excess(G,0);
  node v;
  forall_nodes(v, G) excess[v] = supply[v];

  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v)
    { flow[e] = 0;
      NT lc = lcap[e];
      if (lc != 0) // remove lower capacity bound
      {	lc_count++;
        node i = v;
        node j = target(G,e,v);
        excess[i] -= lc;
        excess[j] += lc;
        cap[e] -= lc;
       }
     }
   }

  bool result = run(G,cap,cost,excess,flow,f);

  if (lc_count > 0) // handle lower capacities
  { edge e;
    forall_edges(e,G) 
    { NT lc = lcap[e];
      flow[e] += lc;
      cap[e] += lc;
     }
   }

  return result;
}



string name()       const { return "mincostflow capacity scaling"; }
float  cpu_time()   const { return T; }
double total_cost() const { return total_c; }
double result()     const { return total_c; }

void set_write_log(bool b) { write_log = b; }  

void statistics(ostream& out) { out << endl; }

};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520220
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif
