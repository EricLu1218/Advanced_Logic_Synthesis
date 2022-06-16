
#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520227
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_bucket_queue.h>
#include <LEDA/graph/member_array.h>

#include <LEDA/graph/node_pq2.h>

#include <LEDA/core/b_queue.h>
#include <LEDA/core/b_stack.h>
#include <LEDA/internal/std/math.h>
#include <LEDA/system/assert.h>


#define MAXPOT MAXDOUBLE

#define rcost(e,v,w) (cost[e] - pot[v] + pot[w])

/*
#define out_cap(e)       cap[e]
#define in_cap(e)        flow[e]
#define total_cap(e)     (cap[e]+flow[e])
#define change_flow(e,x) { flow[e] += x; cap[e] -= x; }
*/

#define out_cap(e)       (cap[e]-flow[e])
#define in_cap(e)        flow[e]
#define total_cap(e)     cap[e]
#define change_flow(e,x) { flow[e] += x; }

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


template<class graph_t, class cap_array>
class edge_smaller {
typedef typename graph_t::edge edge;
const cap_array&  cap;
public:
edge_smaller(const cap_array& c) : cap(c) {}
bool operator()(edge x, edge y) const { return (cap[x] > cap[y]); }
};



template<class graph_t, class cap_array, class cost_array,
                        class flow_array, class pot_array, class rcost_t>
class edge_filter {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

const graph_t&    G;
const cap_array&  cap;
const flow_array& flow;
const cost_array& cost;
const pot_array&  pot;
double af_bound;

public:

edge_filter(const graph_t& g, const cap_array& ca, const cost_array& c, 
                                                   const flow_array& f, 
                                                   const pot_array& p, 
                                                   double af) 
: G(g), cap(ca), flow(f), cost(c), pot(p), af_bound(af) {}

bool operator()(edge e, node u)
{ node v = G.opposite(e,u);
  rcost_t rc = rcost(e,u,v);
  return (rc > af_bound && in_cap(e) ==0 || rc < -af_bound && out_cap(e) == 0);
 }

};




template<class NT>
struct mcf_node_data {
  double   pot; 
  GenPtr   curr; // incident_edge
  NT       excess; 
  int      dist;
  GenPtr   succ; // node
  GenPtr   pred; // node
};

template<class NT>
struct mcf_edge_data {
};



template <class NT, class graph_t=int, class nd_array=int, class ed_array=int>
class mcf_cost_scaling;


template<class NT>
class mcf_cost_scaling<NT,int,int,int>
{ public:
  typedef mcf_node_data<NT> node_data;
  typedef mcf_edge_data<NT> edge_data;
};



template <class NT, class graph_t, class nd_array, class ed_array>

class mcf_cost_scaling 
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  typedef typename graph_t::incident_edge incident_edge;

  typedef mcf_node_data<NT> node_data;
  typedef mcf_edge_data<NT> edge_data;

  //typedef typename pot_array::value_type rcost_t;
  typedef double rcost_t;


 typedef member_array<double, nd_array, node_data> pot_array;
 typedef member_array<NT,     nd_array, node_data> excess_array;
 typedef member_array<int,    nd_array, node_data> dist_array;
 typedef member_array<GenPtr, nd_array, node_data, node> succ_array;
 typedef member_array<GenPtr, nd_array, node_data, node> pred_array;
 typedef member_array<GenPtr, nd_array, node_data, incident_edge> current_array;

  int   alpha;
  float f_update;
  double af_factor;

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

  int max_level;
 
  bool write_log;

  float T;

  float T_update;


void error_handler(int,string msg)
{ cerr << "mcf_cost_scaling: " + msg << endl;
  exit(1);
 }

int scale_down(int eps)
{ int e =  int(double(eps)/alpha + 0.999);
  if (e < 1) e = 1;
  return e;
}


template<class cap_array,class cost_array, class flow_array>
bool check_eps_optimality(const graph_t& G, const cap_array& cap,
                                            const cost_array& cost,
                                            const flow_array& flow,
                                            const pot_array& pot,
                                            rcost_t eps, 
                                            string msg="")
{
  int count = 0;
  node v;
  forall_nodes(v,G)
  { 
/*
    incident_edge e;
    forall_incident_edges(e,v)
    { if (residual_cap(e) == 0) continue;
      node w = G.opposite(e,v);
      if (reduced_cost(e,v,w) < -eps) count++;
     }
*/
   edge e;
   forall_out_edges(e,v)
   { node w = G.opposite(e,v);
     rcost_t rc = rcost(e,v,w);
     if ((rc < -eps && out_cap(e) > 0) || (rc > eps && in_cap(e) > 0)) count++; 
     }
   }

  if (count >  0 && msg != "")
     error_handler(1,"check_eps_opt: " + msg);
     
  return count == 0;
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





template<class cap_array, class cost_array, class flow_array>
bool bellman_ford(const graph_t& G, const cap_array&  cap,
                                    const cost_array& cost,
                                    const pot_array&  pot,
                                    const flow_array& flow,
                                    dist_array& dist,
                                    succ_array& succ)
{
  float t = used_time();

  int n = G.number_of_nodes();

  b_queue<node> Q(n);
  node Q_last = 0;

  node v;
  forall_nodes(v,G) 
  { //dist[v] = (int)pot[v];
    dist[v] = 0;
    Q.append(v);
    succ[v] = v;
    Q_last = v;
   }

  node phase_last = Q_last;
  int  phase_count = 0;

  while (phase_count < n && !Q.empty())
  { node u = Q.pop();
    succ[u] = 0;
    NT du = dist[u];

    incident_edge e;
    forall_incident_edges(e,u) 
    { if (residual_cap(e)== 0) continue;
      node v = G.opposite(e,u);    
      NT d = du + cost[e];
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


cout << string("|Q| = %d   phases = %d  time: %.2f sec.",Q.size(),phase_count,used_time(t)) << endl;

  return Q.empty(); // no negative cycle
}



template<class cap_array, class cost_array, class flow_array>
int topsort_dfs(const graph_t& G, const cap_array& cap, const cost_array& cost,
                                                        flow_array& flow,
                                                        const pot_array& pot,
                                                        pred_array& pred,
                                                        current_array& current,
                                                        node* Q)
{ int n = G.number_of_nodes();
  int n_count = 0;
  int e_count = 0;
  int e_count1 = 0;

  int cycle_count = 0;

  float tt = used_time();

  node* q = Q+n;

  node sentinel = G.stop_node();

  node v;
  forall_nodes(v,G) 
  { pred[v] = 0;
    current[v] = G.first_incident_edge(v);
   }

  node u;
  forall_nodes(u,G)
  { 
    node v = u;

    if (pred[v] != 0) continue;  // visited before

    pred[v] = sentinel; // push v

    while (v != sentinel)
    { 
      incident_edge e = current[v];

      while (e)
      { e_count++;
        node w = G.opposite(e,v);
        if (pred[w] != w && residual_cap(e) > 0 
                         && reduced_cost(e,v,w) < 0) break;
        e = G.next_incident_edge(e,v);
       }

      current[v] = e;
      e_count1++;

      if (e == 0)         // all edges scanned, finish v
      { *--q = v;         // add to front of topsort list
        node u = pred[v]; // pop from stack
        pred[v] = v;      // mark  completed
        v = u;
        if (v != sentinel)
        { incident_edge e = current[v];
          current[v] = e ? G.next_incident_edge(e,v) : G.first_incident_edge(v);
         }
        continue;
       }

      node w  = G.opposite(e,v);
      node pw = pred[w];

      if (pw == 0) // w not visited: tree edge
      { pred[w] = v;
        v = w;
        n_count++;
       }
      else
      { // w visited but not completed: backward edge
        cycle_count++;

        // compute minimum residual capacity on cycle (min_rc)
        // and augment flow by min_rc

        NT   min_rc = MAXINT;
        node min_v  = 0;
        int  count = 0;

        for(node u = v; u != pw; u = pred[u])
        { incident_edge e = current[u];
          NT rc = residual_cap(e);
          if (rc <= min_rc) { min_rc = rc; min_v = u; }
          count++;
         }

        for(node u = v; u != pw; u = pred[u])
        { incident_edge e = current[u];
          push_flow(e,min_rc);
        }

        // continue dfs at min_v by marking all nodes 
        // from min_v to v as unvisited 

        while (v != min_v)  
        { node u = pred[v];
          pred[v] = 0;
          v = u;
         }

      } // backward edge

    } // while stack not empty
  }

 assert(q == Q);

 if (write_log && cycle_count > 0) 
      cout << "cycle_count = " << cycle_count << endl;

if (write_log)
 cout << string("topsort:  %.2f sec  m = %d  m1 = %d  n = %d cycles = %d", 
                        used_time(tt),e_count,e_count1,n_count,cycle_count) << endl;

 return cycle_count == 0;
}




template<class cap_array, class cost_array, class flow_array>
bool price_refine_phase1(const graph_t& G, rcost_t eps,
                                           const cap_array& cap,
                                           const cost_array& cost,
                                           flow_array& flow,
                                           const pot_array& pot,
                                           dist_array& dist, 
                                           pred_array& pred, 
                                           current_array& current)
{
  // sort admissible network topologically

  //float tt = used_time();

  int n = G.number_of_nodes();

  node* Q = new node[n];
  node* Q_stop = Q+n;

  if (!topsort_dfs(G,cap,cost,flow,pot,pred,current,Q))
  { delete[] Q;
    return false; 
   }

  node v;
  forall_nodes(v,G) dist[v] = 0;

  for(node* p = Q; p != Q_stop; p++)
  { node u = *p;
    int du = dist[u];
    incident_edge e;
    forall_incident_edges(e,u)
    { if (residual_cap(e) == 0) continue;
      node v = G.opposite(e,u);
      rcost_t rc = reduced_cost(e,u,v);
      if (rc >= 0) continue;
      int d = du + int((rc+0.5)/eps);
      //int d = du + int(rc/eps+1);
      if (d < dist[v]) dist[v] = d;
     }
   }

  delete[] Q;
//cout << string("phase1 (true):  %.2f sec",used_time(tt)) << endl;
  return true;
}



template<class cap_array, class cost_array, class flow_array, class node_pq>
bool price_refinement(const graph_t& G, rcost_t eps, const cap_array& cap,
                                                     const cost_array& cost,
                                                     flow_array& flow,
                                                     pot_array& pot, 
                                                     dist_array& dist,
                                                     pred_array& pred,
                                                     current_array& current,
                                                     node_pq& PQ)
{
  float tt = used_time();

  if (write_log) 
     cout << string("price refine(eps = %.2f)",eps) << endl;


  bool result = false;

  while (price_refine_phase1(G,eps,cap,cost,flow,pot,dist,pred,current))
  { 
    // admissible network is acyclic

    int count = 0;

    PQ.reset();
    node sentinel = PQ.sentinel();

    node v;
    forall_nodes(v,G) 
    { int dv = dist[v];
      assert(dv <= 0);
      PQ.push(v,inf+dv); // inf_level if dv = 0
      if (dv < 0) count++;
     }

    if (count == 0) // eps-optimal
    { result = true;
      break;
     }

    int level = 0;
    while (level <= PQ.max_level())
    { 
      node v = PQ.pop(level); 
      if (v == sentinel) { level++; continue; }

      int dv = dist[v];
      assert(dv <= 0);

      rcost_t pv = pot[v];
      pot[v] = pv - dv*eps;

      dist[v] = 1-dv;  // mark v scanned (dist > 0)

      incident_edge e;
      forall_incident_edges(e,v)
      { if (residual_cap(e) == 0) continue;
        node w = G.opposite(e,v); 
        int dw = dist[w];
        if (dw > 0 || dv >= dw) continue;
        //rcost_t rc = reduced_cost(e,v,w);
        rcost_t rc = (e.out() ? cost[e] : -cost[e]) - pv + pot[w];
        int d = dv;
        if (rc >= 0) d += int(rc/eps+1);
        if (d < dw)
        { PQ.move(w,inf+dw,inf+d); 
          dist[w] = d;
         }
      }
   }

  if (level > max_level) max_level = level;

 } // while price_refine_phase1


if (write_log)
 cout << string("price refine: res = %d  %.2f sec  eps = %.2f",
                               result,used_time(tt),eps) << endl;

 return result;
}


template<class cap_array, class cost_array, class flow_array, 
                                            class node_pq,
                                            class ex_queue>
bool global_price_update(const graph_t& G, const cap_array& cap,
                                          const cost_array& cost,
                                          const flow_array& flow,
                                          const excess_array& excess,
                                          pot_array& pot,
                                          dist_array& dist,
                                          current_array& current,
                                          node_pq& PQ,
                                          ex_queue& active,
                                          rcost_t eps)
{
  float t1 = used_time();
  num_updates++;

  node    sentinel = PQ.sentinel();
  rcost_t pot_min = MAXDOUBLE;
  int     count = 0;

  active.clear();

  PQ.reset();

  node v;
  forall_nodes(v,G) 
  { NT ev = excess[v];
    if (ev < 0) 
     { dist[v] = 0;
       PQ.push(v,0);
      }
    else
     { dist[v] = inf;
       PQ.push(v,inf);
       if (ev > 0) count++; 
      }
    rcost_t pv = pot[v];
    if (pv < pot_min) pot_min = pv;
   }


  int du = 0;
  int level = 0; 
  while (level <= PQ.max_level())
  { 
    node u = PQ.pop(level);
    if (u == sentinel) { level++; continue; }

    du = level;
    rcost_t pu = pot[u];
    pot[u] = pu + du*eps - pot_min;

    if (excess[u] > 0) 
    { count--; 
      active.push(u);
     }

    incident_edge e;
    forall_incident_edges(e,u)
    { NT x = e.in() ? out_cap(e) : in_cap(e);
      if (x == 0) continue;

      node v = G.opposite(e,u);
      int dv = dist[v];
      if (du >= dv) continue;

      rcost_t rc = (e.in() ? cost[e] : -cost[e]) - pot[v] + pu;
      int c = du;
      if (rc >= 0) c += int(rc/eps + 1);
      if (c < dv)
      { PQ.move(v,dv,c);
        dist[v] = c;
        current[v] = e.reverse();
       }
     }
   }

  // unreached nodes (on level inf)
  for(node v = PQ.first(inf); v != sentinel; v = PQ.next(v))
    pot[v] += (du*eps-pot_min);


  if (level > max_level) max_level = level;

  T_update += (used_time()-t1);

//cout << string("global price update:  %.2f sec",used_time(t1)) << endl;

  return count == 0;
}




template <class cap_array, class cost_array, class flow_array>
void establish_eps_optimality(const graph_t& G, cap_array& cap,
                                                const cost_array& cost,
                                                pot_array& pot,
                                                flow_array& flow,
                                                excess_array& excess,
                                                rcost_t eps)
{
    rcost_t pot_min = MAXPOT;
    rcost_t pot_max = 0;

    node v;
    forall_nodes(v,G) 
    { rcost_t pv = pot[v];
      if (pv < pot_min) pot_min = pv;
      if (pv > pot_max) pot_max = pv;
     }

    forall_nodes(v,G) pot[v] -= pot_min;

    int count1 = 0;
    int count2 = 0;

    forall_nodes(v,G)
    { double dv = 0;

      edge e;
      forall_out_edges(e,v)
      { node w = G.opposite(e,v);
        double ew = excess[w];
        rcost_t rc = rcost(e,v,w);

        if (rc < -eps) 
        { NT x = out_cap(e);
          if (v != w)
          { ew += x;
            dv -= x;
           }
          change_flow(e,x)
          count1++;
         }

        if (rc > eps) 
        { NT x = in_cap(e);
          if (v != w)
          { ew -= x;
            dv += x;
           }
          change_flow(e,-x)
          count2++;
         }

        if (ew > MAXINT || ew < -MAXINT)
            error_handler(1, "excess overflow1");
        excess[w] = int(ew);

       }

     if (dv > MAXINT || dv < -MAXINT)
         error_handler(1, "excess overflow2");

      excess[v] += int(dv);
     }

if (write_log)
   cout << string("count1 = %d  count2 = %d",count1,count2) << endl;
/*
cout << string("pot_min = %.2f  pot_max = %.2f ", pot_min,pot_max) << endl;
*/

}

      

template <class cap_array, class cost_array, class flow_array>
bool cost_scaling(const graph_t& G, const cap_array& cap, 
                                    const cost_array& cost,
                                    flow_array& flow,
                                    excess_array& excess,
                                    pot_array& pot,
                                    dist_array& dist,
                                    succ_array& succ,
                                    pred_array& pred,
                                    current_array& current)
{
  // returns true iff there exists a feasible flow
 
  int n  = G.number_of_nodes();

  bool feasible = true;

  max_level = 0;

  node stop = G.stop_node();
  node_bucket_queue<graph_t,succ_array,pred_array> PQ(succ,pred,inf,stop);

  //node_pq<int,graph_t,bin_heap,succ_array> PQ1(G,succ);

  NT C = 0;   // maximal cost of any edge in G
  NT U = 0;  

  cost_array& ca = (cost_array&)cost;

  edge e;
  forall_edges(e,G) 
  { flow[e] = 0;
    if (cap[e] > U) U = cap[e];
    NT nc = (n+1) * cost[e];
    if (nc/(n+1) != cost[e]) 
        error_handler(1,"cost overflow");
    ca[e] = nc;
    if ( nc > C) C =  nc;
    if (-nc > C) C = -nc;
   }

  assert(C > 0);

  if (write_log) cout << "U = " << U << "   C = " << C << endl;

  b_queue<node> active(n);

  counter push_count = 0;
  counter final_push_count = 0;
  counter discharge_count = 0;

  int relabel_count = 0;

  int af_count = 0;
  int rs_count = 0;
  int refine_count = 0;

  int eps = C + alpha;

  while (eps > 1)
  {
    float t1 = used_time();
    refine_count++;

    int old_eps = eps;
    eps = scale_down(eps);

    if (write_log) 
    { cout << endl;
      cout << string("start of refine %d   eps = %d",refine_count,eps) << endl;
     }

    if (refine_count > 1 && af_factor > 0)  // arc fixing
    { float tt = used_time();
      edge_filter<graph_t,cap_array,cost_array,flow_array,pot_array,rcost_t> 
         F(G,cap,cost,flow,pot,af_factor*eps);
      af_count += G.hide_edges(F);
      node v;
      forall_nodes(v,G) current[v] = G.first_incident_edge(v);

      if (write_log)
      { cout << endl;
        cout << string("arc fixing: bound = %12.2f  m = %d  time = %.2f sec", 
                        af_factor*eps, G.number_of_edges(),used_time(tt));
        cout << endl;
       }
    }
 

    if (refine_count == 1)
     { establish_eps_optimality(G,cap,cost,pot,flow,excess,0);
       if (!global_price_update(G,cap,cost,flow,excess,pot,dist,
                                                       current,PQ,active,eps))
       { feasible = false;
         if (write_log) cout << "price update: infeasible problem." << endl;
         goto TERMINATE;
        }
      }
    else
     { establish_eps_optimality(G,cap,cost,pot,flow,excess,eps);
       node v;
       forall_nodes(v,G)
          if (excess[v] > 0) active.append(v);
      }


    if (write_log) 
    { cout << string("|active| = %d",active.size()) << endl;
      cout << string("eps = %9d cost = %12.0f pot = %14.0f fixed = %5d",
                      eps, total_cost(G,cost,flow,n), total_pot(G,pot),
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


    while (!active.empty())
    { 
      //if (relabel_count > int(f_update*n))
      //if (relabel_count > int(0.4*n) + 30*active.size())
      //if (relabel_count > int(f_update*n) + active.size())
      //if (relabel_count > int(n/2 + 10*active.size()))

      if (relabel_count > int(n/2 + 5*active.size()))
      { assert(global_price_update(G,cap,cost,flow,excess,pot,dist,
                                                   current,PQ,active,eps));
        num_relabels += relabel_count;
        relabel_count = 0;
        continue;
       }
 
      discharge_count++;
 
      node v = active.pop();
      NT ev = excess[v];
      incident_edge e_cur = current[v];
 
      while (ev > 0)
      {
        rcost_t min_rc = MAXPOT;
        incident_edge min_e = 0;
        NT min_x = 0;

        incident_edge e;
        forall_cyclic_incident_edges(e,e_cur,v)
        { NT x = residual_cap(e);
          if (x == 0) continue;

          node w = G.opposite(e,v);
          if (v == w) continue;

          rcost_t rc = reduced_cost(e,v,w);
          if (rc < 0)   // admissible
            { NT ew = excess[w];
              push_count++;
              if (ev < x) x = ev; 
              push_flow(e,x);
              if (ew <= 0 && x > -ew) active.append(w);
              excess[w] = ew+x;
              ev -= x;
              if (ev == 0) 
              { e_cur = e;
                goto FINISH_DISCHARGE;
               }
             }
          else
            if (rc < min_rc) { min_rc = rc; min_e = e; min_x = x; }
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
          node w = G.opposite(min_e,v);
          push_flow(min_e, ev);
          NT ew = excess[w];
          if (ew <= 0 && ev > -ew) active.append(w);
          excess[w] = ew+ev;
          ev = 0;
         }

      } // while ev > 0

FINISH_DISCHARGE:;
 
      current[v] = e_cur;
      excess[v] = 0;
 
     } // while active not empty

    if (write_log) 
    { cout << string("eps = %9d cost = %12.0f pot = %14.0f fixed = %5d", 
                      eps, total_cost(G,cost,flow,n), total_pot(G,pot),
                      af_count) << endl;
     }


    if (refine_count > 1)
    { while (eps > 1)
      { int new_eps = scale_down(eps);
        if (price_refinement(G,new_eps,cap,cost,flow,pot,dist,pred,current,PQ))
           eps = new_eps;
        else
           break;
       }
     }
 
    if (write_log) 
    { cout << string("end of refine: %.2f sec", used_time(t1)) << endl;
      cout << endl;
     }


 // test eps-optimality for all fixed arcs

    int err_count = 0;
    rcost_t rc_max = 0;

    edge e;
    forall_hidden_edges(e,G)
    { node v = G.hidden_source(e);
      node w = G.opposite(e,v);
      rcost_t rc = rcost(e,v,w);
      if (rc < -eps && out_cap(e) > 0)
      { err_count++;
        if(-rc > rc_max) rc_max = -rc;
       }
      if (rc > eps && in_cap(e) > 0)
      { err_count++;
        if(rc > rc_max) rc_max = rc;
       }
    }

    if (err_count > 0)
    { assert(af_count > 0);


      rs_count++;

      //af_factor *= 2;
      af_factor *= 1.5;
      eps = old_eps;

/*
      eps = min(old_eps,int((rc_max*alpha)/af_factor));

      if (write_log) 
      { cout << string("arc fixing: RESTART err_count = %d  rc_max/eps = %.2f",
                                            err_count,rc_max/eps) << endl;
        cout << string("eps = %d  old_eps = %d",eps,old_eps) << endl;
      }
*/

      G.restore_all_edges(false);
      af_count = 0;
      continue;
     }
  
    //check_eps_optimality(G,cap,cost,flow,pot,eps,"eps-opt-test");

  } // while eps > 1                                    


  assert(eps == 1);

TERMINATE:

{
  float ttt = used_time();
  G.restore_all_edges();
  if (write_log)
         cout << string("restore_all_edges: %.2f",used_time(ttt)) << endl;
}

  if (feasible) 
     check_eps_optimality(G,cap,cost,flow,pot,1,"final eps-opt-test");

  num_relabels += relabel_count;
  num_discharges = discharge_count;
  num_pushes = push_count;
  num_final_pushes = final_push_count;
  num_arcfixes = af_count;
  num_restarts = rs_count;
  num_refines = refine_count;

  if (write_log)
  { cout << string("restarts =  %d   max level = %.2f * n", 
                    rs_count, float(max_level)/n) << endl;
    cout << endl;
   }

  forall_edges(e,G) ca[e] /= (n+1);
  return feasible;
}



public:

mcf_cost_scaling() : alpha(0), f_update(1.0), af_factor(0.0),
                     num_nodes(0), num_edges(0), num_discharges(0), 
                     num_pushes(0), num_final_pushes(0), num_relabels(0), 
                     num_updates(0), num_arcfixes(0), num_restarts(0), 
                     num_refines(0), write_log(false), T(0) {}


template<class lcap_array, class ucap_array, class cost_array, 
                                             class supply_array, 
                                             class flow_array>
bool run(const graph_t& G, const lcap_array& lcap, const ucap_array& ucap,
                                                   const cost_array& cost,
                                                   const supply_array& supply,
                                                   flow_array& flow,
                                                   float f = 100,
                                                   int a = 12)
{ T = used_time();
  T_update = 0;
  num_discharges = 0;
  num_relabels = 0;
  num_updates  = 0;
  num_refines = 0;
  num_nodes = G.number_of_nodes();
  num_edges = G.number_of_edges();

  //af_factor = f*std::sqrt(double(num_edges));
  //af_factor = 3*f*std::sqrt(double(num_nodes));
  //af_factor = 7.5*f*num_nodes/std::sqrt(double(num_edges));
  //double d = double(num_nodes)/num_edges;
  //af_factor = 0.2*f*d*num_nodes;
  //af_factor = f * num_nodes * alpha;
  //af_factor = f * num_nodes;

  alpha = a;
  af_factor = f;

  inf = alpha*num_nodes;

/*
  rand_int.set_seed(num_nodes+num_edges);
  G.permute_edges();
*/


  nd_array n_data(G);
  ed_array e_data(G);

  pot_array     pot(n_data,    &node_data::pot);
  excess_array  excess(n_data, &node_data::excess);
  dist_array    dist(n_data,   &node_data::dist);
  succ_array    succ(n_data,   &node_data::succ);
  pred_array    pred(n_data,   &node_data::pred);
  current_array current(n_data,&node_data::curr);


  NT total_excess = 0;

  node v;
  forall_nodes(v,G) 
  { pot[v] = 0;
    excess[v] = supply[v];
    current[v] = G.first_incident_edge(v);
    total_excess += excess[v];
   }

  if (total_excess != 0) return false; // infeasible


  ucap_array& cap = (ucap_array&)ucap;  // non-const reference to cap array
                                        // used to modify capacities
                                        // temporarily for elimination of
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

  bool feasible = cost_scaling(G, cap, cost, flow, excess,
                                             pot, dist, succ, pred, current);

  if (lc_count)
  { // adjust flow and restore capacities
    edge e;
    forall_edges(e, G) 
    { NT lc = lcap[e];
      if (lc != 0)
      { flow[e] += lc;
        cap[e] = total_cap(e);
       }
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


  // check optimality (negative cycle condition)
  // bellman ford:

  int n = G.number_of_nodes();

  b_queue<node> Q(n);
  node Q_last = 0;

  node_array<node,graph_t> succ(G); // if v in Q then succ[v] = v else 0
  node_array<int,graph_t> dist(G);

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

    NT du = dist[u];

    edge e;
    forall_out_edges(e,u) 
    { if (flow[e] == ucap[e]) continue;
      node v = G.opposite(e,u);    
      NT d = du + cost[e];
      if (d < dist[v])
      { dist[v] = d;
        if (succ[v] == 0) { Q.append(v); succ[v] = v; Q_last = v; }
       }
     } 

    forall_in_edges(e,u) 
    { if (flow[e] == lcap[e]) continue;
      node v = G.opposite(e,u);    
      NT d = du - cost[e];
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

  if (write_log) 
      cout << string("phases: %d  time: %.2f sec.",phase_count,used_time(t)) << endl;

  return Q.empty(); // no negative cycle
}


void set_write_log(bool b) { write_log = b; }

float  cpu_time() const { return T;        }
int    restarts() const { return num_restarts; }
int    refines()  const { return num_refines; }
double af()       const { return af_factor; }

template<class cost_array, class flow_array>
double total_cost(const graph_t& G, const cost_array& cost, 
                                    const flow_array& flow, int f=1)
{ double c = 0;
  edge e;
  forall_edges(e,G) c += double(cost[e]/f)*flow[e];
  return c;
}

rcost_t total_pot(const graph_t& G, const pot_array& pot)
{ rcost_t P = 0;
  node v;
  forall_nodes(v,G) P += pot[v];
  return P;
}


void statistics(ostream& out) const
{ out << string("%5d nodes  %5d edges  a = %d  af = %f",
                 num_nodes,num_edges,alpha,af_factor) << endl;
  out << string("%d discharges %d+%d pushes %d relabels %d updates %d fixes",
   num_discharges, num_pushes,num_final_pushes,num_relabels, num_updates, num_arcfixes) << endl;
  out << string("update: %f",T_update) << endl;
  out << endl;
}


};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520227
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif
