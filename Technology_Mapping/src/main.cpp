#include "Decomposer/Decomposer.hpp"
#include "FlowMap/FlowMap.hpp"
#include "Parser/ArgumentParser.hpp"
#include "Parser/Parser.hpp"
#include "Timer/Timer.hpp"

int main(int argc, char *argv[])
{
    ArgumentParser argParser;
    if (!argParser.parse(argc, argv))
        return 1;

    Timer timer(10 * 60 - 5);
    timer.startTimer("runtime");
    timer.startTimer("parse input");

    Parser parser;
    parser.readBlif(argParser.inputFile);
    auto graph = parser.getGraph();

    timer.stopTimer("parse input");
    timer.startTimer("decompose");

    Decomposer::decompose(graph.get());

    timer.stopTimer("decompose");
    timer.startTimer("flow map");

    FlowMap flowMap(graph.get(), argParser.maxLutInputSize);
    auto result = flowMap.solve();

    timer.stopTimer("flow map");
    timer.startTimer("write output");

    result->write(argParser.outputFile);

    timer.stopTimer("write output");
    timer.stopTimer("runtime");

    timer.printTime("parse input");
    timer.printTime("decompose");
    timer.printTime("flow map");
    timer.printTime("write output");
    timer.printTime("runtime");
    return 0;
}
