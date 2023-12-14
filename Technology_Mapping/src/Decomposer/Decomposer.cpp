#include "Decomposer.hpp"
#include <fstream>
#include <iostream>
#include <unordered_set>

#include <LEDA/graph/basic_graph_alg.h>

leda::edge Decomposer::getSmallestLevelEdge(process::Graph *graph, const leda::list<leda::edge> &edgeList)
{
    int minLevel = std::numeric_limits<int>::max();
    leda::edge edge, minEdge = nullptr;
    forall(edge, edgeList)
    {
        int level = graph->ledaGraph.inf(graph->ledaGraph.source(edge))->level;
        if (minLevel > level)
        {
            minLevel = level;
            minEdge = edge;
        }
    }
    return minEdge;
}

void Decomposer::decompose(process::Graph *graph)
{
    leda::node node;
    std::unordered_set<std::string> usedNames;
    forall_nodes(node, graph->ledaGraph)
    {
        usedNames.emplace(graph->ledaGraph.inf(node)->name);
    }

    leda::list<leda::node> topologyList;
    leda::TOPSORT(graph->ledaGraph, topologyList);
    forall(node, topologyList)
    {
        std::string curName = graph->ledaGraph.inf(node)->name;
        int suffix = 0;

        leda::list<leda::graph::edge> inEdgeList = graph->ledaGraph.in_edges(node);
        while (inEdgeList.size() > 2)
        {
            leda::edge inEdge1 = getSmallestLevelEdge(graph, inEdgeList);
            inEdgeList.remove(inEdge1);
            leda::edge inEdge2 = getSmallestLevelEdge(graph, inEdgeList);
            inEdgeList.remove(inEdge2);

            std::string legalName;
            while (true)
            {
                legalName = curName + "_" + std::to_string(suffix);
                if (usedNames.count(legalName) == 0)
                {
                    usedNames.emplace(legalName);
                    break;
                }
                ++suffix;
            }
            int maxLevel = std::max(graph->ledaGraph.inf(graph->ledaGraph.source(inEdge1))->level,
                                    graph->ledaGraph.inf(graph->ledaGraph.source(inEdge2))->level);
            process::NodeInfo *nodeInfo = new process::NodeInfo(legalName, graph->ledaGraph.inf(node)->type, maxLevel + 1);
            leda::graph::node newNode = graph->ledaGraph.new_node(nodeInfo);
            graph->strToNode.emplace(legalName, newNode);
            graph->nodeInfos.emplace_back(nodeInfo);
            graph->ledaGraph.move_edge(inEdge1, graph->ledaGraph.source(inEdge1), newNode);
            graph->ledaGraph.move_edge(inEdge2, graph->ledaGraph.source(inEdge2), newNode);
            graph->ledaGraph.new_edge(newNode, node, std::numeric_limits<int>::max());
            inEdgeList = graph->ledaGraph.in_edges(node);
        }

        if (inEdgeList.size() != 0)
        {
            int maxLevel = std::numeric_limits<int>::min();
            leda::edge inEdge;
            forall(inEdge, inEdgeList)
            {
                maxLevel = std::max(maxLevel, graph->ledaGraph.inf(graph->ledaGraph.source(inEdge))->level);
            }
            graph->ledaGraph.inf(node)->level = maxLevel + 1;
        }
        else
        {
            graph->ledaGraph.inf(node)->level = 0;
        }
    }
}
