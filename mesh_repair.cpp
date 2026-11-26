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

const int MAX_ITERATIONS = 1000;

struct BoundaryEdge
{
    int index;
    bool visited;
};



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



    std::ofstream repairedFile(strippedName + "_repaired.tri");


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
    
    std::vector<BoundaryEdge> boundaryEdges;
    for (int e = 0; e < otherHalf.size(); e++)
    {
        if (otherHalf[e] == -1)
            boundaryEdges.push_back({e, false}); // needs work
    }

    // Sort vector for effiecient element lookup
    std::sort(boundaryEdges.begin(), boundaryEdges.end(), [](const BoundaryEdge& a, const BoundaryEdge& b) {
    return a.index < b.index;
});

    // Check if element with specified value has been visited, true if so, otherwise marks it as visited and returns false
    auto markVisited = [&] (int i) 
    { 
        auto it = std::lower_bound(boundaryEdges.begin(), boundaryEdges.end(), i, [](const BoundaryEdge& edge, int value) {
            return edge.index < value;
        });
        
        if (it != boundaryEdges.end() && it->index == i)
        {
            if (it->visited)
                return true;
            
            it->visited = true;
            return false;
        }
        return false;
    };

    for (int i = 0; i < boundaryEdges.size(); i++)
    {
        //std::cout << i << "edge\n" << std::endl;

        int edgeIndex = boundaryEdges[i].index;
        int count = 0;
        std::vector<int> loopVertices;

        if (boundaryEdges[i].visited)
            continue;
        

        int e = edgeIndex;

        // Iterate through boundary edges to form a loop around the hole
        do
        {
            markVisited(e);
            loopVertices.push_back(EdgeFrom(e, faces));
            e = nextEdge(e);
            int steps = 0;
            while (otherHalf[e] != -1)
            {
                e = nextEdge(otherHalf[e]);
                steps++;
                if (steps > otherHalf.size())
                {
                    std::cout << "invalid hole\n" << std::endl;
                    break;
                }
            }
            count++;
            //std::cout << "count is " << count << "\n" << std::endl;
        }
        while (e != edgeIndex && count < MAX_ITERATIONS);

        if (count >= MAX_ITERATIONS)
            std::cout << "MAX ITERATIONS REACHED\n" << std::endl;

        // Calculate centre of gravity for current hole
        float x = 0;
        float y = 0;
        float z = 0;
        for (int v = 0; v < loopVertices.size(); v++)
        {
            x += mesh[loopVertices[v]].x;
            y += mesh[loopVertices[v]].y;
            z += mesh[loopVertices[v]].z;
        }
        Cartesian3 centre = Cartesian3(x/loopVertices.size(), y/loopVertices.size(), z/loopVertices.size());

        // Add new vertex to mesh
        mesh.push_back(centre);

        // Add new triangles to fill hole
        for (int v = 0; v < loopVertices.size(); v++)
        {
            faces.push_back(Cartesian3(loopVertices[(v+1) % loopVertices.size()],
             loopVertices[v], mesh.size()-1));
        }
    }

    std::vector<Cartesian3> vertices;

    RebuildTri(mesh, faces, vertices);

    repairedFile << faces.size() << "\n";
    
    for (auto v : vertices)
    {
        repairedFile << v << "\n";
    }

    std::cout << "Repair Complete!\n" << std::endl;
    
}