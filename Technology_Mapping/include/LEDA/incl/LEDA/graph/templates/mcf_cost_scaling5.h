#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520223
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/internal/std/math.h>
#include <LEDA/core/b_stack.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/member_array.h>
#include <LEDA/graph/node_pq.h>

#include <LEDA/system/assert.h>


/*
#define out_cap(e)       cap[e]
#define in_cap(e)        flow[e]
#define capacity(e)      (cap[e]+flow[e])
#define change_flow(e,x) { flow[e] += x; cap[e] -= x; }
*/

#define out_cap(e)       (cap[e]-flow[e])
#define in_cap(e)        flow[e]
#define capacity(e)      cap[e]
#define change_flow(e,x) { flow[e] += x; }

#define rcost(e,v,w)        ((pot[w]-pot[v])+n_plus_1*cost[e])
#define residual_cap(e)     (e.out() ? out_cap(e) : in_cap(e))
#define reduced_cost(e,v,w) (e.out() ? rcost(e,v,w) : -rcost(e,w,v))
#define push_flow(e,x)      change_flow(e,(e.out() ? x : -x))




LEDA_BEGIN_NAMESPACE

/*
class counter  {
public:
  counter(int) {}
  void operator++(int) {}
  operator int() { return 0; }
};
*/

typedef int counter;




template<class rcost_t, class graph_t, class cap_array, class cost_array,
                        class flow_array, class pot_array>
class af_filter {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

const graph_t&    G;
const cap_array&  cap;
const flow_array& flow;
const cost_array& cost;
const pot_array&  pot;
double eps;
double af_bound;
int n_plus_1;

public:

af_filter(const graph_t& g, const cap_array& ca, const cost_array& c, 
                                                 const flow_array& f, 
                                                 const pot_array& p, 
                                                 rcost_t e,
                                                 double a) 
: G(g), cap(ca), flow(f), cost(c), pot(p), eps(e), af_bound(a*e)
{ n_plus_1 = g.number_of_nodes() + 1; }

bool operator()(edge e, node u)
{ node v = G.opposite(e,u);
  rcost_t rc = rcost(e,u,v);
  return (rc > af_bound && in_cap(e) == 0 || -rc > af_bound && out_cap(e) == 0);
 }

};



template<class NT, class excess_t, class rcost_t>
struct mcf_node_data {
  excess_t excess; 
  rcost_t  pot; 
  double   dist;
  GenPtr   curr; // incident_edge
};


template<class NT, class excess_t, class rcost_t>
struct mcf_edge_data {
};



template <class NT, class excess_t=double, class rcost_t=double, 
                                           class graph_t=void, 
                                           class nd_array=void, 
                                           class ed_array=void>
class mcf_cost_scaling;


template<class NT, class excess_t, class rcost_t>
class mcf_cost_scaling<NT,excess_t,rcost_t,void,void,void>
{ public:
  typedef mcf_node_data<NT,excess_t,rcost_t> node_data;
  typedef mcf_edge_data<NT,excess_t,rcost_t> edge_data;
};


template<class NT>
class mcf_cost_scaling<NT,double,double,void,void,void>
{ public:
  typedef mcf_node_data<NT,double,double> node_data;
  typedef mcf_edge_data<NT,double,double> edge_data;
};



template <class NT, class excess_t, class rcost_t, 
                    class graph_t, class nd_array, class ed_array>

class mcf_cost_scaling 
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  typedef typename graph_t::incident_edge incident_edge;

  typedef mcf_node_data<NT,excess_t,rcost_t> node_data;
  typedef mcf_edge_data<NT,excess_t,rcost_t> edge_data;


  typedef member_array<rcost_t, nd_array,node_data>      pot_array;
  typedef member_array<excess_t,nd_array,node_data>      excess_array;
  typedef member_array<double,  nd_array,node_data>      dist_array;
  typedef member_array<GenPtr,  nd_array,node_data,incident_edge> current_array;


  int    alpha;
  double af_start_factor;
  double af_factor;
  double af_update;
  double eps_f;
  int    update_limit;

  int n_plus_1;

  int inf;
  int num_nodes;
  int num_edges;
  int num_discharges;
  int num_pushes;
  int num_final_pushes;
  int num_relabels;
  int num_updates;
  int num_arcfixes;
  int num_restarts;
  int num_refines;

  double max_level;
 
  bool write_log;

  float T;
  float T_update;


void error_handler(int,string msg)
{ cerr << "mcf_cost_scaling: " + msg << endl;
  exit(1);
 }


rcost_t scale_down(double eps, int a)
{ assert(eps > 1);
  int e = int(0.5 + eps/a);
  return (e < 1) ? 1 : e;
}


template<class cost_array, class flow_array>
rcost_t primal_objective(const graph_t& G, const cost_array& cost, 
                                          const flow_array& flow)
{ rcost_t Z = 0;
  edge e;
  forall_edges(e,G) Z += rcost_t(cost[e])*flow[e];
  forall_hidden_edges(e,G) Z += rcost_t(cost[e])*flow[e];
  return Z;
}




