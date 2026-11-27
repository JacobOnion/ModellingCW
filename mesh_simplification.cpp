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

const int MAX_ITERATIONS = 8;

const int SAFETY_CAP = 5000;



// Standard greedy mesh simplification
void SimplifyMesh(std::vector<Cartesian3> &mesh, std::vector<int> &firstDirectedEdge, std::vector<Cartesian3> &faces, std::vector<int> &otherHalf, int iterations)
{
    // Vertices still present in mesh
    std::vector<bool> aliveVertices = std::vector<bool>(mesh.size(), true);
    // Vertices that can be editted without breaking manifold
    std::vector<bool> validVertices = std::vector<bool>(mesh.size(), true);
    // Number of vertices still alive
    int vertexCount = mesh.size();
    std::cout << "filled arrays\n" << std::endl;

    // Returns guassian curvature of given vertex
    auto guassianCurvature = [&](int v) {
        float angleSum = 0;
        float areaSum = 0; // maybe needed?
        int e0 = nextEdge(firstDirectedEdge[v]);
        int e = e0;
        int count = 0;
        do
        {
            // check if vertex is still alive before using it in calculations
            if (aliveVertices[EdgeTo(e, faces)] && aliveVertices[EdgeTo(nextEdge(e), faces)])
            {
                Cartesian3 e1 = mesh[EdgeTo(e, faces)] - mesh[v];
                Cartesian3 e2 = mesh[EdgeTo(nextEdge(e), faces)] - mesh[v];
            

                angleSum += acos( e1.dot(e2) / (e1.length() * e2.length()) );
                areaSum += 0.5f * (e1.cross(e2)).length() / 3.f;
            }
            
            e = nextEdge(otherHalf[e]);
            count ++;
            if (count >= SAFETY_CAP)
                std::cout << "curvature calculation loop broken. count is: " << count <<"\n";
        }
        while (e != e0 && count <= SAFETY_CAP);
        return (2.f*M_PI - angleSum) / areaSum;
    };


    // Removes v and repairs remaining mesh
    auto removeVertex = [&](int v, std::vector<int> &ring)
    {
        ring.clear();
        int e0 = nextEdge(firstDirectedEdge[v]);
        int e = e0;
        int count = 0;
        // Collect the vertices in a ring around the one we are removing
        do
        {   int vert = EdgeTo(e, faces);
            if (aliveVertices[vert])
                ring.push_back(vert);
            e = nextEdge(otherHalf[e]);

            if (count >= SAFETY_CAP)
                std::cout << "face correction loop broken\n";
            count ++;
        } while (e != e0 && count <= SAFETY_CAP);

        // selecting apex vertex
        int apex = -1;
        for (int i = 0; i < ring.size() && apex < 0; i++)
        {
            bool suitable = true;
            for (int j = 2; j < ring.size()-1; j++)
            {
                int b = ring[(i+j) % ring.size()];
                int e0 = nextEdge(firstDirectedEdge[ring[i]]);
                int e = e0;
                int count = 0;
                do {
                    if (EdgeTo(e, faces) == b)
                        suitable = false; // edge exists
                    e = nextEdge(otherHalf[e]);

                    count ++;
            if (count >= SAFETY_CAP)
                std::cout << "curvature calculation loop broken. count is: " << count <<"\n";
                } while ( e != e0 && count <= SAFETY_CAP);
            }
            if (suitable)
                    apex = i;
        }
        if (apex < 0)
        {
            std::cout << "no suitable apex vertex, all vertices would result in duplicate edges\n";
            return false;
        }

        // rotate ring so apex element in first in array
        std::rotate(ring.begin(), ring.begin() + apex, ring.end());

        ///mesh.erase(mesh.begin() + v);

        // Delete faces that involve vertex
        std::vector<Cartesian3> newFaces;
        for (int f = 0; f < faces.size(); f++)
        {
            //std::cout << faces[f].x << faces[f].y << faces[f].z << v << "\n";

            if (faces[f].x != v && faces[f].y != v && faces[f].z != v)
            {
                newFaces.push_back(faces[f]);
            }
        }

        // Link up affected vertices
        for (int i = 1; i+1 < ring.size(); i++)
        {
            newFaces.push_back(Cartesian3(ring[0], ring[i+1], ring[i]));
        }

        // Checking if mesh changes will violate eularian condition
        int vCount = vertexCount-1;
        int fCount = newFaces.size();
        int x = GetEuler(vCount, fCount);
        if (x > 2)
        {
            std::cout << "non euler if this vertex is removed. vertex count is " << vCount << ", mesh size is" << mesh.size() <<"\n";
           return false;
        }
        //Delete vertex
        aliveVertices[v] = false;

        // Overwrite faces vector with new face data
        faces = newFaces;

        // Recompute edge data
        firstDirectedEdge.clear();
        otherHalf.clear();
        CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);
        return true;
    }; 
    

    // Populate initial vertex curvatures
    std::vector<double> curvatures;
    for (int v = 0; v < mesh.size(); v++)
    {
        curvatures.push_back(guassianCurvature(v));
    }

    // Run greedy simplification algorithm
    for (int i = 0; i < iterations; i++)
    {
        if (vertexCount <= 4)
        {
            std::cout << "minimum mesh size reached, ending at iteration " << i <<"\n";
            return;
        }
        //std::cout << "iteration: " << i << std::endl;
        std::vector<int> vertexRing;
        bool iterationComplete = false;
        int verticesChecked = 0;
        int targetVert = -1;
        while (iterationComplete == false)
        {
            // locate vertex with lowest curvature
            targetVert = -1;
            for (int v = 0; v < curvatures.size(); v++)
            {
                if (aliveVertices[v] && validVertices[v] && (targetVert < 0 
                    || fabs(curvatures[v]) < fabs(curvatures[targetVert])))
                {
                    targetVert = v;
                }
                    
            }

            if (targetVert < 0)
            {
                std::cout << "no removable vertices left, ending at iteration " << i << "\n";
                return;
            }
            //std::cout << "target after selection: " << targetVert << "\n";
            
            // Remove target vertex and build ring of vertices around it
            if (removeVertex(targetVert, vertexRing))
            {
                //std::cout << "successfully removed vertex\n";
                iterationComplete = true;
            }
            else
            {
                std::cout << "invalid vertex: " << targetVert << "\n";
                validVertices[targetVert] = false;
                verticesChecked++;
            }
        }
        if (verticesChecked >= vertexCount)
        {
            std::cout << "iterations " << i << "\n";
            std::cout << "futher simplification would break manifold properties\n";
            return;
        }

        vertexCount--;

        // Recompute curvatures around the deleted vertex
        for (int v = 0; v < vertexRing.size(); v++)
        {
            curvatures[vertexRing[v]] = guassianCurvature(vertexRing[v]);
            validVertices[vertexRing[v]] = true;
        }
        if (i % 100 == 0)
            std::cout << "iteration " << i << "\n";

    }
    std::cout << "iterations " << iterations << "\n";

}



int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "must include file and number of iterations" << std::endl;
        return 1;
    }
    int iterations = std::stoi(argv[2]);

    //std::string TARGET_DIR = "handout_models/";

    //File initialisation and setup
    std::string inputName = argv[1];

    size_t pathLen = inputName.find_last_of("/");

    if (pathLen == std::string::npos)
    {
        pathLen = -1;
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
    CreateDirEdge(mesh, firstDirectedEdge, faces, otherHalf);


    // Check if provided mesh is manifold
    std::ostringstream msg;
    bool isManifold = checkManifold(mesh, faces, firstDirectedEdge, otherHalf, msg);
    if (!isManifold)
    {
        std::cout << "mesh simplification can only run on manifold meshes. Reason for non-manifold mesh: " << msg.str() << "\n";
        return 1;
    }

    // Run mesh simplification
    SimplifyMesh(mesh, firstDirectedEdge, faces, otherHalf, iterations);



    // Rebuild the .tri file with new mesh data
    std::vector<Cartesian3> vertices;

    RebuildTri(mesh, faces, vertices);

    repairedFile << faces.size() << "\n";
    
    for (auto v : vertices)
    {
        repairedFile << v << "\n";
    }

    std::cout << "Simplification complete!\n\n";
}
