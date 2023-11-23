#include "ResultWriter.hpp"
#include <cmath>
#include <fstream>

void ResultWriter::createTruthTable(int variableNum)
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

ResultWriter::ResultWriter() : maxLutInputSize(0) {}

ResultWriter::ResultWriter(const std::string &modelName, std::vector<std::string> &&primaryInputNames,
                           std::vector<std::string> &&primaryOutputNames, std::vector<process::Lut::ptr> &&luts, int maxLutInputSize)
    : modelName(modelName), primaryInputNames(primaryInputNames),
      primaryOutputNames(primaryOutputNames), luts(std::move(luts)), maxLutInputSize(maxLutInputSize)
{
    createTruthTable(maxLutInputSize);
}

void ResultWriter::write(const std::string &filepath) const
{
    std::ofstream fout(filepath);
    if (!fout.is_open())
    {
        std::cerr << "[Error] Cannot open \"" << filepath << "\".\n";
        exit(EXIT_FAILURE);
    }

    fout << ".model " << modelName << '\n';

    fout << ".inputs";
    for (const auto &name : primaryInputNames)
        fout << ' ' << name;
    fout << '\n';

    fout << ".outputs";
    for (const auto &name : primaryOutputNames)
        fout << ' ' << name;
    fout << '\n';

    for (const auto &lut : luts)
    {
        fout << ".names";
        for (const auto &name : lut->inputNames)
            fout << ' ' << name;
        fout << ' ' << lut->name << '\n';

        if (lut->inputNames.size() == 0 && lut->outputLogics.back() == 1)
        {
            fout << "1\n";
        }
        else
        {
            size_t idx = maxLutInputSize - lut->inputNames.size();
            for (size_t i = 0; i < lut->outputLogics.size(); ++i)
            {
                if (lut->outputLogics.at(i) == 0)
                    continue;

                for (size_t j = idx; j < static_cast<size_t>(maxLutInputSize); ++j)
                    fout << truthTable.at(i).at(j);
                fout << " 1\n";
            }
        }
    }
    fout << ".end\n";
}
