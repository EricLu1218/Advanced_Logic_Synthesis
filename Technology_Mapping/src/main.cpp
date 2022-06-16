#include "Decomposer/Decomposer.hpp"
#include "FlowMap/FlowMap.hpp"
#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/Parser.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " -k <k-LUT> <input file> <output file>\n";
        return 0;
    }
    else if (std::stoi(argv[2]) < 2)
    {
        std::cerr << "[Error] k must be grater than 1!\n";
        return 0;
    }

    GlobalTimer globalTimer(10 * 60 - 5);
    globalTimer.startTimer("runtime");
    globalTimer.startTimer("parse input");

    Parser parser;
    parser.readBlif(argv[3]);
    auto graph = parser.getGraph();

    globalTimer.stopTimer("parse input");
    globalTimer.startTimer("decompose");

    Decomposer::decompose(graph.get());

    globalTimer.stopTimer("decompose");
    globalTimer.startTimer("flow map");

    FlowMap flowMap(graph.get(), std::stoi(argv[2]));
    auto result = flowMap.solve();

    globalTimer.stopTimer("flow map");
    globalTimer.startTimer("write output");

    result->write(argv[4]);

    globalTimer.stopTimer("write output");
    globalTimer.stopTimer("runtime");

    globalTimer.printTime("parse input");
    globalTimer.printTime("decompose");
    globalTimer.printTime("flow map");
    globalTimer.printTime("write output");
    globalTimer.printTime("runtime");
    return 0;
}
