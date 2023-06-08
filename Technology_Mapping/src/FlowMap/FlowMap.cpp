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
        this->truthTable.emplace_back(row);
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
    this->subGraph.clear();
    this->graphToSubGraph.clear();
    this->subGraphToGraph.clear();
}

bool FlowMap::createSubGraph(const leda::node &target)
{
    this->clearSubGraph();

    this->subGraph.predecessors.emplace(target);
    auto info = this->graph.ledaGraph.inf(target);
    this->subGraph.target = this->subGraph.ledaGraph.new_node(info);
    graphToSubGraph.emplace(target, this->subGraph.target);
    subGraphToGraph.emplace(this->subGraph.target, target);

    leda::node_array<bool> visited(this->graph.ledaGraph, false);
    auto edgeList = this->graph.ledaGraph.in_edges(target);
    while (!edgeList.empty())
    {
        auto edge = edgeList.pop_back();
        auto node = this->graph.ledaGraph.source(edge);

        if (!visited[node])
        {
            visited[node] = true;
            this->subGraph.predecessors.emplace(node);
            auto info = this->graph.ledaGraph.inf(node);
            auto newNode = this->subGraph.ledaGraph.new_node(info);
            graphToSubGraph.emplace(node, newNode);
            subGraphToGraph.emplace(newNode, node);

            leda::edge inEdge;
            auto inEdgeList = this->graph.ledaGraph.in_edges(node);
            forall(inEdge, inEdgeList)
            {
                edgeList.push_back(inEdge);
            }
        }
        this->subGraph.ledaGraph.new_edge(this->graphToSubGraph.at(this->graph.ledaGraph.source(edge)),
                                          this->graphToSubGraph.at(this->graph.ledaGraph.target(edge)),
                                          std::numeric_limits<int>::max());
    }

    if (this->graphToSubGraph.find(this->graph.source) != this->graphToSubGraph.end())
    {
        this->subGraph.source = this->graphToSubGraph.at(this->graph.source);
        return true;
    }
    else
    {
        this->graph.ledaGraph.inf(target)->xBar = this->subGraph.predecessors;
        this->subGraph.clear();
        return false;
    }
}

int FlowMap::getMaxFanInLabel(const leda::node &target) const
{
    int maxFanInLabel = std::numeric_limits<int>::min();
    leda::edge inEdge;
    auto inEdgeList = this->subGraph.ledaGraph.in_edges(target);
    forall(inEdge, inEdgeList)
    {
        auto node = this->subGraph.ledaGraph.source(inEdge);
        if (maxFanInLabel < this->subGraph.ledaGraph.inf(node)->label)
            maxFanInLabel = this->subGraph.ledaGraph.inf(node)->label;
    }
    return maxFanInLabel;
}

void FlowMap::collapse(int maxFanInLabel)
{
    std::unordered_set<leda::node> fanInNodes;
    auto nodeList = this->subGraph.ledaGraph.all_nodes();
    nodeList.remove(this->subGraph.source);
    nodeList.remove(this->subGraph.target);

    leda::node node;
    forall(node, nodeList)
    {
        if (this->subGraph.ledaGraph.inf(node)->label == maxFanInLabel)
        {
            leda::edge inEdge;
            auto inEdgeList = this->subGraph.ledaGraph.in_edges(node);
            forall(inEdge, inEdgeList)
            {
                auto sourceNode = this->subGraph.ledaGraph.source(inEdge);
                if (this->subGraph.ledaGraph.inf(sourceNode)->label != maxFanInLabel)
                    fanInNodes.emplace(sourceNode);
            }
            this->subGraph.ledaGraph.del_node(node);
            this->graphToSubGraph.erase(this->subGraphToGraph.at(node));
            this->subGraphToGraph.erase(node);
        }
    }

    leda::edge inEdge;
    auto inEdgeList = this->subGraph.ledaGraph.in_edges(this->subGraph.target);
    forall(inEdge, inEdgeList)
    {
        auto node = this->subGraph.ledaGraph.source(inEdge);
        if (this->subGraph.ledaGraph.inf(node)->label != maxFanInLabel && fanInNodes.count(node) == 1)
            fanInNodes.erase(node);
    }

    for (auto fanInNode : fanInNodes)
        this->subGraph.ledaGraph.new_edge(fanInNode, this->subGraph.target, std::numeric_limits<int>::max());
}

void FlowMap::splitNodes()
{
    auto nodeList = this->subGraph.ledaGraph.all_nodes();
    nodeList.remove(this->subGraph.source);
    nodeList.remove(this->subGraph.target);

    leda::node node;
    forall(node, nodeList)
    {
        auto duplicateNode = this->subGraph.ledaGraph.new_node(nullptr);
        leda::edge outEdge;
        auto outEdgeList = this->subGraph.ledaGraph.out_edges(node);
        forall(outEdge, outEdgeList)
        {
            this->subGraph.ledaGraph.move_edge(outEdge, duplicateNode, this->subGraph.ledaGraph.target(outEdge));
        }
        this->subGraph.ledaGraph.new_edge(node, duplicateNode, 1);
    }
}

std::unordered_set<leda::node> FlowMap::getXBar(leda::edge_array<int> flow) const
{
    leda::edge edge;
    forall_edges(edge, this->subGraph.ledaGraph)
    {
        flow[edge] = this->subGraph.ledaGraph.inf(edge) - flow[edge];
    }

    std::unordered_set<leda::node> xBar = this->subGraph.predecessors;
    std::stack<leda::node> nodeStack;
    nodeStack.push(this->subGraph.source);
    leda::node_array<bool> visited(this->subGraph.ledaGraph, false);
    while (!nodeStack.empty())
    {
        auto node = nodeStack.top();
        nodeStack.pop();

        if (!visited[node])
        {
            visited[node] = true;
            if (this->subGraph.ledaGraph.inf(node) != nullptr)
                xBar.erase(this->subGraphToGraph.at(node));

            auto inEdgeList = this->subGraph.ledaGraph.in_edges(node);
            forall(edge, inEdgeList)
            {
                if (flow[edge] == 0 || flow[edge] != std::numeric_limits<int>::max())
                    nodeStack.push(this->subGraph.ledaGraph.source(edge));
            }
            auto outEdgeList = this->subGraph.ledaGraph.out_edges(node);
            forall(edge, outEdgeList)
            {
                if (flow[edge] != 0)
                    nodeStack.push(this->subGraph.ledaGraph.target(edge));
            }
        }
    }
    return xBar;
}

void FlowMap::labelingPhase()
{
    leda::list<leda::node> topologyList;
    leda::TOPSORT(this->graph.ledaGraph, topologyList);
    topologyList.remove(this->graph.source);
    for (auto node : this->graph.primaryInputNodes)
    {
        this->graph.ledaGraph.inf(node)->label = 0;
        topologyList.remove(node);
    }

    while (!topologyList.empty())
    {
        auto target = topologyList.front();
        topologyList.pop_front();

        if (this->createSubGraph(target) == false) // for const 0, const 1
            continue;

        auto maxFanInLabel = getMaxFanInLabel(target);
        this->collapse(maxFanInLabel);
        this->splitNodes();
        leda::edge_array<int> capacity = this->subGraph.ledaGraph.edge_data();
        leda::edge_array<int> flow(this->subGraph.ledaGraph);
        int cut = leda::MAX_FLOW_T(this->subGraph.ledaGraph, this->subGraph.source, this->subGraph.target, capacity, flow);
        if (cut > this->maxLutInputSize)
        {
            this->graph.ledaGraph.inf(target)->xBar.emplace(target);
            this->graph.ledaGraph.inf(target)->label = maxFanInLabel + 1;
        }
        else
        {
            this->graph.ledaGraph.inf(target)->xBar = this->getXBar(flow);
            this->graph.ledaGraph.inf(target)->label = maxFanInLabel;
        }
    }
}

process::Lut::ptr FlowMap::getLut(const leda::node &target) const
{
    /*  get LUT input  */
    std::unordered_set<leda::node> lutInputs;
    const auto &xBar = this->graph.ledaGraph.inf(target)->xBar;
    auto edgeList = this->graph.ledaGraph.in_edges(target);
    leda::node_array<bool> visited(this->graph.ledaGraph, false);
    while (!edgeList.empty())
    {
        auto edge = edgeList.pop_back();
        auto node = this->graph.ledaGraph.source(edge);
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
                auto inEdgeList = this->graph.ledaGraph.in_edges(node);
                forall(inEdge, inEdgeList)
                {
                    edgeList.push_back(inEdge);
                }
            }
        }
    }

    /* generate LUT and calculate logic function */
    auto lut = new process::Lut(this->graph.ledaGraph.inf(target)->name);
    size_t logicSize = std::pow(2, lutInputs.size());
    std::unordered_map<leda::node, std::vector<int>> nodeToOutputLogics;
    size_t idx = this->maxLutInputSize - lutInputs.size();
    for (const auto &input : lutInputs)
    {
        lut->inputNames.emplace_back(this->graph.ledaGraph.inf(input)->name);
        std::vector<int> outputLogics;
        for (size_t i = 0; i < logicSize; ++i)
            outputLogics.emplace_back(this->truthTable.at(i).at(idx));
        nodeToOutputLogics.emplace(input, std::move(outputLogics));
        ++idx;
    }

    std::vector<leda::node> nodes(xBar.begin(), xBar.end());
    std::sort(nodes.begin(), nodes.end(), [&](const leda::node &a, const leda::node &b) -> bool
              { return this->graph.ledaGraph.inf(a)->level < this->graph.ledaGraph.inf(b)->level; });
    for (const auto &node : nodes)
    {
        std::vector<int> outputLogics;
        const auto type = this->graph.ledaGraph.inf(node)->type;

        std::vector<leda::node> inputNodes;
        leda::edge inEdge;
        auto inEdgeList = this->graph.ledaGraph.in_edges(node);
        forall(inEdge, inEdgeList)
        {
            inputNodes.emplace_back(this->graph.ledaGraph.source(inEdge));
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
    for (auto node : this->graph.primaryOutputNodes)
        nodeStack.push(node);

    std::vector<process::Lut::ptr> luts;
    std::unordered_set<leda::node> haveMapped;
    while (!nodeStack.empty())
    {
        auto node = nodeStack.top();
        nodeStack.pop();
        if (this->graph.ledaGraph.inf(node)->type == process::NodeInfo::Type::PI || haveMapped.count(node))
            continue;

        auto lut = getLut(node);
        for (auto input : lut->inputNames)
            nodeStack.push(this->graph.strToNode.at(input));
        luts.emplace_back(std::move(lut));
        haveMapped.emplace(node);
    }
    return luts;
}

int FlowMap::calCircuitLevel(std::vector<process::Lut::ptr> &luts) const
{
    leda::GRAPH<process::Lut *, int> lutGraph;
    std::unordered_map<std::string, leda::node> strToLutNode;
    for (const auto &node : this->graph.primaryInputNodes)
        strToLutNode.emplace(this->graph.ledaGraph.inf(node)->name, lutGraph.new_node(nullptr));

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

FlowMap::FlowMap(process::Graph *graph, int maxLutInputSize) : graph(*graph), maxLutInputSize(maxLutInputSize)
{
    auto nodeInfo = new process::NodeInfo("source", process::NodeInfo::Type::NONE);
    this->graph.source = this->graph.ledaGraph.new_node(nodeInfo);
    this->graph.nodeInfos.emplace_back(nodeInfo);
    for (auto node : this->graph.primaryInputNodes)
        this->graph.ledaGraph.new_edge(this->graph.source, node, std::numeric_limits<int>::max());
    this->createTruthTable(maxLutInputSize);
}

ResultWriter::ptr FlowMap::solve()
{
    this->labelingPhase();
    auto luts = this->mappingPhase();
    auto circuitLevel = this->calCircuitLevel(luts);
    std::cout << "The circuit level is " << circuitLevel << ".\n"
              << "The number of LUTs is " << luts.size() << ".\n";

    std::vector<std::string> primaryInputNames;
    for (const auto &node : this->graph.primaryInputNodes)
        primaryInputNames.emplace_back(this->graph.ledaGraph.inf(node)->name);
    std::vector<std::string> primaryOutputNames;
    for (const auto &node : this->graph.primaryOutputNodes)
        primaryOutputNames.emplace_back(this->graph.ledaGraph.inf(node)->name);
    return std::unique_ptr<ResultWriter>(
        new ResultWriter(this->graph.name, std::move(primaryInputNames),
                         std::move(primaryOutputNames), std::move(luts), this->maxLutInputSize));
}
