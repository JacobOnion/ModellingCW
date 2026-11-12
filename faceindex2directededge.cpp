#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "triangle_renderer/Cartesian3.h"
#include <vector>
#include<map>
#include <algorithm>

#include "mesh_helpers.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "must include file" << std::endl;
    }

    std::vector<Cartesian3> mesh;
    std::vector<int> firstDirectedEdge;
    std::vector<Cartesian3> faces;
    std::vector<int> otherHalf;

    ParseFaceFile(argv[1], mesh, faces);
    CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);


    std::string inputName = argv[1];

    size_t pathLen = inputName.find_last_of("/");

    if (pathLen == std::string::npos)
    {
        pathLen = -1;
        std::cout << "no slashes\n" << std::endl;
    }

    size_t nameLen = inputName.find(".");


    if (nameLen == std::string::npos)
    {
        std::cout << "invalid file, must be .face\n" << std::endl;
        return 1;
    }

    std::string strippedName = inputName.substr(pathLen + 1, nameLen - (pathLen + 1));

    std::ofstream indexFile(strippedName + ".diredge");
    
    indexFile << "# University of Leeds 2026-2027\n"
                "# COMP 5812M Assignment 1\n"
                "# Jacob Onion\n"
                "# 201710645\n"
                "#\n"
                "# Object Name: " + strippedName + "\n"
                "# Vertices=" << mesh.size() << " Faces=" << faces.size() << "\n" // FIX THIS AT SOME POINT
                "#\n";

    for (int i = 0; i < mesh.size(); i++)
    {
        indexFile << "Vertex " << i << "  " << mesh[i] << "\n";
    }


    for (int i = 0; i < firstDirectedEdge.size(); i++)
    {
        indexFile << "FirstDirectedEdge " << i << "  " << firstDirectedEdge[i] << "\n";
    }

    for (int i = 0; i < faces.size(); i++)
    {
        indexFile << "Face " << i << "  " << faces[i] << "\n";
    }

    for (int i = 0; i < otherHalf.size(); i++)
    {
        indexFile << "OtherHalf " << i << "  " << otherHalf[i] << "\n";
    }

    return 0;
}

