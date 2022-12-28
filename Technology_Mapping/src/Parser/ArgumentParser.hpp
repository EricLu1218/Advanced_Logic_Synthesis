#include <iostream>
#include <string>
#include <unistd.h>

class ArgumentParser
{
public:
    int maxLutInputSize;
    std::string inputFile, outputFile;

    ArgumentParser() : maxLutInputSize(-1) {}

    bool parse(int argc, char *argv[])
    {
        int opt;
        while ((opt = getopt(argc, argv, "k:h")) != -1)
        {
            switch (opt)
            {
            case 'k':
                maxLutInputSize = std::stoi(optarg);
                if (maxLutInputSize < 2)
                {
                    std::cerr << "[Error] k must be grater than 1!" << std::endl;
                    return false;
                }
                break;

            default:
                std::cerr << "Usage: " << argv[0] << " -k [k-LUT] [input file] [output file]" << std::endl;
                return false;
                break;
            }
        }

        if (maxLutInputSize == -1 || argc - optind != 2)
        {
            std::cerr << "Usage: " << argv[0] << " -k [k-LUT] [input file] [output file]" << std::endl;
            return false;
        }
        inputFile = argv[optind];
        outputFile = argv[optind + 1];

        return true;
    }
};
