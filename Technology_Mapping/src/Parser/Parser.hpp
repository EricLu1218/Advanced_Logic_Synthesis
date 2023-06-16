#pragma once
#include "../Structure/Process/Process.hpp"
#include "../Structure/Raw/Raw.hpp"
#include <string>
#include <vector>

class Parser
{
    std::string modelName;
    std::vector<std::string> primaryInputNames, primaryOutputNames;
    std::vector<raw::Gate::ptr> gates;

public:
    Parser();
    void readBlif(const std::string &filename);
    process::Graph::ptr getGraph() const;
};
