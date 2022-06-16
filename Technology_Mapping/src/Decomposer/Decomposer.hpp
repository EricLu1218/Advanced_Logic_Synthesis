#pragma once
#include "../Structure/Process/Process.hpp"

class Decomposer
{
    static leda::edge getSmallestLevelEdge(process::Graph *graph, const leda::list<leda::edge> &edgeList);

public:
    static void decompose(process::Graph *graph);
};