template<class pot_array, class cap_array, class cost_array, class supply_array>
rcost_t dual_objective(const graph_t& G, const pot_array& pot,
                                         const cap_array& cap,
                                         const cost_array& cost,
                                         const supply_array& supply,
                                         rcost_t pot_f = 1)
{ rcost_t W = 0;

  node v;
  forall_nodes(v,G) 
  { rcost_t pv = pot[v]/pot_f;
    W += pv*supply[v];

    edge e;
    forall_out_edges(e,v) 
    { node w = G.opposite(e,v);
      rcost_t pw = pot[w]/pot_f;
      rcost_t rc = pw - pv + cost[e];
      if (rc < 0) W += rc*cap[e];
    }
/*
    forall_hidden_out_edges(e,v,G)
    { node w = G.opposite(e,v);
      rcost_t pw = pot[w]/pot_f;
      rcost_t rc = pw - pv + cost[e];
      if (rc < 0) W += rc*cap[e];
    }
*/
  }

   edge e;
   forall_hidden_edges(e,G)
   { node v = G.hidden_source(e);
     node w = G.opposite(e,v);
     rcost_t rc = (pot[w] - pot[v])/pot_f + cost[e];
     if (rc < 0) W += rc*cap[e];
   }

  return W;
}



template<class cap_array,class cost_array, class flow_array, class pot_array>
bool check_eps_optimality(const graph_t& G, const cap_array& cap,
                                            const cost_array& cost,
                                            const flow_array& flow,
                                            const pot_array& pot,
                                            double eps, 
                                            string msg="")
{ rcost_t rc_min = 0;

  node v;
  forall_nodes(v,G)
  { incident_edge e;
    forall_incident_edges(e,v)
    { if (residual_cap(e) == 0) continue;
      node w = G.opposite(e,v);
      rcost_t rc = reduced_cost(e,v,w);
      if (rc < rc_min) rc_min = rc;
     }
   }

  if (write_log)
    cout << string("check_eps_opt: rc_min = %f", rc_min) << endl;

  if (rc_min >= -eps) return true;
   
  if (msg != "") 
  { cerr << string("check_eps_opt: rc_min = %.2f  eps = %.2f", rc_min,eps);
    cerr << endl;
    error_handler(1,"check_eps_opt: " + msg);
   }
     
  return false;
}


excess_t total_excess(const graph_t& G, const excess_array&  excess)
{ excess_t sum = 0;
  node v;
  forall_nodes(v,G) sum += excess[v];
  return sum;
}



template<class cap_array,class cost_array, class flow_array>
bool check_current_edge(const graph_t& G, const cap_array&  cap,
                                          const cost_array& cost,
                                          const flow_array& flow,
                                          const pot_array& pot,
                                          const current_array& current)
{ int count = 0;
  node v;
  forall_nodes(v,G)
  { incident_edge e_cur = current[v];
    incident_edge e;
    forall_incident_edges(e,v)
    { if (e == e_cur) break;
      NT x = residual_cap(e);
      if (x == 0) continue;
      node w = G.opposite(e,v);
      rcost_t rc = reduced_cost(e,v,w);
      if (rc < 0) count++;
     }
    assert(e == e_cur);
   }
  return count == 0;
}


template<class cap_array,class cost_array, class flow_array>
rcost_t check_fixed_arcs(const graph_t& G, const cap_array& cap,
                                            const cost_array& cost,
                                            const flow_array& flow,
                                            const pot_array& pot)
{ 
  rcost_t min_eps = 0;

/*
  node v;
  forall_nodes(v,G)
  { edge e;
    forall_hidden_out_edges(e,v,G)
    { node w = G.opposite(e,v);
      rcost_t rc = rcost(e,v,w);
      if (rc < -min_eps && out_cap(e) > 0) min_eps = -rc;
      if (rc >  min_eps && in_cap(e) > 0)  min_eps =  rc;
    }
  }
*/

  edge e;
  forall_hidden_edges(e,G)
  { node v = G.hidden_source(e);
    node w = G.opposite(e,v);
    rcost_t rc = rcost(e,v,w);
    if (rc < -min_eps && out_cap(e) > 0) min_eps = -rc;
    if (rc >  min_eps && in_cap(e) > 0)  min_eps =  rc;
  }

  return min_eps;
}


template<class cap_array, class cost_array, class flow_array, 
                                            class dist_array, 
                                            class succ_array>
bool bellman_ford_in_residual(const graph_t& G, const cap_array&  cap,
                                                const cost_array& cost,
                                                const flow_array& flow,
                                                dist_array& dist,
                                                succ_array& succ)
{ float t = used_time();

  int n = G.number_of_nodes();

  if (n == 0) return true;

  node Q_first = 0;
  node Q_last = 0;

  node v;
  forall_nodes(v,G) 
  { dist[v] = 0;
    if (Q_last == 0) 
      Q_first = v;
    else
      succ[Q_last] = v;
    Q_last = v;
   }

  succ[Q_last] = Q_last;

  node phase_last = Q_last;
  int  phase_count = 0;

  while (phase_count < n)
  { node u = Q_first;
    Q_first = succ[u];
    succ[u] = u;        // mark: not in Q

    int du = dist[u];

    incident_edge e;
    forall_incident_edges(e,u) 
    { if (residual_cap(e)== 0) continue;
      node v = G.opposite(e,u);    
      int d = du + (e.out() ? cost[e] : -cost[e]);
      if (d < dist[v])
      { dist[v] = d;
        if (succ[v] != v) continue;
        if (Q_first == 0)
           Q_first = v;
        else
           succ[Q_last] = v;
        succ[v] = 0; 
        Q_last = v; 
       }
     } 

    if (Q_first == 0) break;

    if (u == phase_last) 
    { phase_count++;
      phase_last = Q_last;
     }
  }

  if (write_log)
     cout << string("bellman_ford:  phases = %d  time: %.2f sec.",
                     phase_count,used_time(t)) << endl;

  return Q_first == 0; // no negative cycles
}


