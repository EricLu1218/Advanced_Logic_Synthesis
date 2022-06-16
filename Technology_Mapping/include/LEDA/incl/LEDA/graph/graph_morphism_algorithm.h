/*******************************************************************************
+
+  LEDA 5.2  
+
+  graph_morphism_algorithm.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.5 $  $Date: 2007/02/28 07:44:13 $

#ifndef GRAPH_MORPHISM_ALGORITHM_H
#define GRAPH_MORPHISM_ALGORITHM_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520300
#include <LEDA/internal/PREAMBLE.h>
#endif


/*******************************************************************************
+
+  public interface for graph morphism algorithms
+
+  Author: Johannes Singler (2005)
+
*******************************************************************************/

#include <LEDA/numbers/integer.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/list.h>
#include <LEDA/graph/morphism/_callback_base.h>


LEDA_BEGIN_NAMESPACE


/*{\Manpage{graph_morphism_algorithm} {graph_t} {Graph Morphism Algorithm Functionality} {alg}}*/

template<class graph_t = graph>
class graph_morphism_algorithm
{
public:
 /*{\Mtypes}*/

 typedef typename graph_t::node node;
 /*{\Mtypemember the type of an input graph node}*/

 typedef typename graph_t::edge edge;
 /*{\Mtypemember the type of an input graph edge}*/

 typedef node_array<node, graph_t> node_morphism;
 /*{\Mtypemember the type for a found node mapping}*/

 typedef edge_array<edge, graph_t> edge_morphism;
 /*{\Mtypemember the type for a found edge mapping}*/

 typedef leda_cmp_base<node> node_compat;
 /*{\Mtypemember the type for a node compatibility functor}*/

 typedef leda_cmp_base<edge> edge_compat;
 /*{\Mtypemember the type for an edge compatibility functor}*/

 typedef two_tuple<node_morphism, edge_morphism> morphism;
 /*{\Mtypemember the type for a found node and edge mapping}*/

 typedef list<morphism*> morphism_list;
 /*{\Mtypemember the type of a list of all found morphisms}*/

 typedef leda_callback_base<morphism> callback;
 /*{\Mtypemember the type for the callback functor}*/

 //typedef long cardinality_t;
 typedef integer cardinality_t;
 /*{\Mtypemember the number type of the returned cardinality}*/

 typedef void* prep_graph;
 /*{\Mtypemember the type of a prepared graph data structure}*/

 static leda_cmp_base<node> DEFAULT_NODE_CMP;
 static leda_cmp_base<edge> DEFAULT_EDGE_CMP;


 virtual ~graph_morphism_algorithm() {}

 /*{\Moperations}*/
 virtual prep_graph prepare_graph( const graph_t& g, 
                    const node_compat& node_comp = DEFAULT_NODE_CMP,
                    const edge_compat& edge_comp = DEFAULT_EDGE_CMP) const = 0;
 /*{\Mop prepares a data structures of a graph to be used as input to 
         subsequent morphism search calls. This may speed up computation if 
         the same graph is used several times.}*/

 virtual void delete_prepared_graph(prep_graph pg) const = 0;
 /*{\Mop frees the memory allocated to a prepared graph data structure 
         constructed before.}*/

 virtual cardinality_t get_num_calls() = 0;
 /*{\Mop return the number of recursive calls the algorithm has made so far.}*/

 virtual void reset_num_calls() = 0;
 /*{\Mop resets the number of recursive calls to 0.}*/


