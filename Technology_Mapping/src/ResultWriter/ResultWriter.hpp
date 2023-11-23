#pragma once
#include "../Structure/Process/Process.hpp"
#include <memory>
#include <string>
#include <vector>

class ResultWriter
{
    std::string modelName;
    std::vector<std::string> primaryInputNames, primaryOutputNames;
    std::vector<process::Lut::ptr> luts;
    int maxLutInputSize;

    std::vector<std::vector<int>> truthTable;

    void createTruthTable(int variableNum);

public:
    using ptr = std::unique_ptr<ResultWriter>;

    ResultWriter();
    ResultWriter(const std::string &modelName, std::vector<std::string> &&primaryInputNames,
                 std::vector<std::string> &&primaryOutputNames, std::vector<process::Lut::ptr> &&luts, int maxLutInputSize);
    void write(const std::string &filepath) const;
};
