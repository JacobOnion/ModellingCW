#include "mesh_helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "triangle_renderer/Cartesian3.h"
#include <vector>
#include <algorithm>
#include <filesystem>

int main(int argc, char **argv)
{
    // if (argc != 2)
    // {
    //     std::cout << "must include file" << std::endl;
    // }

    std::string TARGET_DIR = "handout_models/";

        

    // File initialisation and setup
    //std::string inputName = argv[1];

    // size_t pathLen = inputName.find_last_of("/");

    // if (pathLen == std::string::npos)
    // {
    //     pathLen = -1;
    //     std::cout << "no slashes\n" << std::endl;
    // }

    // size_t nameLen = inputName.find(".");


    // if (nameLen == std::string::npos)
    // {
    //     std::cout << "invalid file, must be .tri\n" << std::endl;
    //     return 1;
    // }

    //td::string strippedName = inputName.substr(pathLen + 1, nameLen - (pathLen + 1));




    std::ofstream reportFile("manifold_test_results.txt");

    for (const auto & entry : std::filesystem::directory_iterator(TARGET_DIR))
    {
        std::cout << "RUNNING LOOPY\n" << std::endl;
        std::vector<Cartesian3> mesh;
        std::vector<Cartesian3> faces;
        std::vector<int> firstDirectedEdge;
        std::vector<int> otherHalf;

        //std::cout << "PATH IS:" << entry.path() << "\n" << std::endl;

        CreateFaceIndex(entry.path(), mesh, faces);
        if (mesh.size() == 0)
        {
            std::cout << "INVALID .tri FILE\n" << std::endl;
            continue;
        }
        std::cout << "FINISHED FACE\n" << std::endl;
        CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);
        std::cout << "FINISHED DIREDGE\n" << std::endl;
        reportFile << entry.path().filename() << "\n";

    }
}