#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "triangle_renderer/Cartesian3.h"
#include <vector>
#include<map>
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
                "# Vertices=8 Faces=12\n" // FIX THIS AT SOME POINT
                "#\n";


    // File processing
    std::vector<Cartesian3> mesh;
    std::vector<int> vertexIndices; 
    std::vector<Cartesian3> faces;
    std::string currentLine;
    std::ifstream inputFile(argv[1]);
    int vertCount = 0;
    while (getline(inputFile, currentLine))
    {
        //std::cout << currentLine;

        std::istringstream coordString(currentLine);
        std::string tag;
        int index;
        float x, y, z;
        if (coordString >> tag >> index >> x >> y >> z)
        {
            Cartesian3 data = Cartesian3(x,y,z);
            if (tag == "Vertex")
            {
                vertCount++;
                indexFile << currentLine << "\n";
            }

            if (tag == "Face")
            {
                faces.push_back(data);
                // for (int i = 0;  i < 3; i++)
                // {
                //     int e = 3 * index + i;
                //     int from = 0;
                //     if (i == 0)
                //         from= data.x;
                //     else if (i == 1)
                //         from = data.y;
                //     else
                //         from = data.z;
                    
                //     if (firstDirectedEdge[from] == -1)
                //         firstDirectedEdge[from] = e;
                // }
            }
        }
    }
        
        std::vector<int> firstDirectedEdge(vertCount, -1);

        for (int f = 0; f < faces.size(); f++)
        {
            for (int i = 0;  i < 3; i++)
                {
                    int e = 3 * f + i;
                    int from = 0;
                    if (i == 0)
                        from= faces[f].x;
                    else if (i == 1)
                        from = faces[f].y;
                    else
                        from = faces[f].z;
                    
                    if (firstDirectedEdge[from] == -1)
                        firstDirectedEdge[from] = e;
                }
        }

        for (int i = 0; i < firstDirectedEdge.size(); i++)
        {
            indexFile << "FirstDirectedEdge " << i << "  " << firstDirectedEdge[i] << "\n";
        }

        std::map<std::pair<int,int>, int> edgePairs;

        for (int i = 0; i < faces.size(); i++)
        {
            // write faces to file
            indexFile << "Face " << i << "  " << faces[i] << "\n";

            // calculate edges
            for (int j = 0; j < 3; j++)
            {
                int e = 3*i + j;
                int from = 0;
                int to = 0;
                if (j == 0) { from= faces[i].x; to = faces[i].y;}
                else if (j == 1){ from = faces[i].y; to = faces[i].z;}
                else { from = faces[i].z; to = faces[i].x;}

                edgePairs[{from, to}] = e;
            }
        }


        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int e = 3*i + j;
                int from = 0;
                int to = 0;
                if (j == 0) { from= faces[i].x; to = faces[i].y;}
                else if (j == 1){ from = faces[i].y; to = faces[i].z;}
                else { from = faces[i].z; to = faces[i].x;}

                auto edge = edgePairs.find({to, from});
                if (edge != edgePairs.end())
                    indexFile << "OtherHalf " << e << "  " << edge->second << "\n";
                else
                {
                    indexFile << "miss\n";
                    std::cout << "miss\n" << std::endl;
                }
                
                std::cout << "hi there\n" << std::endl;

            }
        }


        //indexFile << currentLine + "\n";
    // std::cout << vertexIndices.size();
    // for (int i = 0; i < vertexIndices.size() / 3; i++)
    // {
    //     indexFile << "Face " << i << "  " << vertexIndices[i*3] << " " << vertexIndices[i*3+1] << " " << vertexIndices[i*3+2] << "\n";
    // }


    inputFile.close();
}