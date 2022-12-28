#include "Decomposer/Decomposer.hpp"
#include "FlowMap/FlowMap.hpp"
#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/ArgumentParser.hpp"
#include "Parser/Parser.hpp"

int main(int argc, char *argv[])
{
    ArgumentParser argParser;
    if (!argParser.parse(argc, argv))
        return 1;

    GlobalTimer globalTimer(10 * 60 - 5);
    globalTimer.startTimer("runtime");
    globalTimer.startTimer("parse input");

    Parser parser;
    parser.readBlif(argParser.inputFile);
    auto graph = parser.getGraph();

    globalTimer.stopTimer("parse input");
    globalTimer.startTimer("decompose");

    Decomposer::decompose(graph.get());

    globalTimer.stopTimer("decompose");
    globalTimer.startTimer("flow map");

    FlowMap flowMap(graph.get(), argParser.maxLutInputSize);
    auto result = flowMap.solve();

    globalTimer.stopTimer("flow map");
    globalTimer.startTimer("write output");

    result->write(argParser.outputFile);

    globalTimer.stopTimer("write output");
    globalTimer.stopTimer("runtime");

    globalTimer.printTime("parse input");
    globalTimer.printTime("decompose");
    globalTimer.printTime("flow map");
    globalTimer.printTime("write output");
    globalTimer.printTime("runtime");
    return 0;
}
