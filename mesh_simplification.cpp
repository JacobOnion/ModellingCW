#include "mesh_helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "Cartesian3.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <filesystem>

const int MAX_ITERATIONS = 1000;



int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "must include file" << std::endl;
        return 1;
    }

    //std::string TARGET_DIR = "handout_models/";

    //File initialisation and setup
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
        std::cout << "invalid file, must be .tri\n" << std::endl;
        return 1;
    }

    std::string strippedName = inputName.substr(pathLen + 1, nameLen - (pathLen + 1));



    std::ofstream repairedFile(strippedName + "_simplified.tri");


    std::vector<Cartesian3> mesh;
    std::vector<Cartesian3> faces;
    std::vector<int> firstDirectedEdge;
    std::vector<int> otherHalf;

    CreateFaceIndex(argv[1], mesh, faces);

    if (mesh.size() == 0)
    {
        std::cout << "INVALID .tri FILE\n" << std::endl;
        return 1;
    }
    //std::cout << "FINISHED FACE\n" << std::endl;
    CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);

    std::cout << "filled arrays\n" << std::endl;


    auto guassianCurvature = [&](int v) {
        float angleSum = 0;
        float areaSum = 0; // maybe needed?
        int e0 = firstDirectedEdge[v];
        int e = e0;
        do
        {
            Cartesian3 e1 = mesh[EdgeTo(e, faces)] - mesh[v];
            Cartesian3 e2 = mesh[EdgeTo(next(e), faces)] - mesh[v];
            

            angleSum += acos( e1.dot(e2) / (e1.length() * e2.length()) );
            areaSum += 0.5f * (e1.cross(e2)).length() / 3.f;

            e = next(otherHalf[e]);
        }
        while (e != e0);
        return (2.f*M_PI - angleSum) / areaSum;
    };


    std::vector<float> vertexCurvatures;
    // Curvature per vertex
    for (int v = 0; v < mesh.size(); v++)
    {
        float angleSum = 0;
        float areaSum = 0; // maybe needed?
        for (int f = 0; f < faces.size(); f++)
        {
            Cartesian3 e1;
            Cartesian3 e2;
            bool connected = false;
            if (v == faces[f].x)
            {
                e1 = mesh[faces[f].y] - mesh[v];
                e2 = mesh[faces[f].z] - mesh[v];
                connected = true;
            }
            else if (v == faces[f].y)
            {
                e1 = mesh[faces[f].x] - mesh[v];
                e2 = mesh[faces[f].z] - mesh[v];
                connected = true;
            }
            else if (v == faces[f].z)
            {
                e1 = mesh[faces[f].x] - mesh[v];
                e2 = mesh[faces[f].y] - mesh[v];
                connected = true;
            }
            //else
                //std::cout <<"not connected\n";

            if (connected)
            {
                angleSum += acos( e1.dot(e2) / (e1.length() * e2.length()) );
                areaSum += 0.5f * (e1.cross(e2)).length() / 3.f;
                connected = false;
            }
        }
        float deficit = (2.f*M_PI - angleSum) / areaSum;
        vertexCurvatures.push_back(deficit);
        std::cout << " vertex " << v << " : deficit is " << deficit << "\n";
    }
    //for (int j = 0; j < faces.size(); j++)
    //        std::cout << "face " << j <<": " << faces[j] << "\n" << std::endl;
    std::cout << Cartesian3(3, 4, 0).length() << "\n";

    // Sort vector for effiecient element lookup
    std::sort(vertexCurvatures.begin(), vertexCurvatures.end(), [](const float& a, const float& b) {
    return a > b;
    });

    float currentVert = vertexCurvatures[vertexCurvatures.size()-1];
    vertexCurvatures.pop_back();
}