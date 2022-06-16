/*******************************************************************************
+
+  LEDA 5.2  
+
+  graph_morphism.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.10 $  $Date: 2007/02/28 07:44:13 $

#ifndef GRAPH_MORPHISM_H
#define GRAPH_MORPHISM_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520265
#include <LEDA/internal/PREAMBLE.h>
#endif

/*******************************************************************************
+
+  public wrapper class for graph morphism algorithms
+
+  Author: Johannes Singler (2005)
+
*******************************************************************************/


#include <LEDA/graph/graph.h>

#include <LEDA/graph/morphism/_vf2_exp.h>
#include <LEDA/graph/morphism/_vf2_best.h>
#include <LEDA/graph/morphism/_vf2_simple.h>
#include <LEDA/graph/morphism/_vf2_better.h>
#include <LEDA/graph/morphism/_vf2.h>
#include <LEDA/graph/morphism/_conauto_basic.h>
#include <LEDA/graph/morphism/_conauto_fast.h>
#include <LEDA/graph/morphism/_conauto.h>
#include <LEDA/graph/graph_morphism_checker.h>
#include <LEDA/graph/graph_morphism_algorithm.h>


LEDA_BEGIN_NAMESPACE


/* extract as Manual Manpage */
/*{\Manpage{graph_morphism} {graph_t, impl} {Graph Morphism Algorithms} {alg}}*/


template<class graph_t = graph, class impl = vf2<graph_t> >
class graph_morphism : public graph_morphism_algorithm<graph_t>
{
/*{\Mdefinition
  An instance [[\Mvar]] of the parameterized data type |\Mname| is
  an algorithm object that supports finding graph isomorphisms, 
  subgraph isomorphisms, graph monomorphisms and graph automorphisms. 
  The first parameter type parametrizes the input graphs' types.
  It defaults to [[graph]].
  The second parameter type determines the actual algorithm implementation
  to use. There are two implementations available so far which work
  differently well for certain types of graphs. More details can
  be found in the report {\em Graph Isomorphism Implementation for LEDA}
  by Johannes Singler. It is available from our homepage. You can also
  contact our support team to get it: 
  {\tt support@algorithmic-solutions.com} resp. {\tt support@quappa.com}.
}*/

public:

 typedef graph_morphism_algorithm<graph_t> gma;
 typedef typename gma::node_morphism node_morphism;
 typedef typename gma::edge_morphism edge_morphism;
 typedef typename gma::morphism morphism;
 typedef typename gma::node_compat node_compat;
 typedef typename gma::edge_compat edge_compat;
 typedef typename gma::cardinality_t cardinality_t;
 typedef typename gma::prep_graph prep_graph;

private:

 impl alg;
 graph_morphism_checker<graph_t> checker;

public:

 virtual ~graph_morphism() {}; // virtual destructor (sn)

 prep_graph prepare_graph(const graph_t& _g, 
                 const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                 const edge_compat& ec = gma::DEFAULT_EDGE_CMP) const
 {
    const node_compat* _node_comp = &nc;
    const edge_compat* _edge_comp = &ec;
    if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
    if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

    return (prep_graph) alg.prepare_graph(_g,_node_comp,_edge_comp); 
  }


 void delete_prepared_graph(prep_graph pg) const
 { alg.delete_prepared_graph(pg); }


