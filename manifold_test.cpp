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

bool checkManifold( const std::vector<Cartesian3>& mesh,
                        std::vector<Cartesian3>& faces,
                        const std::vector<int>& firstDirectedEdge,
                        const std::vector<int>& otherHalf,
                        std::ostringstream& msg)
{
    std::vector<int> deg(mesh.size(), 0);
    int numEdges = 3 * faces.size();
    int genus = 0;
    //std::ostringstream msg;

    //Edge test
    for (int f = 0; f < faces.size(); f++)
    { // iterate through faces
        for (int j = 0; j < 3; j++)
        { // iterate through edges on face
            int e = 3*f+j;
            if (otherHalf.size() < faces.size() * 3)
            {
                msg << "failed: otherHalf table too short\n";
                return false;
            }
            
            if (otherHalf[e] < 0)
            {
                msg << "failed: edge "<< e << " otherHalf not present\n" << std::endl;  
                return false;
            }
            if (otherHalf[otherHalf[e]] != e)
            {
                msg << "failed: edge "<< e <<  " otherHalf edge doesn't match up properly\n" << std::endl;  
                return false;
            }
            if (EdgeFrom(e, faces) != EdgeTo(otherHalf[e], faces) || EdgeTo(e, faces) != EdgeFrom(otherHalf[e], faces))
            {
                msg << "failed: edge "<< e << " u,v in edge did not match v,u in otherHalf\n" << std::endl;  
                return false;
            }
            
        }
    }

    // Vertex test
    for (int i = 0; i < numEdges; i++)
    { // Get degree of each vertex
        deg[EdgeTo(i, faces)]++;
    }
    for (int v = 0; v < mesh.size(); v++)
    {
        int start = firstDirectedEdge[v];
        if (start < 0 || start >= numEdges || EdgeTo(start, faces) != v)
        {
            msg << "failed: vertex " << v <<  " has no valid first directed edge\n";
            return false;
        }

        int steps = 0;
        int edge = start;
        do
        {
            int next =  3 * (edge / 3) + (edge + 1) % 3;
            edge = otherHalf[next];
            steps++;
        } while (edge != start && steps <= numEdges);

        if (steps != deg[v])
        {
            msg << " failed: vertex " << v << " has more than one fan\n";
            return false;
        }
    }
    msg << "succeeded: valid manifold mesh!\n" << std::endl;  
    return true;
}



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

        /*auto edgeFrom = [&](int e) {
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

        std::vector<int> deg(mesh.size(), 0);
        int numEdges = 3 * faces.size();
        int genus = 0;

        //Edge test
        for (int f = 0; f < faces.size(); f++)
        { // iterate through faces
            for (int j = 0; j < 3; j++)
            { // iterate through edges on face
                int e = 3*f+j;
                if (otherHalf.size() < faces.size() * 3)
                {
                    reportFile << entry.path().filename() << " failed: otherHalf table too short\n";
                    goto next_file;
                }
                
                if (otherHalf[e] < 0)
                {
                    reportFile << entry.path().filename() << "failed: edge "<< e << " otherHalf not present\n" << std::endl;  
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

        {
            // Vertex test
            for (int i = 0; i < numEdges; i++)
            { // Get degree of each vertex
                deg[edgeTo(i)]++;
            }
            for (int v = 0; v < mesh.size(); v++)
            {
                int start = firstDirectedEdge[v];
                if (start < 0 || start >= numEdges || edgeTo(start) != v)
                {
                    reportFile << entry.path().filename() << " failed: vertex has no valid first directed edge\n";
                    goto next_file;
                }

                int steps = 0;
                int edge = start;
                do
                {
                    int next =  3 * (edge / 3) + (edge + 1) % 3;
                    edge = otherHalf[next];
                    steps++;
                } while (edge != start && steps <= numEdges);

                if (steps != deg[v])
                {
                    reportFile << entry.path().filename() << " failed: vertex " << v << " has more than one fan\n";
                    goto next_file;
                }
            }*/

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
            
            //reportFile << entry.path().filename() << msg.str() << std::endl;  
            //genus = 1 - ((int)mesh.size() - numEdges/2 + (int)faces.size()) / 2;
        //}

        //next_file:;

    }
}