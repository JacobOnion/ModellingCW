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
        //std::cout << "FINISHED FACE\n" << std::endl;
        CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);
        //std::cout << "FINISHED DIREDGE\n" << std::endl;
        //reportFile << entry.path().filename() << "\n";

        auto edgeFrom = [&](int e) {
            int v = 0;
            const auto t = faces[e / 3];
            int j = e % 3;
            if (j == 0) { v= t.z;}
                else if (j == 1){ v = t.x;}
                else { v = t.y;}
            return v;
        };

        auto edgeTo = [&](int e) {
            int v = 0;
            const auto t = faces[e / 3];
            int j = e % 3;
            if (j == 0) { v= t.x;}
                else if (j == 1){ v = t.y;}
                else { v = t.z;}
            return v;
        };
    

        // Edge test
        for (int f = 0; f < faces.size(); f++)
        { // iterate through faces
            for (int j = 0; j < 3; j++)
            { // iterate through edges on face
                int e = 3*f+j;
                //int to = 0;
                //int from = 0;
                //if (j == 0) { from= faces[f].z; to = faces[f].x;}
                //else if (j == 1){ from = faces[f].x; to = faces[f].y;}
                //else { from = faces[f].y; to = faces[f].z;}
                if (otherHalf.size() < faces.size() * 3)
                {
                    reportFile << entry.path().filename() << " failed: otherHalf table too short\n";
                    goto next_file;
                }
                
                if (otherHalf[e] < 0)
                {
                    reportFile << entry.path().filename() << "failed: otherHalf not present\n" << std::endl;  
                    goto next_file;
                }
                if (otherHalf[otherHalf[e]] != e)
                {
                    reportFile << entry.path().filename() << "failed: otherHalf edge doesn't match up properly\n" << std::endl;  
                    goto next_file;
                }
                if (edgeFrom(e) != edgeTo(otherHalf[e]) || edgeTo(e) != edgeFrom(otherHalf[e]))
                {
                    reportFile << entry.path().filename() << "failed: u,v in edge did not match v,u in otherHalf\n" << std::endl;  
                    goto next_file;
                }
                
            }
        }
        reportFile << entry.path().filename() << "succeeded: valid manifold mesh!\n" << std::endl;  
        next_file:;

    }
}