 virtual bool find_iso(const graph_t& _g1, const graph_t& _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { 
   alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_iso(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_iso(const graph_t& _g1, prep_graph _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_iso(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_iso(prep_graph _g1, const graph_t& _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_iso(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_iso(prep_graph _g1, prep_graph _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_iso(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual cardinality_t cardinality_iso(const graph_t& _g1, const graph_t& _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_iso(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_iso(prep_graph _g1, const graph_t& _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_iso(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_iso(const graph_t& _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_iso(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }

 virtual cardinality_t cardinality_iso(prep_graph _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_iso(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
 }

 virtual cardinality_t find_all_iso(const graph_t& _g1, const graph_t& _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_iso(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }

 virtual cardinality_t find_all_iso(prep_graph _g1, const graph_t& _g2, 
                        list<morphism*>& _isomorphisms, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_iso(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_iso(const graph_t& _g1, prep_graph _g2, 
                        list<morphism*>& _isomorphisms, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_iso(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_iso(prep_graph _g1, prep_graph _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_iso(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_iso(const graph_t& _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_iso(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_iso(prep_graph _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_iso(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_iso(const graph_t& _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_iso(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_iso(prep_graph _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_iso(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual bool find_sub(const graph_t& _g1, const graph_t& _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_sub(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_sub(const graph_t& _g1, prep_graph _g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_sub(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_sub(prep_graph _g1, const graph_t& _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_sub(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_sub(prep_graph _g1, prep_graph _g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_sub(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }

 virtual cardinality_t cardinality_sub(const graph_t& _g1, const graph_t& _g2,
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_sub(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_sub(prep_graph _g1, const graph_t& _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_sub(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_sub(const graph_t& _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_sub(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_sub(prep_graph _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_sub(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
 }


 virtual cardinality_t find_all_sub(const graph_t& _g1, const graph_t& _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_sub(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_sub(prep_graph _g1, const graph_t& _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_sub(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_sub(const graph_t& _g1, prep_graph _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_sub(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_sub(prep_graph _g1, prep_graph _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_sub(_isomorphisms, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_sub(const graph_t& _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP) 
  { alg.input1(_g1);
    alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

    cardinality_t n = alg.go_enumerate_sub(_callb, _node_comp, _edge_comp);
    alg.clean_up();
    return n;
  }


 virtual cardinality_t enumerate_sub(prep_graph _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_sub(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_sub(const graph_t& _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_sub(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_sub(prep_graph _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_sub(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual bool find_mono(const graph_t& _g1, const graph_t& _g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_mono(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_mono(const graph_t& _g1, prep_graph _g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
  { alg.input1(_g1);
    alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

    bool b = alg.go_find_mono(_node_morph, _edge_morph, _node_comp, _edge_comp);
    alg.clean_up();
    return b;
   }


 virtual bool find_mono(prep_graph _g1, const graph_t& _g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   bool b = alg.go_find_mono(_node_morph, _edge_morph, _node_comp, _edge_comp);
   alg.clean_up();
   return b;
  }


 virtual bool find_mono(prep_graph _g1, prep_graph _g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                        const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
  { alg.input1(_g1);
    alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

    bool b = alg.go_find_mono(_node_morph, _edge_morph, _node_comp, _edge_comp);
    alg.clean_up();
    return b;
   }


 virtual cardinality_t cardinality_mono(const graph_t& _g1, const graph_t& _g2,
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_mono(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
 }


 virtual cardinality_t cardinality_mono(prep_graph _g1, const graph_t& _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_mono(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_mono(const graph_t& _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_mono(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t cardinality_mono(prep_graph _g1, prep_graph _g2, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_cardinality_mono(_node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_mono(const graph_t& _g1, const graph_t& _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_mono(_isomorphisms,_node_comp,_edge_comp);
   alg.clean_up();
   return n;
  }

 virtual cardinality_t find_all_mono(prep_graph _g1, const graph_t& _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_mono(_isomorphisms,_node_comp,_edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_mono(const graph_t& _g1, prep_graph _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_mono(_isomorphisms,_node_comp,_edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t find_all_mono(prep_graph _g1, prep_graph _g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_find_all_mono(_isomorphisms,_node_comp,_edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_mono(const graph_t& _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_mono(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_mono(prep_graph _g1, const graph_t& _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_mono(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_mono(const graph_t& _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_mono(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 virtual cardinality_t enumerate_mono(prep_graph _g1, prep_graph _g2, 
                       leda_callback_base<morphism>& _callb, 
                       const node_compat& nc = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& ec = gma::DEFAULT_EDGE_CMP)
 { alg.input1(_g1);
   alg.input2(_g2);

   const node_compat* _node_comp = &nc;
   const edge_compat* _edge_comp = &ec;
   if (_node_comp == &gma::DEFAULT_NODE_CMP) _node_comp = NULL;
   if (_edge_comp == &gma::DEFAULT_EDGE_CMP) _edge_comp = NULL;

   cardinality_t n = alg.go_enumerate_mono(_callb, _node_comp, _edge_comp);
   alg.clean_up();
   return n;
  }


 void  reset_num_calls() { alg.num_calls = 0; }

 cardinality_t get_num_calls() { return alg.num_calls; }


 virtual bool is_graph_isomorphism(const graph_t& g1, const graph_t& g2, 
                       node_morphism const* node_morph, 
                       edge_morphism const* edge_morph = NULL, 
                       const node_compat& node_comp = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP)
  { 
    return checker.is_graph_isomorphism(g1, g2, node_morph, edge_morph, 
                                                node_comp, edge_comp);
   }


 virtual bool is_subgraph_isomorphism(const graph_t& g1, const graph_t& g2, 
                       node_morphism const* node_morph, 
                       edge_morphism const* edge_morph = NULL, 
                       const node_compat& node_comp = gma::DEFAULT_NODE_CMP,
                       const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP)
 {
   return checker.is_subgraph_isomorphism(g1, g2, node_morph, edge_morph, 
                                                  node_comp, edge_comp);
  }


 virtual bool is_graph_monomorphism(const graph_t& g1, const graph_t& g2, 
                       node_morphism const* node_morph, 
                       edge_morphism const* edge_morph = NULL, 
                       const node_compat& node_comp = gma::DEFAULT_NODE_CMP, 
                       const edge_compat& edge_comp = gma::DEFAULT_EDGE_CMP)
 {
   return checker.is_graph_monomorphism(g1, g2, node_morph, edge_morph, 
                                                node_comp, edge_comp);
  }

};



/*{\Mimplementation
Allowed implementations parameters are [[vf2<graph_t>]] and [[conauto<graph_t, ord_t>]].}*/

/*{\Mexample
\vspace{-0.5cm}
\begin{verbatim}
#include <LEDA/graph/graph_morphism.h>

// declare the input graphs.
graph g1, g2;

// In order to use node compatibility, declare associated node maps for the 
// attributes and a corresponding node compatibility function 
// (exemplary, see above for the definition of identity_compatibility).

node_map<int> nm1(g1), nm2(g2);
identity_compatibility<int> ic(nm1, nm2);

// do something useful to build up the graphs and the attributes

// instantiate the algorithm object
graph_morphism<graph, conauto<graph> > alg;

// declare the node and edge mapping arrays
node_array<node> node_mapping(g2);
edge_array<edge> edge_mapping(g2);

// prepare a graph morphism data structure for the first graph.
graph_morphism_algorithm<>::prep_graph pg1 = alg.prepare_graph(g1, ic);

// find the graph isomorphism.
bool isomorphic = alg.find_iso(pg1, g2, &node_mapping, &edge_mapping, ic);

// delete the prepared graph data structure again.
alg.delete_prepared_graph(pg1);
\end{verbatim}

Please see {\tt demo/graph\_iso/gw\_isomorphism.cpp} for an interactive
demo program.

}*/


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520265
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif

