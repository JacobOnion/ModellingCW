#include "faceindex2directededge.h"

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

    ParseFile(argv[1], mesh, faces);
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

void ParseFile(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces)
{
    std::ifstream inputFile(filename);
    std::string currentLine;
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
                vertices.push_back(data);
            }

            if (tag == "Face")
            {
                faces.push_back(data);
            }
        }
    }
}


void CreateDirEdge(std::vector<Cartesian3> &vertices, std::vector<int> &firstDirectedEdges, std::vector<Cartesian3> &faces, std::vector<int> &otherHalf)
{       
    //std::vector<int> firstDirectedEdge(vertCount, -1);
    firstDirectedEdges.resize(vertices.size());
    std::fill(firstDirectedEdges.begin(), firstDirectedEdges.end(), -1);

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
                
                if (firstDirectedEdges[from] == -1)
                    firstDirectedEdges[from] = e;
            }
    }


    std::map<std::pair<int,int>, int> edgePairs;

    for (int i = 0; i < faces.size(); i++)
    {
        // write faces to file
        //indexFile << "Face " << i << "  " << faces[i] << "\n";

        // calculate edges
        for (int j = 0; j < 3; j++)
        {
            int e = 3*i + j;
            int from = 0;
            int to = 0;
            if (j == 0) { from= faces[i].z; to = faces[i].x;}
            else if (j == 1){ from = faces[i].x; to = faces[i].y;}
            else { from = faces[i].y; to = faces[i].z;}

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
            if (j == 0) { from= faces[i].z; to = faces[i].x;}
            else if (j == 1){ from = faces[i].x; to = faces[i].y;}
            else { from = faces[i].y; to = faces[i].z;}

            auto edge = edgePairs.find({to, from});
            if (edge != edgePairs.end())
                otherHalf.push_back(edge->second);  
            //indexFile << "OtherHalf " << e << "  " << edge->second << "\n";
        }
    }
}

