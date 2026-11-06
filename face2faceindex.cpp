#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "triangle_renderer/Cartesian3.h"
#include <vector>
#include <algorithm>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "must include file" << std::endl;
    }

    // File initialisation and setup
    std::string inputName = argv[1];

    size_t pathLen = inputName.find_last_of("/");

    if (pathLen == std::string::npos)
    {
        pathLen = -1;
        std::cout << "no slashes\n" << std::endl;
    }

    size_t nameLen = inputName.find(".");


    if (pathLen == std::string::npos)
    {
        std::cout << "invalid file, must be .tri\n" << std::endl;
        return 1;
    }

    std::string strippedName = inputName.substr(pathLen + 1, nameLen - (pathLen + 1));

    std::ofstream indexFile(strippedName + ".face");
    
    indexFile << "# University of Leeds 2026-2027\n"
                "# COMP 5812M Assignment 1\n"
                "# Jacob Onion\n"
                "# 201710645\n"
                "#\n"
                "# Object Name: " + strippedName + "\n"
                "# Vertices=8 Faces=12\n"
                "#\n";


    // File processing
    std::vector<Cartesian3> mesh;
    std::vector<int> vertexIndices; 
    std::vector<Cartesian3> faces;
    std::string currentLine;
    std::ifstream inputFile(argv[1]);
    int count = 0;
    while (getline(inputFile, currentLine))
    {
        //std::cout << currentLine;

        std::istringstream coordString(currentLine);
        float x, y, z;
        if (coordString >> x >> y >> z)
        {
            Cartesian3 v = Cartesian3(x,y,z);

            auto iter = std::find(mesh.begin(), mesh.end(), v);

            if (iter == mesh.end())
            {
                std::cout << "duplicate vertex: " << v << "\n" << std::endl;
                mesh.push_back(v);
                vertexIndices.push_back(mesh.size()-1);
                indexFile << "Vertex " << count << "  " << v << "\n";
                count++;
            }
            else
            {
                vertexIndices.push_back(iter - mesh.begin());
            }
        }

        //indexFile << currentLine + "\n";
    }
    std::cout << vertexIndices.size();
    for (int i = 0; i < vertexIndices.size() / 3; i++)
    {
        indexFile << "Face " << i << "  " << vertexIndices[i*3] << " " << vertexIndices[i*3+1] << " " << vertexIndices[i*3+2] << "\n";
    }


    inputFile.close();
}