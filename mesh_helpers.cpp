#include "mesh_helpers.h"


void CreateFaceIndex(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces)
{
    // File initialisation and setup
    
    // File processing
    //std::vector<Cartesian3> mesh;
    std::vector<int> vertexIndices; 
    //std::vector<Cartesian3> faces;
    std::string currentLine;

    std::string name = std::filesystem::path(filename).filename().string();
    if (name[0] == '.')
    {
        std::cerr << "Failed to open " << filename << ", please only use standard .tri files"<< "\n";
        return;
    }

    std::ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open " << filename << "\n";
        return;
    }
    
    int count = 0;
    while (getline(inputFile, currentLine))
    {
        std::istringstream coordString(currentLine);
        float x, y, z;
        if (coordString >> x >> y >> z)
        {
            Cartesian3 v = Cartesian3(x,y,z);

            auto iter = std::find(vertices.begin(), vertices.end(), v);

            if (iter == vertices.end())
            {
                vertices.push_back(v);
                vertexIndices.push_back(vertices.size()-1);
                //indexFile << "Vertex " << count << "  " << v << "\n";
                count++;
            }
            else
            {
                vertexIndices.push_back(iter - vertices.begin());
            }
        }

        //indexFile << currentLine + "\n";
    }
    for (int i = 0; i < vertexIndices.size() / 3; i++)
    {
        //indexFile << "Face " << i << "  " << vertexIndices[i*3] << " " << vertexIndices[i*3+1] << " " << vertexIndices[i*3+2] << "\n";
        faces.push_back(Cartesian3(vertexIndices[i*3], vertexIndices[i*3+1], vertexIndices[i*3+2]));
    }


    inputFile.close();
}


void ParseFaceFile(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces)
{
    std::ifstream inputFile(filename);
    std::string currentLine;
    while (getline(inputFile, currentLine))
    {
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
            else
                otherHalf.push_back(-1);
            //indexFile << "OtherHalf " << e << "  " << edge->second << "\n";
        }
    }
}

void RebuildTri(std::vector<Cartesian3> &vertexIndices, std::vector<Cartesian3> &faces, std::vector<Cartesian3> &vertices)
{
    for (int i = 0; i < faces.size(); i++)
    {
        vertices.push_back(vertexIndices[faces[i].x]);
        vertices.push_back(vertexIndices[faces[i].y]);
        vertices.push_back(vertexIndices[faces[i].z]);
    }
}

