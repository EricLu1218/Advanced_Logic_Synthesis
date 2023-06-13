#pragma once
#include "../Raw/Raw.hpp"
#include <climits>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <LEDA/graph/graph.h>

namespace process
{
    struct NodeInfo
    {
        using ptr = std::unique_ptr<NodeInfo>;

        enum struct Type : int
        {
            AND = 0,
            OR,
            NOT,
            BUFFER,
            CONST_0,
            CONST_1,
            PI,
            NONE
        };

        std::string name;
        Type type;
        int level, label;
        std::unordered_set<leda::node> xBar;

        NodeInfo() : type(Type::NONE), level(-1), label(-1) {}
        NodeInfo(const std::string &name, Type type, int level = -1)
            : name(name), type(type), level(level), label(-1) {}
        NodeInfo(const std::string &name, raw::Gate::Type type, int level = -1)
            : name(name), level(level), label(-1)
        {
            setType(type);
        }

        void setType(const raw::Gate::Type &gateType)
        {
            switch (gateType)
            {
            case raw::Gate::Type::AND:
                type = process::NodeInfo::Type::AND;
                break;
            case raw::Gate::Type::OR:
                type = process::NodeInfo::Type::OR;
                break;
            case raw::Gate::Type::NOT:
                type = process::NodeInfo::Type::NOT;
                break;
            case raw::Gate::Type::BUFFER:
                type = process::NodeInfo::Type::BUFFER;
                break;
            case raw::Gate::Type::CONST_0:
                type = process::NodeInfo::Type::CONST_0;
                break;
            case raw::Gate::Type::CONST_1:
                type = process::NodeInfo::Type::CONST_1;
                break;
            default:
                type = process::NodeInfo::Type::NONE;
                break;
            }
        }
    };

    struct Graph
    {
        using ptr = std::unique_ptr<Graph>;

        std::string name;
        leda::GRAPH<NodeInfo *, int> ledaGraph;
        leda::node source;
        std::vector<leda::node> primaryInputNodes, primaryOutputNodes;
        std::unordered_map<std::string, leda::node> strToNode;
        std::vector<NodeInfo::ptr> nodeInfos;

        Graph() : source(nullptr) {}
        Graph(const std::string &name) : name(name), source(nullptr) {}

        void clear()
        {
            name.clear();
            ledaGraph.clear();
            source = nullptr;
            primaryInputNodes.clear();
            primaryOutputNodes.clear();
            strToNode.clear();
            nodeInfos.clear();
        }
    };

    struct SubGraph
    {
        using ptr = std::unique_ptr<SubGraph>;

        leda::GRAPH<NodeInfo *, int> ledaGraph;
        leda::node source, target;
        std::unordered_set<leda::node> predecessors;

        SubGraph() : source(nullptr), target(nullptr) {}

        void clear()
        {
            ledaGraph.clear();
            source = target = nullptr;
            predecessors.clear();
        }
    };

    struct Lut
    {
        using ptr = std::unique_ptr<Lut>;

        std::string name;
        std::vector<std::string> inputNames;
        std::vector<int> outputLogics;

        Lut() {}
        Lut(const std::string name) : name(name) {}
    };
}
