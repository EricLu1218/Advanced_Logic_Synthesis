#pragma once
#include "../ResultWriter/ResultWriter.hpp"
#include "../Structure/Process/Process.hpp"
#include <unordered_map>
#include <vector>

class FlowMap
{
    process::Graph &graph;
    int maxLutInputSize;

    process::SubGraph subGraph;
    std::unordered_map<leda::node, leda::node> graphToSubGraph, subGraphToGraph;
    std::vector<std::vector<int>> truthTable;

    void createTruthTable(int variableNum);
    void clearSubGraph();
    bool createSubGraph(const leda::node &target);
    int getMaxFanInLabel(const leda::node &target) const;
    void collapse(int maxFanInLabel);
    void splitNodes();
    std::unordered_set<leda::node> getXBar(leda::edge_array<int> flow) const;
    void labelingPhase();

    process::Lut::ptr getLut(const leda::node &target) const;
    std::vector<process::Lut::ptr> mappingPhase();

    int calCircuitLevel(std::vector<process::Lut::ptr> &luts) const;

public:
    FlowMap(process::Graph *graph, int maxLutInputSize);
    ResultWriter::ptr solve();
};