template<class cap_array, class cost_array, class flow_array, class node_pq>
void compute_optimal_potential(const graph_t& G, const cap_array&  cap,
                                                 const cost_array& cost,
                                                 const flow_array& flow,
                                                 pot_array& pot,
                                                 dist_array& dist,
                                                 node_pq& PQ)

{ // precondition: (flow, pot) must be 1-optimal

  const int eps = 1;

  node_array<int,graph_t> length(G);

  PQ.clear();
        
  node v;
  forall_nodes(v,G)
  { dist[v] = pot[v];
    length[v] = 0;
    PQ.insert(v, dist[v]);
   }
        
        
  while (!PQ.empty())
  { rcost_t dv = 0;
    node v = PQ.del_min(dv);

    incident_edge e;
    forall_incident_edges(e,v)
    { if (residual_cap(e) == 0) continue;
      node w = G.opposite(e,v);
      rcost_t rc = reduced_cost(e,v,w);
      assert(rc >= -1);
      rcost_t c = dv + rc + eps;
      if (c < dist[w])
      { dist[w] = c;
        PQ.decrease_p(w,c);
        length[w] = length[v] + 1;
       }
     }
   }

  forall_nodes(v,G)
    pot[v] += length[v] * eps - dist[v];
}



template<class cap_array, class flow_array, class cost_array>
bool topsort_admissible(const graph_t& G, const cap_array& cap,
                                          const cost_array& cost,
                                          const flow_array& flow,
                                          const pot_array& pot,
                                          dist_array& outdeg,
                                          node* Q)
{
  // sort admissible network topologically

  int n = G.number_of_nodes();
  node* q = Q+n;

  node u;
  forall_nodes(u,G)
  { int deg = 0;
    incident_edge e;
    forall_incident_edges(e,u)
    { if (residual_cap(e) == 0) continue;
      node v = G.opposite(e,u);
      if (reduced_cost(e,u,v) < 0) deg++;
     }
    if (deg == 0) *--q = u;
    outdeg[u] = deg;
   }

  for(node* p = Q+n-1; p >= q; p--)
  { node u = *p;
    incident_edge e;
    forall_incident_edges(e,u)
    { incident_edge r = e.reverse();
      if (residual_cap(r) == 0) continue;
      node v = G.opposite(r,u);
      if (reduced_cost(r,v,u) >= 0) continue;
      if (--outdeg[v] == 0) *--q = v;
     }
   }

  return q == Q; // acyclic
}



template<class cap_array, class cost_array, class flow_array>
bool topsort_dfs(const graph_t& G, const cap_array& cap, const cost_array& cost,
                                                        flow_array& flow,
                                                        const pot_array& pot,
                                                        current_array& current,
                                                        node* Q)
{ int n = G.number_of_nodes();

  int cycle_count = 0;

/*
  float tt = used_time();
*/

  node* t_ptr = Q+n;
  node* s_ptr = Q-1;

  //b_stack<node> S(n);


  incident_edge sentinel(G.stop_edge(),true);

  node v;
  forall_nodes(v,G) current[v] = 0;

  node u;
  forall_nodes(u,G)
  { 
    if (current[u] != 0) continue; // visited before

    //S.push(u);
    *++s_ptr = u;
    current[u] = G.first_incident_edge(u);

    //while (!S.empty())
    while (s_ptr >= Q)
    { 
      //node v = S.top();
      node v = *s_ptr;

      incident_edge e = current[v];

      while (e)
      { node w = G.opposite(e,v);
        if (current[w] != sentinel && // skip edges to completed nodes
            residual_cap(e) > 0 && reduced_cost(e,v,w) < 0) break;
        e = G.next_incident_edge(e,v);
       }


      if (e == 0)              // all edges scanned, finish v
      { *--t_ptr = v;          // add to front of topsort list
        current[v] = sentinel; // mark  completed
        //S.pop();
        --s_ptr;
/* 
        // not necessary since current edge points to the node which
        // was just completed above
        if (!S.empty())
        { node u = S.top();
          current[u] = G.next_incident_edge(current[u],u);
         }
*/
        continue;
       }

      current[v] = e;

      node w  = G.opposite(e,v);

      if (current[w] == 0) // w not visited: tree edge
      { //S.push(w);
        *++s_ptr = w;
        current[w] = G.first_incident_edge(w);
       }
      else
      { // w visited but not completed: backward edge

        cycle_count++;

        // compute minimum residual capacity on cycle (min_rc)
        // and augment flow by min_rc

        NT   min_rc = residual_cap(e);
        node min_v  = v;
        int  count = 1;
        node u = w;

        do { incident_edge e = current[u];
             NT rc = residual_cap(e);
             if (rc < min_rc) { min_rc = rc; min_v = u; }
             u = G.opposite(e,u);
             count++;
           } while (u != v);
       
        do { incident_edge e = current[u];
             push_flow(e,min_rc);
             u = G.opposite(e,u);
             if (G.hidden_in_edge(e)) G.restore_in_edge(e,u);
           } while (u != v);

        // continue dfs at min_v by marking all nodes 
        // from min_v to v as unvisited 

        while (v != min_v)  
        { current[v] = 0;   // mark unvisited
          //S.pop();
          //v = S.top();
          v = *--s_ptr;
         }

      } // backward edge

    } // while stack not empty
  }

 assert(t_ptr == Q);

 if (write_log && cycle_count > 0) 
      cout << "cycle_count = " << cycle_count << endl;

/*
if (write_log)
{ cout << string("topsort: %.2f sec  cycles = %d", used_time(tt),cycle_count);
  cout  << endl;
 }
*/

 return cycle_count == 0;
}





template<class cap_array, class cost_array, class flow_array, class node_pq>
bool price_refinement(const graph_t& G, rcost_t eps, const cap_array& cap,
                                                     const cost_array& cost,
                                                     flow_array& flow,
                                                     pot_array&  pot, 
                                                     dist_array& dist,
                                                     current_array& current,
                                                     node_pq& PQ)
{
  float tt = used_time();

  if (write_log) 
     cout << string("price refine(eps = %.2f)",eps) << endl;

  bool result = false;

  int n = G.number_of_nodes();
  node* Q = new node[n];

  //while (topsort_admissible(G,cap,cost,flow,pot,dist,Q))
  while (topsort_dfs(G,cap,cost,flow,pot,current,Q))
  {
    // now Q contains all nodes in topological order with respect to
    // the admissible network. We compute shortest admissible paths 
    // using this topological ordering.

    node v;
    forall_nodes(v,G) dist[v] = 0;

    PQ.clear();

    int non_zero = 0;

    for(int i=0; i<n; i++)
    { 
      node u = Q[i];
      double du = dist[u];
      assert(du <= 0);
      PQ.insert(u,du);

      if (du < 0) non_zero++;

      incident_edge e;
      forall_incident_edges(e,u)
      { if (residual_cap(e) == 0) continue;
        node v = G.opposite(e,u);
        rcost_t rc = reduced_cost(e,u,v);
        if (rc < 0)
        { double d = du + rc + eps; 
          if (d < dist[v]) dist[v] = d;
         }
       }
     }


    if (non_zero == 0) // eps-optimal
    { result = true;
      break;
     }


    while (!PQ.empty())
    { double dv = 0;
      node v = PQ.del_min(dv);

      incident_edge e;
      forall_incident_edges(e,v)
      { if (residual_cap(e) == 0) continue;

        node w = G.opposite(e,v); 
        double dw = dist[w];
        if (dv >= dw) continue;

        rcost_t rc = reduced_cost(e,v,w);
        double d = dv;
        if (rc >= 0) d += rc+eps;
        if (d < dw)
        { PQ.decrease_p(w,d);
          dist[w] = d;
         }
       }

      pot[v] -= dv;
    }

 } // while the admissible network is acyclic

 delete[] Q;

 if (write_log)
   cout << string("price refine: res = %d  %.2f sec  eps = %.2f",
                                 (int)result,used_time(tt),eps) << endl;

 return result;
}



template<class cap_array, class cost_array, class flow_array, class node_pq,
                                                              class ex_queue>
bool global_update(const graph_t& G, const cap_array& cap,
                                     const cost_array& cost,
                                     flow_array& flow,
                                     excess_array& excess,
                                     pot_array& pot,
                                     dist_array& dist,
                                     current_array& current,
                                     node_pq& PQ,
                                     ex_queue& active,
                                     rcost_t eps, 
                                     bool rebuild_active)
{
  float t1 = used_time();
  num_updates++;

  const double infi = std::numeric_limits<double>::max();

  int n = G.number_of_nodes();

  excess_t sum_excess = 0;

  PQ.clear();

  b_queue<node> permanent(n);

  node v;
  forall_nodes(v,G)
  { excess_t ev = excess[v];
    if (ev >= 0) 
       dist[v] = infi;
    else
     { dist[v] = 0;
       PQ.insert(v,0);
       sum_excess += ev;
      }
   }

  double dmax = 0;

  if (rebuild_active) active.clear();

  while (!PQ.empty())
  { double du = 0;
    node u = PQ.del_min(du);

    permanent.append(u);
    dmax = du;

    if (excess[u] > 0)
    { sum_excess += excess[u];
      if (rebuild_active) active.push(u);
      if (sum_excess == 0) break;
     }

    rcost_t pu = pot[u] + eps;

    G.swap_in_edges(u); 

    edge e;
    forall_out_edges(e,u)
    { if (in_cap(e) == 0) continue;

      node v = G.opposite(e,u);
      double dv = dist[v];
      if (du >= dv) continue;

      rcost_t c = du - n_plus_1*cost[e]-pot[v] + pu;
      if (c < dv)
      { if (dv == infi)
          PQ.insert(v,c);
        else
          PQ.decrease_p(v,c);
        dist[v] = c;
        current[v] = incident_edge(e,false);
       }
     }

    forall_in_edges(e,u)
    { if (out_cap(e) == 0) continue;

      node v = G.opposite(e,u);
      double dv = dist[v];
      if (du >= dv) continue;

      rcost_t c = du + n_plus_1*cost[e]-pot[v] + pu;
      if (c < dv)
      { if (dv == infi)
          PQ.insert(v,c);
        else
          PQ.decrease_p(v,c);
        dist[v] = c;
        current[v] = incident_edge(e,true);
       }
     }


/*
    incident_edge e;
    forall_incident_edges(e,u)
    { incident_edge r = e.reverse();
      if (residual_cap(r) == 0) continue;

      node v = G.opposite(e,u);
      double dv = dist[v];
      if (du >= dv) continue;

      double c = du + reduced_cost(r,v,u) + eps;
      if (c >= dv) continue;

      dist[v] = c;
      current[v] = r;
      PQ.insert(v,c,dist);
     }
*/

    G.swap_in_edges(u); 
  }

  // update potential of all permanent labeled nodes

  forall(v,permanent)
  { pot[v] += (dist[v]-dmax);
    //if (rebuild_active && excess[v] > 0) active.push(v);
   }


  update_limit = int(1.5*permanent.length());

  if (update_limit < n/10) update_limit = n/10;
  if (update_limit <  100) update_limit = 100;

  if (dmax > max_level) max_level = dmax;

  T_update += (used_time()-t1);

  if (write_log)
  { float r = (100.0*permanent.length())/n;
    cout << string("update: eps = %8.2f  r = %5.1f %%  %5.2f sec",
                            eps, r, used_time(t1)) << endl;
   }

  return sum_excess == 0;
}




template <class cap_array, class flow_array>
int filter_in_edges(const graph_t& G, cap_array& cap, flow_array& flow,
                                                      excess_array& excess)
{ G.hide_all_in_edges();
  int count = 0;
  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v)
    { if (in_cap(e) == 0) continue;
      node u = G.opposite(e,v);
      G.restore_in_edge(e,u);
      count++;
     }
  }
  return count;
}
    

template <class cap_array, class cost_array, class flow_array>
void establish_eps_optimality(const graph_t& G, cap_array& cap,
                                                const cost_array& cost,
                                                pot_array& pot,
                                                flow_array& flow,
                                                excess_array& excess,
                                                rcost_t eps)
{
  int m = G.number_of_edges();

  double max_double_int = ldexp(1.0,53);

  double excess_max = std::numeric_limits<excess_t>::max(); 
  if (excess_max > max_double_int) excess_max = max_double_int;

  rcost_t pot_min = std::numeric_limits<rcost_t>::max();

  node v;
  forall_nodes(v,G) 
  { rcost_t pv = pot[v];
    if (pv < pot_min) pot_min = pv;
   }

  forall_nodes(v,G) pot[v] -= pot_min;

  int count = 0;

  forall_nodes(v,G)
  { double dv = 0;

    edge e;
    forall_out_edges(e,v)
    { node w = G.opposite(e,v);
      double dw = 0;
      rcost_t rc = rcost(e,v,w);

      if (rc < -eps) 
      { NT x = out_cap(e);
        if (x > 0)
        { dw += x;
          dv -= x;
          change_flow(e,x)
         }
       }

      if (rc > eps) 
      { NT x = in_cap(e);
        if (x > 0)
        { dw -= x;
          dv += x;
          change_flow(e,-x)
         }
       }

      if (in_cap(e) > 0) count++;

      if (dw > excess_max || dw < -excess_max) 
         error_handler(1, "excess overflow1");

      excess[w] += dw;
     }

    if (dv > excess_max || dv < -excess_max) 
       error_handler(1, "excess overflow2");

    excess[v] += dv;
   }

   // filter in-edges if sparse in residual network

   if (count < 0.5*m) 
   { int mf = filter_in_edges(G,cap,flow,excess);
     if (write_log)
        cout << string("m = %8d   after filter_in_edges: %8d",m,mf) << endl;
    }
}

      

template <class cap_array, class cost_array, class flow_array, 
                                             class supply_array>
bool cost_scaling(const graph_t& G, const cap_array& cap, 
                                    const cost_array& cost,
                                    flow_array& flow,
                                    excess_array& excess,
                                    pot_array& pot,
                                    dist_array& dist,
                                    current_array& current,
                                    const supply_array& supply)
{
  // returns true iff there exists a feasible flow
 
  int n  = G.number_of_nodes();

  bool feasible = true;

  max_level = 0;

  node_pq<double,graph_t> PQ(G);

/*
  int m  = G.number_of_edges();
  rand_int.set_seed(n+m);
  G.permute_edges();
*/


  NT C = 0;
  NT U = 0;  
  double C_average  = 0;
  int tr_nodes = 0;

  node v;
  forall_nodes(v,G) 
      if (excess[v] == 0) tr_nodes++;

  edge e;
  forall_edges(e,G) 
  { flow[e] = 0;
    if (cap[e] > U) U = cap[e];
    NT nc = cost[e];
    if (nc < 0) nc = -nc; 
    C_average += nc;
    if (nc > C) C =  nc;
   }

   C_average /= G.number_of_edges(); 

  assert(C > 0);

  if (write_log) 
  { cout << string("U = %d  C = %d  C_av = %d  tr_nodes = %d  (%.2f %%)",
                    U,C,int(C_average),tr_nodes,(100.0*tr_nodes)/n);
    cout << endl;
   }

  b_queue<node> active(n);

  counter push_count = 0;
  counter final_push_count = 0;
  counter discharge_count = 0;

  int relabel_count = 0;

  int af_count = 0;
  int rs_count = 0;
  int refine_count = 0;

  rcost_t eps = n_plus_1 * C;

  if (tr_nodes < 0.75*n)
     eps = rcost_t(eps/eps_f);
  else
     eps = rcost_t(eps*eps_f);
  

  while (eps > 1)
  {
    float t1 = used_time();
    refine_count++;

    rcost_t old_eps = eps;
    eps = scale_down(eps,alpha);

    if (write_log) 
    { cout << endl;
      cout << string("start of refine(%d):  eps = %f",refine_count,eps) << endl;
     }

    if (refine_count > 1 && af_factor > 0)  // arc fixing
    { float tt = used_time();

      af_filter<rcost_t,graph_t,cap_array,cost_array,flow_array,pot_array> 
                                F(G,cap,cost,flow,pot,eps,af_factor);

      af_count += G.hide_edges(F);

      if (write_log)
      { cout << endl;
        cout << string("arc fixing: bound = %12.2f  m = %d  time = %.2f sec", 
                        af_factor*eps, G.number_of_edges(),used_time(tt));
        cout << endl;
       }
     }
 
    if (refine_count == 1)
      establish_eps_optimality(G,cap,cost,pot,flow,excess,0);
    else
      establish_eps_optimality(G,cap,cost,pot,flow,excess,eps);

    // reset current edges
    node v;
    forall_nodes(v,G) 
      current[v] = G.first_incident_edge(v);


    num_relabels += relabel_count;
    relabel_count = 0;

    if (!global_update(G,cap,cost,flow,excess,pot,dist,current,PQ,active,
                                                                  eps,true))
    { if (refine_count == 1)
      { feasible = false;
        if (write_log) cout << "price update: infeasible problem." << endl;
        goto TERMINATE;
       }
      error_handler(1,"mcf_cost_scaling: global_update failed.");
     }


    if (write_log) 
    { cout << string("|active| = %d",active.size()) << endl;
      cout << string("eps = %9f primal = %12.0f dual = %14.0f fixed = %5d",
                      eps, primal_objective(G,cost,flow), 
                           dual_objective(G,pot,cap,cost,supply,n_plus_1),
                           af_count) << endl;

      int zero_count = 0;
      node v;
      forall_nodes(v,G)
      { edge e;
        forall_out_edges(e,v)
        { node w = G.opposite(e,v);
          if (rcost(e,v,w) == 0) zero_count++;
         }
       }
      cout << string("zero_count = %d",zero_count) << endl;
     }


    update_limit = n;

    while (!active.empty())
    { 
      //if (relabel_count > int(0.4*n) + 30*active.size())
      //if (relabel_count > int(n + 2*active.size()))
      //if (relabel_count > int(n/2 + 5*active.size()))

    //if (relabel_count > update_limit)
      if (relabel_count > int(n/2 + 5*active.size()))
      { 
        bool price_up = global_update(G,cap,cost,flow,excess,pot,dist,current,
                                      PQ,active,eps,true);
        assert(price_up);
        num_relabels += relabel_count;
        relabel_count = 0;
       }
 
      discharge_count++;
 
      node v = active.pop();
      excess_t ev = excess[v];
      incident_edge e_cur = current[v];

      while (ev > 0)
      {
        rcost_t min_rc = std::numeric_limits<rcost_t>::max();
        incident_edge min_e = 0;
        NT min_x = 0;

        if (e_cur == 0) e_cur = G.first_incident_edge(v);

/*
        incident_edge e;
        forall_cyclic_incident_edges(e,e_cur,v)
        { NT x = residual_cap(e);
          if (x == 0) continue;

          node w = G.opposite(e,v);
          if (v == w) continue; // skip self-loops

          rcost_t rc = reduced_cost(e,v,w);

          if (rc < 0)   // admissible
            { push_count++;
              excess_t ew = excess[w];
              if (ev < x) x = (NT)ev; 
              push_flow(e,x);
              if (G.hidden_in_edge(e)) G.restore_in_edge(e,w);

              excess[w] = ew+x;
              ev -= x;
              if (ew <= 0 && excess[w] > 0) active.append(w);

              if (ev == 0) 
              { e_cur = e;
                goto FINISH_DISCHARGE;
               }
             }
          else
            if (rc < min_rc) { min_rc = rc; min_e = e; min_x = x; }
         }
*/

        rcost_t  pot_v = pot[v];
        incident_edge e;
        forall_cyclic_incident_edges(e,e_cur,v)
        { NT fe = flow[e];
          if (e.out())
          { NT x = cap[e]-fe;
            if (x == 0) continue;

            node w = G.opposite(e,v);
            if (v == w) continue; // skip self-loops

            //rcost_t rc = rcost(e,v,w);
            rcost_t rc = n_plus_1*cost[e]-pot_v + pot[w];
            if (rc < 0)   // admissible
              { push_count++;
                excess_t ew = excess[w];
                if (ev < x) x = (NT)ev; 
                flow[e] = fe+x;
                if (fe == 0) G.restore_in_edge(e,w);
  
                excess[w] = ew+x;
                ev -= x;
                if (ew <= 0 && excess[w] > 0) active.append(w);
  
                if (ev == 0) 
                { e_cur = e;
                  goto FINISH_DISCHARGE;
                 }
               }
             else
              if (rc < min_rc) { min_rc = rc; min_e = e; min_x = x; }
           }
           else
           { NT x = fe;
             if (x == 0) continue;

             node w = G.opposite(e,v);
             if (v == w) continue; // skip self-loops

             //rcost_t rc = -rcost(e,w,v);
             rcost_t rc = -n_plus_1*cost[e]-pot_v + pot[w];
             if (rc < 0)   // admissible
              { push_count++;
                excess_t ew = excess[w];
                if (ev < x) x = (NT)ev; 
                flow[e] = fe-x;
                excess[w] = ew+x;
                ev -= x;
                if (ew <= 0 && excess[w] > 0) active.append(w);
  
                if (ev == 0) 
                { e_cur = e;
                  goto FINISH_DISCHARGE;
                 }
               }
            else
              if (rc < min_rc) { min_rc = rc; min_e = e; min_x = x; }
           }
        }

       if (min_e == 0)
       { assert(refine_count == 1);
         feasible = false;
         if (write_log) cout << "relabel: infeasible problem." << endl;
         goto TERMINATE;
        }

        e_cur = min_e;
        relabel_count++;
        pot[v] += (eps+min_rc);

        if (ev <= min_x) // final push
        { final_push_count++;
          NT x = (NT)ev;
          node w = G.opposite(min_e,v);
          push_flow(min_e,x);
          excess_t ew = excess[w];
          excess[w] = ew+x;
          if (ew <= 0 && excess[w] > 0) active.append(w);
          if (G.hidden_in_edge(min_e)) G.restore_in_edge(min_e,w);
          ev = 0;
         }

      } // while ev > 0

FINISH_DISCHARGE:;
 
      current[v] = e_cur;
      excess[v] = 0;
 
     } // while active not empty


    if (refine_count > 1)
    { while (eps > 1)
      { rcost_t new_eps = scale_down(eps,alpha);
        if (price_refinement(G,new_eps,cap,cost,flow,pot,dist,current,PQ))
           eps = new_eps;
        else
           break;
       }
     }

 
    if (write_log) 
    { 
      double z = primal_objective(G,cost,flow); 
      double w = dual_objective(G,pot,cap,cost,supply,n_plus_1);

      cout << string("end of refine: %2d", refine_count) << endl;
      cout << string("eps = %9.0f  z-w = %12.0f  fixed = %8d time: %6.2f", 
                      eps, z-w, af_count, used_time(t1)) << endl;

      for(int i=0; i<75; i++) cout << '-'; cout << endl;
      cout << endl;
     }


    float tt = used_time();

    rcost_t af_eps = check_fixed_arcs(G,cap,cost,flow,pot);

    if (write_log) 
      cout << string("check fixed arcs: t = %.2f", used_time(tt)) << endl;


    double f = af_eps/eps;

    if (write_log)
        cout << string("f = %10.2f  af_factor = %10.2f",f,af_factor) <<endl;

    if (af_eps > eps)
    { assert(af_count > 0);
      float t = used_time();

      if (f > af_update*af_factor) 
         af_factor *= af_update;
      else
         af_factor += f;

      rs_count++;

      eps = old_eps;
      G.restore_all_edges(false);
      af_count = 0;

      if (write_log)
      { cout << string("RESTART: refine = %d f = %.2f  t = %.2f",
                        refine_count,af_factor,used_time(t));
        cout << endl;
       }

      continue;
     }

    //check_eps_optimality(G,cap,cost,flow,pot,eps,"eps-opt-test");

  } // while eps > 1                                    


  assert(eps == 1);

TERMINATE:

  G.restore_all_edges();
 
  if (feasible && write_log) 
  { 
    check_eps_optimality(G,cap,cost,flow,pot,1,"final eps-opt-test (eps=1)");

    compute_optimal_potential(G,cap,cost,flow,pot,dist,PQ);
    rcost_t primal_obj = primal_objective(G,cost,flow);
    rcost_t dual_obj = dual_objective(G,pot,cap,cost,supply,n_plus_1);

    cout << string("primal objective = %f", primal_obj) << endl;
    cout << string("dual   objective = %f", dual_obj) << endl;
    cout << endl;

    assert(primal_obj == dual_obj);
   }


  if (write_log)
  { cout << string("restarts =  %d   max level = %.2f", 
                    rs_count, max_level) << endl;
    cout << endl;
  }

  num_relabels += relabel_count;
  num_discharges = discharge_count;
  num_pushes = push_count;
  num_final_pushes = final_push_count;
  num_arcfixes = af_count;
  num_restarts = rs_count;
  num_refines = refine_count;
 

  return feasible;
}


