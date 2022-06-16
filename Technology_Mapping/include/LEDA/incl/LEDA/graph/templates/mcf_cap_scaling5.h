/******************************************************************************
+
+  LEDA 5.2  
+
+
+  mcf_cap_scaling6.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.11 $  $Date: 2007/02/28 07:44:14 $

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520230
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/templates/max_flow7.h>
#include <LEDA/graph/node_pq22.h>
#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>
#include <LEDA/core/b_stack.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/system/assert.h>



LEDA_BEGIN_NAMESPACE

/*
#define out_cap(e)       cap[e]
#define in_cap(e)        flow[e]
#define total_cap(e)     (cap[e]+flow[e])
#define change_flow(e,x) (flow[e] += x, cap[e] -= x)
*/

/*
#define out_cap(e)       (cap[e]-flow[e])
#define in_cap(e)        flow[e]
#define total_cap(e)     cap[e]
#define change_flow(e,x) (flow[e] += x)
*/

/*
#define rcost(e,v,w)        ((pot[w]-pot[v])+cost[e])
*/

#define rcost(e,v,w)        ((pot[v]-pot[w])+cost[e])

/*
#define push_flow(e,x)      change_flow(e,(e.out() ? x : -x))
*/

#define residual_cap(e)     (e.out() ? out_cap(e) : in_cap(e))
#define reduced_cost(e,v,w) (e.out() ? rcost(e,v,w) : -rcost(e,w,v))



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
  typedef typename graph_t::incident_edge incident_edge;
  typedef node_pq_t node_pq;
  
  float T;
  double total_c;
  bool write_log;


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
  NT dmax = 0;

  node* top = V;
  V[0] = s;

  dist[s] = 0;
  mark[s] = count;
  pred[s] = nil;

  int n = G.number_of_nodes();
  b_stack<node> labeled(n);

  while (top >= V || !PQ.empty())
  { 
    node u = (top >= V) ? *top-- : PQ.del_min(dist);

    if (mark[u] == count+1) continue;

    NT  du = dist[u];
    dmax = du;
    if (du == dt) break;

    //PQ.push(u);   // label u permanently  (u != t)
    labeled.push(u);
    mark[u] = count+1;

    incident_edge e;
    forall_incident_edges(e,u) 
    { if (residual_cap(e) < delta)  continue;

      node v = G.opposite(e,u);

      if (mark[v] == count+1) continue; // v permanently labeled

      NT c = du + reduced_cost(e,u,v);
      if (c >= dt || (mark[v] == count && c >= dist[v])) continue;
      if (excess[v] <= -delta) { t = v; dt = c; }

      dist[v] = c;
      pred[v] = e;
      mark[v] = count;

      if (c == du) 
        *++top = v;
      else
        PQ.insert(v,c,dist); 

     }
   }

  if (dmax > 0) {
    while (!labeled.empty())
    { node v = labeled.pop();
      pot[v] += (dist[v]-dmax);
     }
/*
    for(node* p = PQ.Top(); p != PQ.Stop(); p++)  
    { node v = *p;
      pot[v] += (dist[v]-dmax);
     }
*/
  }

  PQ.clear(); 

  return t;
}



/*
template<class cost_array, class cap_array, class flow_array>
bool DIJKSTRA_IN_RESIDUAL1(const graph_t& G,int delta, 
                             const cost_array&   cost,
                             const cap_array&    cap,
                             const flow_array&   flow,
                             const excess_array& excess,
                             pot_array&  pot,
                             dist_array& dist,
                             mark_array& mark,
                             node_pq&  PQ, 
                             node* V,
                             int count)
{
  int t_reached = 0;
  NT  dmax = 0;

  node v;
  forall_nodes(v,G)
  { if (excess[v] < delta) continue;
    PQ.insert(v,0);
    dist[v] = 0;
    mark[v] = count;
  }

  while (!PQ.empty())
  { node u = PQ.del_min(dist);
    if (mark[u] == count+1) continue;

    NT  du = dist[u];
    dmax = du;
    mark[u] = count+1; // label u permanently
    PQ.push(u);   

    if (excess[u] <= -delta) t_reached++;

    incident_edge e;
    forall_incident_edges(e,u) 
    { if (residual_cap(e) < delta)  continue;

      node v = G.opposite(e,u);
      if (mark[v] == count+1) continue; // v permanently labeled

      NT c = du + reduced_cost(e,u,v);

      if (mark[v] < count || c < dist[v])
      { dist[v] = c;
        mark[v] = count;
        PQ.insert(v,c); 
       }
     }
   }


  if (t_reached > 0) {
    for(node* p = PQ.Top(); p != PQ.Stop(); p++)  
    { node v = *p;
      pot[v] += (dist[v]-dmax);
     }
  }

  PQ.clear(); 

  return t_reached;
}
*/



template<class cost_array, class cap_array, class flow_array>
int MAXFLOW_IN_ADMISSIBLE(const graph_t& G, int delta,
                                               const cost_array& cost,
                                               cap_array& cap,
                                               const pot_array& pot,
                                               excess_array& excess,
                                               pred_array& pred,
                                               flow_array& flow)
{
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  typedef max_flow<int>::node_data nd_type;
  typedef max_flow<int>::edge_data ed_type;

  const int ns = sizeof(nd_type)/4;
  const int es = sizeof(ed_type)/4;

  typedef static_fgraph<opposite_graph,data_slots<ns>,
                                       data_slots<es+4> > a_graph;
  typedef a_graph::node a_node;
  typedef a_graph::edge a_edge;

  typedef node_slot1<nd_type,a_graph,0> node_data_array;
  typedef edge_slot1<ed_type,a_graph,0> edge_data_array;


  a_graph A;

  edge_slot1<int,a_graph,es+0>  a_cap(A);
  edge_slot1<int,a_graph,es+1>  a_flow(A);
  edge_slot1<edge,a_graph,es+2> e_in_G(A);
  edge_slot1<bool,a_graph,es+3> out(A);

  node_array<a_node,graph_t> v_in_A(G,0);

  A.start_construction(n+2,2*m);

  node u;
  forall_nodes(u,G) v_in_A[u] = A.new_node();

  a_node s = A.new_node();
  a_node t = A.new_node();

  forall_nodes(u,G)
  { a_node au = v_in_A[u]; 
    edge e;
    forall_out_edges(e,u)
    { node v = G.opposite(e,u);
      NT rc = cost[e] + pot[u] - pot[v];
      if (rc != 0) continue;
      if (out_cap(e) >= delta) 
      { a_edge a = A.new_edge(au,v_in_A[v]);
        a_cap[a] = out_cap(e);
        e_in_G[a] = e;
        out[a] = true;
       }
     }
    forall_in_edges(e,u)
    { node v = G.opposite(e,u);
      NT rc = -(cost[e] + pot[v] - pot[u]);
      if (rc != 0) continue;
      if (in_cap(e) >= delta) 
      { a_edge a = A.new_edge(au,v_in_A[v]);
        a_cap[a] = in_cap(e);
        e_in_G[a] = e;
        out[a] = false;
       }
     }
    NT b = excess[u];
    if ( b <= -delta) 
    { a_edge a = A.new_edge(au,t);
      a_cap[a] = -b;
      e_in_G[a] = (edge)u;
     }
  }

  node v;
  forall_nodes(v, G) 
  { NT b = excess[v];
    if ( b >=  delta)
    { a_edge a = A.new_edge(s,v_in_A[v]);
      a_cap[a] = b;
      e_in_G[a] = (edge)v;
     }
  }

  A.finish_construction();

  max_flow<int,a_graph,node_data_array,edge_data_array> mf;

  int fmax = mf.run(A,s,t,a_cap,a_flow);

  a_node au; 
  forall_nodes(au,A)
  { if (au == s) continue;
    a_edge x;
    forall_out_edges(x,au)
    { edge e = e_in_G[x];
      a_node av = A.opposite(x,au);
      if (av == t) continue;
      change_flow(e, out[x] ? a_flow[x] : -a_flow[x]);
     }
   }

  a_edge x;
  forall_out_edges(x,s)
  { node v = (node)e_in_G[x];
    excess[v] -= a_flow[x];
   }

  forall_in_edges(x,t)
  { node v = (node)e_in_G[x];
    excess[v] += a_flow[x];
   }

  G.clear_in_edges();
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v)
    { if (in_cap(e) == 0) continue;
      node w = G.opposite(e,v);
      G.push_in_edge(w,e);
     }
  }

  return fmax; 
}




template<class cap_array, class cost_array, class flow_array>

bool check_opt(const graph_t& G, const cap_array&   cap,
                                 const cost_array&   cost,
                                 const flow_array&   flow,
                                 const pot_array&    pot,
                                 int delta)
{
  int err_count = 0;
  node u;
  forall_nodes(u,G)
  { edge e;
    forall_out_edges(e,u)
    { assert(flow[e] >= 0);
      assert(flow[e] <= cap[e]);
      node v = G.opposite(e,u);
      //NT rc = cost[e] + pot[u] - pot[v];
      NT rc = rcost(e,u,v);
      if (out_cap(e) >= delta && rc < 0) 
      { cout << G.index(u) << " ---> " << G.index(v) <<endl;
        err_count++;
       }
      if (in_cap(e) >= delta && rc > 0) 
      { cout << G.index(u) << " ---> " << G.index(v) <<endl;
        err_count++;
       }
    }
  }

if (err_count > 0) cout << "errors = " << err_count <<endl;
 return err_count == 0;
}





public:

mcf_cap_scaling() 
{ T = 0; 
  write_log = false; 
} 

template <class cap_array, class cost_array, class flow_array>
bool run(const graph_t& G, cap_array& cap, const cost_array& cost,
                                           excess_array& excess,
                                           flow_array& flow, int f = 2)
{
  float t0 = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

 

  int total_dijkstra = 0;

  node v;
  edge e;

  double U = 0;		// maximal excess or capacity

  node_array<int,graph_t> supply(G);

  forall_nodes(v,G) supply[v] = excess[v];

  forall_edges(e, G)  
  { flow[e] = 0;
    if (cap[e] > U) U = cap[e];
   }

  pot_array     pot(G,0);
  dist_array    dist(G,MAXINT);
  pred_array    pred(G,0);
  mark_array    mark(G,0);

  node*         V = new node[n];

  node_pq PQ(4*n);

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
    cout << endl;
    cout << " delta  excess dijkstra augment correct  p_length    volume   time" << endl;
    cout << "---------------------------------------------------------" << endl;
   }

  while (delta > 0) // delta scaling phase
  { 
    float tt = used_time();
    int excess_count = 0;
    int correct_count = 0;
    int dijkstra_count = 0;
    int augment_count = 0;
    int sum_len = 0;
    int sum_f = 0;


    // saturate all edges entering the delta residual network which have
    // a negative reduced edge cost. Then only the reverse edge (with positive
    // reduced edge cost) will stay in the residual network. 

    //G.hide_all_in_edges();
    G.clear_in_edges();

    node u;
    forall_nodes(u,G)
    { 
      edge e;
      forall_out_edges(e,u)
      { node v = G.opposite(e,u);

        NT x = in_cap(e);
        if (x >= delta && rcost(e,u,v) > 0)
        { excess[u] += x;
          excess[v] -= x;
          change_flow(e,-x);
          continue;
         }

        x = out_cap(e);
        if (x >= delta && rcost(e,u,v) < 0)
        { excess[u] -= x;
          excess[v] += x;
          change_flow(e,x);
         }

        //if (in_cap(e) >= delta) G.restore_in_edge(e,v);
        if (in_cap(e) >= delta) G.push_in_edge(v,e);
       }
     }


  float t_mf = used_time();

/*
  int m_count = 0;
  int i=0;
  double f = 0;

  while (i < m_count)
  { count += 2;
    if (!DIJKSTRA_IN_RESIDUAL1(G,delta,cost,cap,flow,excess,pot,dist,mark,PQ,V,count)) break;
    
    f += MAXFLOW_IN_ADMISSIBLE(G,delta,cost,cap,pot,excess,pred,flow);
    i++;
   }
*/

   //cout << string("maxflow: %3d    f = %12.0f",i,f) << endl;

t_mf = used_time(t_mf);


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
      {
        dijkstra_count++;
        count += 2;
        node  t = DIJKSTRA_IN_RESIDUAL(G,s,delta,cost,cap,flow, excess,pot,
                                             dist,pred,mark,PQ,V,count);

        if (t == nil) break; 

        augment_count++;
  
        // compute maximal amount f of flow that can be pushed through P

        NT f = min(es,-excess[t]);
  
        // add f units of flow along the augmenting path 

        int len = 0;
        NT fmax = f;

        assert(pred[t]);

        for(node v = t; v != s; )
        { incident_edge e = pred[v];
          NT rc = residual_cap(e);
          push_flow(e,f);
          //if (G.hidden_in_edge(e)) G.restore_in_edge(e,v);
          if (!G.test_in_edge(e)) G.push_in_edge(v,e);
          if (rc < fmax) fmax = rc;
          len++;
          v = G.opposite(e,v);
         }

        if (fmax < f) // correct flow
        { correct_count++;
          NT d = f - fmax;
          f = fmax;
          for(node v = t; v != s; )
          { incident_edge e = pred[v];
            push_flow(e,-d);
            v = G.opposite(e,v);
          }
        }

        sum_len += len;
        sum_f   += f;
  
        es -= f;
        excess[t] += f;
      }

      excess[s] = es;

  
    } // end of delta-phase


  total_dijkstra += dijkstra_count;
  
  if (write_log)
  { cout << string("%6d  %6d  %6d  %6d  %6d  %8.2f  %8.2f   %.2f",
            delta, excess_count, dijkstra_count, augment_count, correct_count, 
            float(sum_len)/augment_count, float(sum_f)/augment_count, 
            used_time(tt)) << endl;
  }

  
    delta /= 2;

  } // end of scaling

  if (write_log) 
  { cout << endl;
    cout << "dijkstra: " << total_dijkstra <<endl;
    cout << endl;
   }

  bool feasible = true;

  forall_nodes(v,G) 
    if (excess[v]  != 0) feasible = false;

  total_c = 0;
  forall_edges(e,G) 
  { total_c += double(cost[e])*flow[e];
    cap[e] = total_cap(e);
   }

  delete[] V;

  G.rebuild_in_edges();

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
        node j = G.opposite(e,v);
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



template<class lcap_array, class ucap_array, class cost_array, 
                                             class supply_array,
                                             class flow_array>

bool check(const graph_t& G, const lcap_array& lcap, const ucap_array&   ucap,
                                                     const cost_array&   cost,
                                                     const supply_array& supply,
                                                     const flow_array&   flow,
                                                     string& msg)
{
 
  msg = "";

  //check feasibility

  int count1 = 0;
  int count2 = 0;

  edge e;
  forall_edges(e,G)
    if (flow[e] < lcap[e] || flow[e] > ucap[e]) count1++;

  if (count1 > 0)
         msg = string("illegal flow value for %d edge(s)",count1);
  
  node v;
  forall_nodes(v,G)
  { edge e;
    double ev = double(supply[v]);
    forall_out_edges(e,v) ev -= flow[e];
    forall_in_edges(e,v)  ev += flow[e];
    if (ev != 0) count2++;
   }

  if (count2 > 0)
  { if (msg != "") msg += " + ";
     msg = string("non-zero excess at %d node(s)",count2);
   }

  if (count1 || count2) return false;


  // check optimality (negative cycle condition)
  // bellman ford:

  int n = G.number_of_nodes();

  b_queue<node> Q(n);
  node Q_last = 0;

  node_array<node,graph_t> succ(G,0); 
  // if v in Q then succ[v] = v else succ[v] = 0

  node_array<int,graph_t>  dist(G,0);

  forall_nodes(v,G) 
  { dist[v] = 0;
    Q.append(v);
    succ[v] = v;
    Q_last = v;
   }

  node phase_last = Q_last;
  int  phase_count = 0;

  while (phase_count < n && !Q.empty())
  { 
    node u = Q.pop();
    succ[u] = 0;

    int du = dist[u];

    edge e;
    forall_out_edges(e,u) 
    { if (flow[e] == ucap[e]) continue;
      node v = G.opposite(e,u);    
      int d = du + cost[e];
      if (d < dist[v])
      { dist[v] = d;
        if (succ[v] == 0) { Q.append(v); succ[v] = v; Q_last = v; }
       }
     } 

    forall_in_edges(e,u) 
    { if (flow[e] == lcap[e]) continue;
      node v = G.opposite(e,u);    
      int d = du - cost[e];
      if (d < dist[v])
      { dist[v] = d;
        if (succ[v] == 0) { Q.append(v); succ[v] = v; Q_last = v; }
       }
     } 

    if (u == phase_last && !Q.empty()) 
    { phase_count++;
      phase_last = Q_last;
     }
  }

  if (!Q.empty())
  { if (msg != "") msg += " + ";
    msg += "negative cycle in residual network."; 
   }

  return Q.empty(); // no negative cycle
}




string name()       const { return "mincostflow capacity scaling"; }
float  cpu_time()   const { return T; }
double total_cost() const { return total_c; }
double result()     const { return total_c; }

void set_write_log(bool b) { write_log = b; }  

void statistics(ostream& out) { out << endl; }

};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520230
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif
