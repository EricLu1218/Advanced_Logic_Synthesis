#include "FlowMap.hpp"
#include <algorithm>
#include <stack>
#include <unordered_set>

#include <LEDA/graph/basic_graph_alg.h>
#include <LEDA/graph/max_flow.h>

void FlowMap::createTruthTable(int variableNum)
{
    static std::vector<int> row;
    if (variableNum == 0)
    {
        truthTable.emplace_back(row);
        return;
    }

    for (int i = 0; i < 2; ++i)
    {
        row.emplace_back(i);
        createTruthTable(variableNum - 1);
        row.pop_back();
    }
}

void FlowMap::clearSubGraph()
{
    subGraph.clear();
    graphToSubGraph.clear();
    subGraphToGraph.clear();
}

bool FlowMap::createSubGraph(const leda::node &target)
{
    clearSubGraph();

    subGraph.predecessors.emplace(target);
    auto info = graph.ledaGraph.inf(target);
    subGraph.target = subGraph.ledaGraph.new_node(info);
    graphToSubGraph.emplace(target, subGraph.target);
    subGraphToGraph.emplace(subGraph.target, target);

    leda::node_array<bool> visited(graph.ledaGraph, false);
    auto edgeList = graph.ledaGraph.in_edges(target);
    while (!edgeList.empty())
    {
        auto edge = edgeList.pop_back();
        auto node = graph.ledaGraph.source(edge);

        if (!visited[node])
        {
            visited[node] = true;
            subGraph.predecessors.emplace(node);
            auto info = graph.ledaGraph.inf(node);
            auto newNode = subGraph.ledaGraph.new_node(info);
            graphToSubGraph.emplace(node, newNode);
            subGraphToGraph.emplace(newNode, node);

            leda::edge inEdge;
            auto inEdgeList = graph.ledaGraph.in_edges(node);
            forall(inEdge, inEdgeList)
            {
                edgeList.push_back(inEdge);
            }
        }
        subGraph.ledaGraph.new_edge(graphToSubGraph.at(graph.ledaGraph.source(edge)),
                                    graphToSubGraph.at(graph.ledaGraph.target(edge)),
                                    std::numeric_limits<int>::max());
    }

    if (graphToSubGraph.find(graph.source) != graphToSubGraph.end())
    {
        subGraph.source = graphToSubGraph.at(graph.source);
        return true;
    }
    else
    {
        graph.ledaGraph.inf(target)->xBar = subGraph.predecessors;
        subGraph.clear();
        return false;
    }
}

int FlowMap::getMaxFanInLabel(const leda::node &target) const
{
    int maxFanInLabel = std::numeric_limits<int>::min();
    leda::edge inEdge;
    auto inEdgeList = subGraph.ledaGraph.in_edges(target);
    forall(inEdge, inEdgeList)
    {
        auto node = subGraph.ledaGraph.source(inEdge);
        if (maxFanInLabel < subGraph.ledaGraph.inf(node)->label)
            maxFanInLabel = subGraph.ledaGraph.inf(node)->label;
    }
    return maxFanInLabel;
}

void FlowMap::collapse(int maxFanInLabel)
{
    std::unordered_set<leda::node> fanInNodes;
    auto nodeList = subGraph.ledaGraph.all_nodes();
    nodeList.remove(subGraph.source);
    nodeList.remove(subGraph.target);

    leda::node node;
    forall(node, nodeList)
    {
        if (subGraph.ledaGraph.inf(node)->label == maxFanInLabel)
        {
            leda::edge inEdge;
            auto inEdgeList = subGraph.ledaGraph.in_edges(node);
            forall(inEdge, inEdgeList)
            {
                auto sourceNode = subGraph.ledaGraph.source(inEdge);
                if (subGraph.ledaGraph.inf(sourceNode)->label != maxFanInLabel)
                    fanInNodes.emplace(sourceNode);
            }
            subGraph.ledaGraph.del_node(node);
            graphToSubGraph.erase(subGraphToGraph.at(node));
            subGraphToGraph.erase(node);
        }
    }

    leda::edge inEdge;
    auto inEdgeList = subGraph.ledaGraph.in_edges(subGraph.target);
    forall(inEdge, inEdgeList)
    {
        auto node = subGraph.ledaGraph.source(inEdge);
        if (subGraph.ledaGraph.inf(node)->label != maxFanInLabel && fanInNodes.count(node) == 1)
            fanInNodes.erase(node);
    }

    for (auto fanInNode : fanInNodes)
        subGraph.ledaGraph.new_edge(fanInNode, subGraph.target, std::numeric_limits<int>::max());
}

void FlowMap::splitNodes()
{
    auto nodeList = subGraph.ledaGraph.all_nodes();
    nodeList.remove(subGraph.source);
    nodeList.remove(subGraph.target);

    leda::node node;
    forall(node, nodeList)
    {
        auto duplicateNode = subGraph.ledaGraph.new_node(nullptr);
        leda::edge outEdge;
        auto outEdgeList = subGraph.ledaGraph.out_edges(node);
        forall(outEdge, outEdgeList)
        {
            subGraph.ledaGraph.move_edge(outEdge, duplicateNode, subGraph.ledaGraph.target(outEdge));
        }
        subGraph.ledaGraph.new_edge(node, duplicateNode, 1);
    }
}

std::unordered_set<leda::node> FlowMap::getXBar(leda::edge_array<int> flow) const
{
    leda::edge edge;
    forall_edges(edge, subGraph.ledaGraph)
    {
        flow[edge] = subGraph.ledaGraph.inf(edge) - flow[edge];
    }

    std::unordered_set<leda::node> xBar = subGraph.predecessors;
    std::stack<leda::node> nodeStack;
    nodeStack.push(subGraph.source);
    leda::node_array<bool> visited(subGraph.ledaGraph, false);
    while (!nodeStack.empty())
    {
        auto node = nodeStack.top();
        nodeStack.pop();

        if (!visited[node])
        {
            visited[node] = true;
            if (subGraph.ledaGraph.inf(node) != nullptr)
                xBar.erase(subGraphToGraph.at(node));

            auto inEdgeList = subGraph.ledaGraph.in_edges(node);
            forall(edge, inEdgeList)
            {
                if (flow[edge] == 0 || flow[edge] != std::numeric_limits<int>::max())
                    nodeStack.push(subGraph.ledaGraph.source(edge));
            }
            auto outEdgeList = subGraph.ledaGraph.out_edges(node);
            forall(edge, outEdgeList)
            {
                if (flow[edge] != 0)
                    nodeStack.push(subGraph.ledaGraph.target(edge));
            }
        }
    }
    return xBar;
}

void FlowMap::labelingPhase()
{
    leda::list<leda::node> topologyList;
    leda::TOPSORT(graph.ledaGraph, topologyList);
    topologyList.remove(graph.source);
    for (auto node : graph.primaryInputNodes)
    {
        graph.ledaGraph.inf(node)->label = 0;
        topologyList.remove(node);
    }

    while (!topologyList.empty())
    {
        auto target = topologyList.front();
        topologyList.pop_front();

        if (createSubGraph(target) == false) // for const 0, const 1
            continue;

        auto maxFanInLabel = getMaxFanInLabel(target);
        collapse(maxFanInLabel);
        splitNodes();
        leda::edge_array<int> capacity = subGraph.ledaGraph.edge_data();
        leda::edge_array<int> flow(subGraph.ledaGraph);
        int cut = leda::MAX_FLOW_T(subGraph.ledaGraph, subGraph.source, subGraph.target, capacity, flow);
        if (cut > maxLutInputSize)
        {
            graph.ledaGraph.inf(target)->xBar.emplace(target);
            graph.ledaGraph.inf(target)->label = maxFanInLabel + 1;
        }
        else
        {
            graph.ledaGraph.inf(target)->xBar = getXBar(flow);
            graph.ledaGraph.inf(target)->label = maxFanInLabel;
        }
    }
}

process::Lut::ptr FlowMap::getLut(const leda::node &target) const
{
    /*  get LUT input  */
    std::unordered_set<leda::node> lutInputs;
    const auto &xBar = graph.ledaGraph.inf(target)->xBar;
    auto edgeList = graph.ledaGraph.in_edges(target);
    leda::node_array<bool> visited(graph.ledaGraph, false);
    while (!edgeList.empty())
    {
        auto edge = edgeList.pop_back();
        auto node = graph.ledaGraph.source(edge);
        if (!visited[node])
        {
            visited[node] = true;
            if (xBar.count(node) == 0)
            {
                lutInputs.emplace(node);
            }
            else
            {
                leda::edge inEdge;
                auto inEdgeList = graph.ledaGraph.in_edges(node);
                forall(inEdge, inEdgeList)
                {
                    edgeList.push_back(inEdge);
                }
            }
        }
    }

    /* generate LUT and calculate logic function */
    auto lut = new process::Lut(graph.ledaGraph.inf(target)->name);
    size_t logicSize = std::pow(2, lutInputs.size());
    std::unordered_map<leda::node, std::vector<int>> nodeToOutputLogics;
    size_t idx = maxLutInputSize - lutInputs.size();
    for (const auto &input : lutInputs)
    {
        lut->inputNames.emplace_back(graph.ledaGraph.inf(input)->name);
        std::vector<int> outputLogics;
        for (size_t i = 0; i < logicSize; ++i)
            outputLogics.emplace_back(truthTable.at(i).at(idx));
        nodeToOutputLogics.emplace(input, std::move(outputLogics));
        ++idx;
    }

    std::vector<leda::node> nodes(xBar.begin(), xBar.end());
    std::sort(nodes.begin(), nodes.end(), [&](const leda::node &a, const leda::node &b) -> bool
              { return graph.ledaGraph.inf(a)->level < graph.ledaGraph.inf(b)->level; });
    for (const auto &node : nodes)
    {
        std::vector<int> outputLogics;
        const auto type = graph.ledaGraph.inf(node)->type;

        std::vector<leda::node> inputNodes;
        leda::edge inEdge;
        auto inEdgeList = graph.ledaGraph.in_edges(node);
        forall(inEdge, inEdgeList)
        {
            inputNodes.emplace_back(graph.ledaGraph.source(inEdge));
        }
        if (inputNodes.size() == 0)
        {
            if (type == process::NodeInfo::Type::CONST_0)
                outputLogics.resize(logicSize, 0);
            else if (type == process::NodeInfo::Type::CONST_1)
                outputLogics.resize(logicSize, 1);
        }
        else if (inputNodes.size() == 1)
        {
            outputLogics = nodeToOutputLogics.at(inputNodes.at(0));
            if (type == process::NodeInfo::Type::NOT)
                for (auto &it : outputLogics)
                    it = !it;
        }
        else
        {
            const auto &input1 = nodeToOutputLogics.at(inputNodes.at(0)),
                       &input2 = nodeToOutputLogics.at(inputNodes.at(1));
            if (type == process::NodeInfo::Type::AND)
                for (size_t i = 0; i < logicSize; ++i)
                    outputLogics.emplace_back(input1[i] & input2[i]);
            else if (type == process::NodeInfo::Type::OR)
                for (size_t i = 0; i < logicSize; ++i)
                    outputLogics.emplace_back(input1[i] | input2[i]);
        }
        nodeToOutputLogics.emplace(node, std::move(outputLogics));
    }
    lut->outputLogics = std::move(nodeToOutputLogics.at(target));

    /*  change the logic function with all true/false to const 1/0  */
    size_t logicSum = 0;
    for (const auto &outputLogic : lut->outputLogics)
        logicSum += outputLogic;
    if (logicSum == 0)
    {
        lut->inputNames.clear();
        lut->outputLogics = std::vector<int>(1, 0);
    }
    else if (logicSum == lut->outputLogics.size())
    {
        lut->inputNames.clear();
        lut->outputLogics = std::vector<int>(1, 1);
    }
    return std::unique_ptr<process::Lut>(lut);
}

std::vector<process::Lut::ptr> FlowMap::mappingPhase()
{
    std::stack<leda::node> nodeStack;
    for (auto node : graph.primaryOutputNodes)
        nodeStack.push(node);

    std::vector<process::Lut::ptr> luts;
    std::unordered_set<leda::node> haveMapped;
    while (!nodeStack.empty())
    {
        auto node = nodeStack.top();
        nodeStack.pop();
        if (graph.ledaGraph.inf(node)->type == process::NodeInfo::Type::PI || haveMapped.count(node))
            continue;

        auto lut = getLut(node);
        for (auto input : lut->inputNames)
            nodeStack.push(graph.strToNode.at(input));
        luts.emplace_back(std::move(lut));
        haveMapped.emplace(node);
    }
    return luts;
}

int FlowMap::calCircuitLevel(std::vector<process::Lut::ptr> &luts) const
{
    leda::GRAPH<process::Lut *, int> lutGraph;
    std::unordered_map<std::string, leda::node> strToLutNode;
    for (const auto &node : graph.primaryInputNodes)
        strToLutNode.emplace(graph.ledaGraph.inf(node)->name, lutGraph.new_node(nullptr));

    for (const auto &lut : luts)
        strToLutNode.emplace(lut->name, lutGraph.new_node(lut.get()));

    for (const auto &lut : luts)
        for (const auto &inputName : lut->inputNames)
            lutGraph.new_edge(strToLutNode.at(inputName), strToLutNode.at(lut->name), 0);

    int circuitLevel = 0;
    leda::TOPSORT1(lutGraph);
    leda::node_array<int> nodeToLevel(lutGraph, 0);
    leda::node node;
    forall_nodes(node, lutGraph)
    {
        if (lutGraph.indeg(node) == 0)
            continue;

        int maxLevel = 0;
        leda::edge inEdge;
        auto inEdgeList = lutGraph.in_edges(node);
        forall(inEdge, inEdgeList)
        {
            auto inNode = lutGraph.source(inEdge);
            maxLevel = std::max(maxLevel, nodeToLevel[inNode]);
        }
        nodeToLevel[node] = maxLevel + 1;
        circuitLevel = std::max(circuitLevel, maxLevel + 1);
    }
    return circuitLevel;
}

FlowMap::FlowMap(process::Graph *graph_, int maxLutInputSize) : graph(*graph_), maxLutInputSize(maxLutInputSize)
{
    auto nodeInfo = new process::NodeInfo("source", process::NodeInfo::Type::NONE);
    graph.source = graph.ledaGraph.new_node(nodeInfo);
    graph.nodeInfos.emplace_back(nodeInfo);
    for (auto node : graph.primaryInputNodes)
        graph.ledaGraph.new_edge(graph.source, node, std::numeric_limits<int>::max());
    createTruthTable(maxLutInputSize);
}

ResultWriter::ptr FlowMap::solve()
{
    labelingPhase();
    auto luts = mappingPhase();
    auto circuitLevel = calCircuitLevel(luts);
    std::cout << "The circuit level is " << circuitLevel << ".\n"
              << "The number of LUTs is " << luts.size() << ".\n";

    std::vector<std::string> primaryInputNames;
    for (const auto &node : graph.primaryInputNodes)
        primaryInputNames.emplace_back(graph.ledaGraph.inf(node)->name);
    std::vector<std::string> primaryOutputNames;
    for (const auto &node : graph.primaryOutputNodes)
        primaryOutputNames.emplace_back(graph.ledaGraph.inf(node)->name);
    return std::unique_ptr<ResultWriter>(
        new ResultWriter(graph.name, std::move(primaryInputNames),
                         std::move(primaryOutputNames), std::move(luts), maxLutInputSize));
}