 virtual bool find_iso(const graph_t& g1, const graph_t& g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 /*{\Mop searches for a graph isomorphism between [[g1]] and [[g2]] and 
         returns it through [[node_morph]] and [[edge_morph]] if a non-NULL 
         pointer to a node map and a non-NULL pointer to an edge map are passed
         respectively. Those must be initialized to [[g2]] and will therefore 
         carry references to the mapped node or edge in [[g1]]. The possible 
         mappings can be restricted by the node and edge compatibility functors
         [[node_comp]] and [[edge_comp]]. This method can be called with 
         prepared graph data structures as input for either graph, too.}*/


 virtual bool find_iso(prep_graph g1, const graph_t& g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_iso(const graph_t& g1, prep_graph g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 
 virtual bool find_iso(prep_graph g1, prep_graph g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_iso(const graph_t& g1, const graph_t& g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for a graph isomorphism between [[g1]] and [[g2]] and returns
         its cardinality. The possible mappings can be restricted by the node 
         and edge compatibility functors [[node_comp]] and [[edge_comp]]. 
         This method can be called with prepared graph data structures as 
         input for either graph, too.}*/


 virtual cardinality_t cardinality_iso(prep_graph g1, const graph_t& g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_iso(const graph_t& g1, prep_graph g2,
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_iso(prep_graph g1, prep_graph g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_iso(const graph_t& g1, const graph_t& g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all graph isomorphisms between [[g1]] and [[g2]] and 
         returns them through [[_isomorphisms]]. The possible mappings can be 
         restricted by the node and edge compatibility functors [[node_comp]] 
         and [[edge_comp]]. This method can be called with prepared graph data 
         structures as input for either graph, too.}*/


 virtual cardinality_t find_all_iso(prep_graph g1, const graph_t& g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_iso(const graph_t& g1, prep_graph g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_iso(prep_graph g1, prep_graph g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_iso(const graph_t& g1, const graph_t& g2, 
                       leda_callback_base<morphism>& _callback, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all graph isomorphisms between [[g1]] and [[g2]] and 
         calls the callback functor [[callb]] for each one. The possible 
         mappings can be restricted by the node and edge compatibility 
         functors [[node_comp]] and [[edge_comp]]. This method can be called 
         with prepared graph data structures as input for either graph, too.}*/


 virtual cardinality_t enumerate_iso(prep_graph g1, const graph_t& g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_iso(const graph_t& g1, prep_graph g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_iso(prep_graph g1, prep_graph g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_sub(const graph_t& g1, const graph_t& g2, 
                         node_morphism* _node_morph = NULL, 
                         edge_morphism* _edge_morph = NULL, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 /*{\Mop searches for a subgraph isomorphism from [[g2]] to [[g1]] and returns 
         it through [[node_morph]] and [[edge_morph]] if a non-NULL pointer to 
         a node map and a non-NULL pointer to an edge map are passed 
         respectively. Those must be initialized to [[g2]] and will therefore 
         carry references to the mapped node or edge in [[g1]]. [[g2]] must 
         not have more nodes or more edges than [[g1]] to make a mapping 
         possible. The possible mappings can be restricted by the node and 
         edge compatibility functors [[node_comp]] and [[edge_comp]]. 
         This method can be called with prepared graph data structures as 
         input for either graph, too.}*/


 virtual bool find_sub(prep_graph g1, const graph_t& g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_sub(const graph_t& g1, prep_graph g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_sub(prep_graph g1, prep_graph g2, 
                       node_morphism* _node_morph = NULL, 
                       edge_morphism* _edge_morph = NULL, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_sub(const graph_t& g1, const graph_t& g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for a subgraph isomorphism from [[g2]] to [[g1]] and returns
         its cardinality. [[g2]] must not have more nodes or more edges than 
         [[g1]] to make a mapping possible. The possible mappings can be 
         restricted by the node and edge compatibility functors [[node_comp]] 
         and [[edge_comp]]. This method can be called with prepared graph data 
         structures as input for either graph, too.}*/


 virtual cardinality_t cardinality_sub(prep_graph g1, const graph_t& g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_sub(const graph_t& g1, prep_graph g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_sub(prep_graph g1, prep_graph g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_sub(const graph_t& g1, const graph_t& g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all subgraph isomorphisms from [[g2]] to [[g1]] and 
         returns them through [[_isomorphisms]]. [[g2]] must not have more 
         nodes or more edges than [[g1]] to make a mapping possible. The 
         possible mappings can be restricted by the node and edge compatibility
         functors [[node_comp]] and [[edge_comp]]. This method can be called 
         with prepared graph data structures as input for either graph, too.}*/


 virtual cardinality_t find_all_sub(prep_graph g1, const graph_t& g2, 
                          list<morphism*>& _isomorphisms, 
                          const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                          const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_sub(const graph_t& g1, prep_graph g2, 
                          list<morphism*>& _isomorphisms, 
                          const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                          const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_sub(prep_graph g1, prep_graph g2, 
                          list<morphism*>& _isomorphisms, 
                          const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                          const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_sub(const graph_t& g1, const graph_t& g2, 
                          leda_callback_base<morphism>& _callback, 
                          const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                          const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all subgraph isomorphisms from [[g2]] to [[g1]] and 
         calls the callback functor [[callb]] for each one. [[g2]] must not 
         have more nodes or more edges than [[g1]] to make a mapping possible.
         The possible mappings can be restricted by the node and edge 
         compatibility functors [[node_comp]] and [[edge_comp]]. 
         This method can be called with prepared graph data structures as 
         input for either graph, too.}*/


 virtual cardinality_t enumerate_sub(prep_graph g1, const graph_t& g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_sub(const graph_t& g1, prep_graph g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_sub(prep_graph g1, prep_graph g2, 
                         leda_callback_base<morphism>& _callback, 
                         const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_mono(const graph_t& g1, const graph_t& g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for a graph monomorphism from [[g2]] to [[g1]] and returns 
         it through [[node_morph]] and [[edge_morph]] if a non-NULL pointer 
         to a node map and a non-NULL pointer to an edge map are passed 
         respectively. Those must be initialized to [[g2]] and will therefore 
         carry references to the mapped node or edge in [[g1]]. [[g2]] must 
         not have more nodes or more edges than [[g1]] to make a mapping 
         possible. The possible mappings can be restricted by the node and 
         edge compatibility functors [[node_comp]] and [[edge_comp]]. 
         This method can be called with prepared graph data structures as 
         input for either graph, too.}*/


 virtual bool find_mono(prep_graph g1, const graph_t& g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_mono(const graph_t& g1, prep_graph g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool find_mono(prep_graph g1, prep_graph g2, 
                        node_morphism* _node_morph = NULL, 
                        edge_morphism* _edge_morph = NULL, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_mono(const graph_t& g1, const graph_t& g2, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for a graph monomorphism from [[g2]] to [[g1]] and returns 
         its cardinality. [[g2]] must not have more nodes or more edges than 
         [[g1]] to make a mapping possible. The possible mappings can be 
         restricted by the node and edge compatibility functors [[node_comp]] 
         and [[edge_comp]]. This method can be called with prepared graph data 
         structures as input for either graph, too.}*/


 virtual cardinality_t cardinality_mono(prep_graph g1, const graph_t& g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_mono(const graph_t& g1, prep_graph g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t cardinality_mono(prep_graph g1, prep_graph g2, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_mono(const graph_t& g1, const graph_t& g2, 
                       list<morphism*>& _isomorphisms, 
                       const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                       const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all graph monomorphisms from [[g2]] to [[g1]] and 
         returns them through [[_isomorphisms]]. [[g2]] must not have more 
         nodes or more edges than [[g1]] to make a mapping possible. The 
         possible mappings can be restricted by the node and edge compatibility 
         functors [[node_comp]] and [[edge_comp]]. This method can be called 
         with prepared graph data structures as input for either graph, too.}*/


 virtual cardinality_t find_all_mono(prep_graph g1, const graph_t& g2, 
                        list<morphism*>& _isomorphisms, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_mono(const graph_t& g1, prep_graph g2, 
                        list<morphism*>& _isomorphisms, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t find_all_mono(prep_graph g1, prep_graph g2, 
                        list<morphism*>& _isomorphisms, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_mono(const graph_t& g1, const graph_t& g2, 
                        leda_callback_base<morphism>& _callback, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop searches for all graph monomorphisms from [[g2]] to [[g1]] and calls 
         the callback functor [[callb]] for each one. [[g2]] must not have 
         more nodes or more edges than [[g1]] to make a mapping possible.
         The possible mappings can be restricted by the node and edge 
         compatibility functors [[node_comp]] and [[edge_comp]]. 

         This method can be called with prepared graph data structures as 
         input for either graph, too.}*/


 virtual cardinality_t enumerate_mono(prep_graph g1, const graph_t& g2, 
                        leda_callback_base<morphism>& _callback, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_mono(const graph_t& g1, prep_graph g2, 
                        leda_callback_base<morphism>& _callback, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual cardinality_t enumerate_mono(prep_graph g1, prep_graph g2, 
                        leda_callback_base<morphism>& _callback, 
                        const node_compat& _node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& _edge_comp = DEFAULT_EDGE_CMP) = 0;

 virtual bool is_graph_isomorphism(const graph_t& g1, const graph_t& g2, 
                        node_morphism const* node_morph, 
                        edge_morphism const* edge_morph = NULL, 
                        const node_compat& node_comp = DEFAULT_NODE_CMP, 
                        const edge_compat& edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop checks whether the morphism given by [[node_morph]] and [[edge_morph]]
         (optional) is a valid graph isomorphisms between [[g1]] and [[g2]].
         The allowed mappings can be restricted by the node and edge 
         compatibility functors [[node_comp]] and [[edge_comp]].}*/


 virtual bool is_subgraph_isomorphism(const graph_t& g1, const graph_t& g2, 
                         node_morphism const* node_morph, 
                         edge_morphism const* edge_morph = NULL, 
                         const node_compat& node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop checks whether the morphism given by [[node_morph]] and [[edge_morph]]
         (optional) is a valid subgraph isomorphisms from [[g2]] to [[g2]].
         The allowed mappings can be restricted by the node and edge 
         compatibility functors [[node_comp]] and [[edge_comp]].}*/


 virtual bool is_graph_monomorphism(const graph_t& g1, const graph_t& g2, 
                         node_morphism const* node_morph, 
                         edge_morphism const* edge_morph = NULL, 
                         const node_compat& node_comp = DEFAULT_NODE_CMP, 
                         const edge_compat& edge_comp = DEFAULT_EDGE_CMP) = 0;
 /*{\Mop checks whether the morphism given by [[node_morph]] and [[edge_morph]]
         (optional) is a valid graph monomorphisms from [[g2]] to [[g1]].
         The allowed mappings can be restricted by the node and edge 
         compatibility functors [[node_comp]] and [[edge_comp]].}*/
};



template<class graph_t>
typename graph_morphism_algorithm<graph_t>::node_compat graph_morphism_algorithm<graph_t>::DEFAULT_NODE_CMP;

template<class graph_t>
typename graph_morphism_algorithm<graph_t>::edge_compat graph_morphism_algorithm<graph_t>::DEFAULT_EDGE_CMP;


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 520300
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif

