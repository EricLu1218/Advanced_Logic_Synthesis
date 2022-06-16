/*******************************************************************************
+
+  _conauto_base.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
+  base class for all conauto implementations
+
*******************************************************************************/

#ifndef CONAUTO_BASE_H
#define CONAUTO_BASE_H

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/morphism/_graph_morphism_base.h>


LEDA_BEGIN_NAMESPACE


//make sorting stable?
//#define STABLE

//declaration


//information stored with a cell

template<class graph_t, typename ord_t>
class cell_info
{
public:
   // available degree with respect to all remaining nodes in the partition, 
   // with respect to pivot cell
   ord_t pdegrees[4]; 
   ord_t cdegrees[4]; 
   bool valid;
   typename graph_t::node rep; // representative node for this cell, used 
                               // for checking compatibility with node_comp

   LEDA_MEMORY(cell_info)
};

/*
__exportF graph_morphism_algorithm<graph>::cardinality_t fac(int n);   
*/

inline integer fac(int n)  { return factorial(integer(n)); }

template<class graph_t, typename ord_t>
ostream& operator<<(ostream& o, const cell_info<graph_t, ord_t>& ci);



template<class graph_t, typename ord_t>
class conauto_base : public graph_morphism_base<graph_t>
{
protected:
  typedef graph_morphism_base<graph_t> base;
  typedef typename base::node node;
  typedef typename base::edge edge;
  typedef typename base::node_morphism node_morphism;
  typedef typename base::edge_morphism edge_morphism;
  typedef typename base::morphism morphism;
  typedef typename base::morphism_list morphism_list;
  typedef typename base::node_compat node_compat;
  typedef typename base::edge_compat edge_compat;
  typedef typename base::callback callback;
  typedef typename base::prep_graph prep_graph;
  typedef typename base::JOB JOB;
  //partition: unknown (in case of doubt backtracking), by node, 
  // unknown changed to by node, by cell, backtracking

        
protected:
  //compare cells by available degree with respect to all remaining nodes

  template<class inner_graph_t>
  class cell_comparator_part : 
                     public leda_cmp_base<cell_info<inner_graph_t, ord_t> >
  { public:
    virtual int operator()(const cell_info<inner_graph_t, ord_t>& ci1, 
                           const cell_info<inner_graph_t, ord_t>& ci2) const
    { for(int i = 1; i <= 3; i++)
      { if (ci1.pdegrees[i] < ci2.pdegrees[i]) return -1;
        if (ci1.pdegrees[i] > ci2.pdegrees[i]) return  1;
       }

#ifdef STABLE
      //ensure stable sorting, index must have been stored in cdegrees[0]
      if (ci1.cdegrees[0] < ci2.cdegrees[0]) return -1;
      if (ci1.cdegrees[0] > ci2.cdegrees[0]) return  1;
#endif

      return 0;
     }
   };

  //compare cells by available degree with respect to the pivot cell

  template<class inner_graph_t>
  class cell_comparator_cell : 
                        public leda_cmp_base<cell_info<inner_graph_t, ord_t> >
  { public:
    virtual int operator()(const cell_info<inner_graph_t, ord_t>& ci1, 
                           const cell_info<inner_graph_t, ord_t>& ci2) const
    { for(int i = 1; i <= 3; i++)
      { if (ci1.cdegrees[i] < ci2.cdegrees[i]) return -1;
        if (ci1.cdegrees[i] > ci2.cdegrees[i]) return  1;
       }
#ifdef STABLE
      //ensure stable sorting, index must have been stored in cdegrees[0]
      if (ci1.cdegrees[0] < ci2.cdegrees[0]) return -1;
      if (ci1.cdegrees[0] > ci2.cdegrees[0]) return  1;
#endif
      return 0;
     }
   };
public:
	  enum PIVOT { UNKNOWN, NODE, NODE_BACKTR, CELL, BACKTR };
};



template<class graph_t, typename ord_t>
ostream& operator<<(ostream& o, const cell_info<graph_t, ord_t>& ci)
{
  o << "("  << ci.pdegrees[3] << "," << ci.pdegrees[2] << "," << ci.pdegrees[1];
  o << ": " << ci.pdegrees[0] << ")   ";
  o << "("  << ci.cdegrees[3] << "," << ci.cdegrees[2] << "," << ci.cdegrees[1];
  o << ": " << ci.cdegrees[0] << ")   ";         
  o << (ci.valid ? "valid" : "not valid");
  return o;
}


LEDA_END_NAMESPACE

#endif
