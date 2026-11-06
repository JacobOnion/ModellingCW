#include <iostream>
#include <fstream>
#include "string.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "must include file" << std::endl;
    }
    
    std::string currentLine;

    std::ifstream inputFile(argv[1]);

    while (getline(inputFile, currentLine))
    {
        std::cout << currentLine;
    }

    inputFile.close();
}