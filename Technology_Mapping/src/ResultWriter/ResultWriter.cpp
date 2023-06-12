#include "ResultWriter.hpp"
#include <cmath>
#include <fstream>

void ResultWriter::createTruthTable(int variableNum)
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

void ResultWriter::write(const std::string &filename) const
{
    std::ofstream fout(filename);
    if (!fout)
    {
        std::cerr << "[Error] Cannot open \"" << filename << "\".\n";
        exit(EXIT_FAILURE);
    }

    fout << ".model " << this->modelName << '\n';

    fout << ".inputs";
    for (const auto &name : this->primaryInputNames)
        fout << ' ' << name;
    fout << '\n';

    fout << ".outputs";
    for (const auto &name : this->primaryOutputNames)
        fout << ' ' << name;
    fout << '\n';

    for (const auto &lut : this->luts)
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
            size_t idx = this->maxLutInputSize - lut->inputNames.size();
            for (size_t i = 0; i < lut->outputLogics.size(); ++i)
            {
                if (lut->outputLogics.at(i) == 0)
                    continue;

                for (size_t j = idx; j < static_cast<size_t>(this->maxLutInputSize); ++j)
                    fout << truthTable.at(i).at(j);
                fout << " 1\n";
            }
        }
    }
    fout << ".end\n";
}
