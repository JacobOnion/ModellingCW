#include "mesh_helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "Cartesian3.h"
#include <vector>
#include <algorithm>
#include <filesystem>


int main(int argc, char **argv)
{

    //std::string TARGET_DIR = "handout_models";
    if (argc != 2)
    {
        std::cout << "must include relative path to target directory\n";
        return 1;
    }

    std::ofstream reportFile("manifold_test_results.txt");

    for (const auto & entry : std::filesystem::directory_iterator(argv[1]))
    {
        std::cout << "RUNNING LOOPY\n" << std::endl;
        std::vector<Cartesian3> mesh;
        std::vector<Cartesian3> faces;
        std::vector<int> firstDirectedEdge;
        std::vector<int> otherHalf;

        CreateFaceIndex(entry.path(), mesh, faces);
        if (mesh.size() == 0)
        {
            std::cout << "INVALID .tri FILE\n" << std::endl;
            continue;
        }
        CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);


            std::ostringstream msg;
            bool isManifold = checkManifold(mesh, faces,
                 firstDirectedEdge, otherHalf, msg);
            reportFile << entry.path().filename() << msg.str() << std::endl;  

            if (isManifold)
            {
                std::vector<int> faceComp(faces.size(), -1);
                int compNum = 0;
                for (int i = 0; i < faces.size(); i++)
                {
                    if (faceComp[i] >= 0) continue;
                    std::vector<int> stack{(int)i};
                    faceComp[i] = compNum;
                    while ( (!stack.empty()))
                    {
                        int f = stack.back(); stack.pop_back();
                        for (int j = 0; j < 3; j++)
                        {
                            int g = otherHalf[3*f + j] / 3;
                            if (faceComp[g] < 0)
                            {
                                faceComp[g] = compNum;
                                stack.push_back(g);
                            }
                        }
                    }
                    compNum++;
                }
                
                std::vector<int> V(compNum, 0), F(compNum, 0);
                for (int f = 0; f < faces.size(); f++)
                    F[faceComp[f]]++;
                for (int v = 0; v < mesh.size(); v++)
                    V[faceComp[firstDirectedEdge[v] / 3]]++;

                for (int c = 0; c < compNum; c++)
                {
                    int euler = GetEuler(V[c], F[c]);
                    int genus = (2 - euler) / 2;
                    reportFile << "component " << c << ": genus is " << genus << "\n" << std::endl;
                }
            
            }

    }
}