public:

mcf_cost_scaling() : alpha(0),af_factor(0),af_update(0),eps_f(0),
                     update_limit(0), num_nodes(0), num_edges(0), 
                     num_discharges(0), num_pushes(0), num_final_pushes(0), 
                     num_relabels(0), num_updates(0), num_arcfixes(0), 
                     num_restarts(0), num_refines(0), write_log(false), T(0) {}


template<class lcap_array, class ucap_array, class cost_array, 
                                             class supply_array, 
                                             class flow_array>
bool run(const graph_t& G, const lcap_array& lcap, const ucap_array& ucap,
                                                   const cost_array& cost,
                                                   const supply_array& supply,
                                                   flow_array& flow)
{ 
  T = used_time();

  T_update = 0;
  num_discharges = 0;
  num_relabels = 0;
  num_updates  = 0;
  num_refines = 0;
  num_nodes = G.number_of_nodes();
  num_edges = G.number_of_edges();

  n_plus_1 = num_nodes+1;

  double n = num_nodes;
  double m = num_edges;

  if (alpha == 0)
  { // alpha = log(n^2/m) - 1
    int a = 0;
    for(int d = int(n*n/m); d > 1; d /= 2) a++;
    alpha = max(a,4);
   }


  if (af_factor == 0)
  { //af_factor = 0.5*std::sqrt(n);
    //af_factor = 0.7 * alpha * alpha;

    int d = int(m/n);
/*
    if (d <=  8)      af_factor = 2.00 * std::sqrt(n);
    else if (d <= 16) af_factor = 1.00 * std::sqrt(n);
    else if (d <= 64) af_factor = 0.50 * std::sqrt(n);
    else              af_factor = 0.25 * std::sqrt(n);
*/
    if (d <=  8)      af_factor = 2.00 * std::sqrt(n);
    else if (d <= 64) af_factor = 0.50 * std::sqrt(n);
    else              af_factor = 0.25 * std::sqrt(n);
   }


  af_start_factor = af_factor;


  if (af_update == 0) af_update = 2.0;

  if (eps_f == 0) eps_f = 2.0;


  inf = alpha*num_nodes;

  nd_array n_data(G);
  ed_array e_data(G);

  pot_array     pot(n_data,    &node_data::pot);
  excess_array  excess(n_data, &node_data::excess);
  dist_array    dist(n_data,   &node_data::dist);
  current_array current(n_data,&node_data::curr);

  excess_t total_excess = 0;

  node v;
  forall_nodes(v,G) 
  { pot[v] = 0;
    excess[v] = supply[v];
    total_excess += excess[v];
   }

  if (total_excess != 0) return false; // infeasible


  ucap_array& cap = (ucap_array&)ucap;  // non-const reference to cap-array
                                        // used to modify capacities
                                        // temporarily to eliminate
                                        // lower capacity bounds
  int lc_count = 0;

  forall_nodes(v,G) 
  { edge e;
    forall_out_edges(e,v) 
    { NT lc = lcap[e];
      if (lc != 0)                  // nonzero lower capacity bound
      { node w  = G.opposite(e,v);
        cap[e] -= lc;
        excess[v] -= lc;
        excess[w] += lc;
        lc_count++;
       }
     }
  }

  bool feasible = cost_scaling(G, cap, cost, flow, excess, pot, 
                               dist, current,supply);

  if (lc_count)
  { // adjust flow and restore capacities
    edge e;
    forall_edges(e, G) 
    { NT lc = lcap[e];
      flow[e] += lc;
      cap[e] = capacity(e) + lc;
     }
   }

  T = used_time(T);

  return feasible;
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
  if (write_log) cout << "checking flow " << endl;

  float t = used_time();

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

  edge_array<int,graph_t>  cap(G);
  edge_array<int,graph_t>  f(G);

  node_array<int,graph_t>  dist(G);
  node_array<node,graph_t> succ(G);


  forall_edges(e,G)
  { cap[e] = ucap[e]-lcap[e];
    f[e] = flow[e]-lcap[e];
   }

  // check optimality (negative cycle condition)
  // bellman ford:

  if (!bellman_ford_in_residual(G,cap,cost,f,dist,succ))
  { if (msg != "") msg += " + ";
    msg += "negative cycle in residual network."; 
    return false;
   }

  forall_nodes(v,G) dist[v] = -dist[v];

  if (write_log) 
  { cout << string("checking: time: %.2f sec.", used_time(t)) << endl;
    cout << string("Z = %f",primal_objective(G,cost,f)) << endl;
    cout << string("W = %f",dual_objective(G,dist,cap,cost,supply,1)) << endl;
    cout << endl;
   }

  return true;
}


void set_scale_factor(int a) { alpha = a; }
void set_af_factor(double f) { af_factor = f; }
void set_af_update(double f) { af_update = f; }
void set_eps_factor(double f) { eps_f = f; }

void set_write_log(bool b) { write_log = b; }

float  cpu_time()  const { return T;        }
int    restarts()  const { return num_restarts; }
int    refines()   const { return num_refines; }
double af()        const { return af_factor; }
double af_start()  const { return af_start_factor; }
int    scale_factor() const { return alpha; }



template<class cost_array, class flow_array>
double total_cost(const graph_t& G, const cost_array& cost, 
                                    const flow_array& flow)
{ return primal_objective(G,cost,flow); }


void statistics(ostream& out) const
{ out << string("%5d nodes  %5d edges  a = %d  af = %f",
                 num_nodes,num_edges,alpha,af_factor) << endl;
  out << string("%d discharges %d+%d pushes %d relabels %d updates %d fixes",
   num_discharges, num_pushes,num_final_pushes,num_relabels, num_updates, num_arcfixes) << endl;
  out << string("update: %.2f   max_level: %.2f  (= %.2f * MAXINT)",
                 T_update,max_level,max_level/MAXINT) << endl;
  out << endl;
}


};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520223
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif
