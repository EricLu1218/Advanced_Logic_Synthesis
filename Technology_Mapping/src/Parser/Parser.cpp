#include "Parser.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

inline std::string strip(std::string input, std::string chars = " \t\r\n")
{
    input.erase(0, input.find_first_not_of(chars));
    input.erase(input.find_last_not_of(chars) + 1);
    return input;
}

Parser::Parser() {}

void Parser::readBlif(const std::string &filepath)
{
    std::ifstream fin(filepath);
    if (!fin.is_open())
    {
        std::cerr << "[Error] Cannot open \"" << filepath << "\".\n";
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector<std::string>> lines;
    for (std::string buff; std::getline(fin, buff);)
    {
        buff = strip(buff);
        if (buff.empty())
            continue;

        std::stringstream buffStream(buff);
        std::vector<std::string> datas;
        for (std::string data; buffStream >> data;)
            datas.emplace_back(std::move(data));

        if (!lines.empty() && lines.back().back() == "\\")
        {
            lines.back().pop_back();
            std::copy(datas.begin(), datas.end(), std::back_inserter(lines.back()));
        }
        else
        {
            lines.emplace_back(std::move(datas));
        }
    }

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string &identifier = lines.at(i).at(0);
        if (identifier == ".model")
        {
            modelName = lines.at(i).at(1);
        }
        else if (identifier == ".inputs")
        {
            std::copy(lines.at(i).begin() + 1, lines.at(i).end(), std::back_inserter(primaryInputNames));
        }
        else if (identifier == ".outputs")
        {
            std::copy(lines.at(i).begin() + 1, lines.at(i).end(), std::back_inserter(primaryOutputNames));
        }
        else if (identifier == ".names")
        {
            std::vector<std::string> fanInNames(lines.at(i).begin() + 1, lines.at(i).end() - 1);
            std::string name = lines.at(i).back();
            raw::Gate::Type type;
            if (fanInNames.size() == 0)
            {
                if (lines.at(i + 1).at(0) == "1")
                {
                    type = raw::Gate::Type::CONST_1;
                    ++i;
                }
                else
                {
                    type = raw::Gate::Type::CONST_0;
                }
            }
            else if (fanInNames.size() == 1)
            {
                if (lines.at(i + 1).at(0) == "1")
                    type = raw::Gate::Type::BUFFER;
                else
                    type = raw::Gate::Type::NOT;
                ++i;
            }
            else
            {
                size_t cnt = 0;
                for (const auto &c : lines.at(i + 1).at(0))
                    if (c == '1')
                        ++cnt;

                if (cnt == 1)
                {
                    type = raw::Gate::Type::OR;
                    i += fanInNames.size();
                }
                else if (cnt == fanInNames.size())
                {
                    type = raw::Gate::Type::AND;
                    ++i;
                }
                else
                {
                    std::cerr << "[Error] Logic error.\n";
                    exit(EXIT_FAILURE);
                }
            }
            gates.emplace_back(new raw::Gate(name, type, std::move(fanInNames)));
        }
        else if (identifier == ".end")
        {
            break;
        }
        else
        {
            std::cerr << "[Error] Parse error.\n";
            exit(EXIT_FAILURE);
        }
    }
}

process::Graph::ptr Parser::getGraph() const
{
    auto graph = new process::Graph(modelName);

    for (const auto &name : primaryInputNames)
    {
        auto nodeInfo = new process::NodeInfo(name, process::NodeInfo::Type::PI);
        auto node = graph->ledaGraph.new_node(nodeInfo);
        graph->primaryInputNodes.emplace_back(node);
        graph->strToNode.emplace(name, node);
        graph->nodeInfos.emplace_back(nodeInfo);
    }

    for (const auto &gate : gates)
    {
        auto nodeInfo = new process::NodeInfo(gate->name, gate->type);
        auto node = graph->ledaGraph.new_node(nodeInfo);
        graph->strToNode.emplace(gate->name, node);
        graph->nodeInfos.emplace_back(nodeInfo);
    }

    for (const auto &name : primaryOutputNames)
    {
        graph->primaryOutputNodes.emplace_back(graph->strToNode.at(name));
    }

    for (const auto &gate : gates)
    {
        auto node = graph->strToNode.at(gate->name);
        for (const auto &name : gate->fanInNames)
            graph->ledaGraph.new_edge(graph->strToNode.at(name), node, std::numeric_limits<int>::max());
    }

    return std::unique_ptr<process::Graph>(graph);
}
