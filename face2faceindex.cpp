#include "face2faceindex.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "must include file" << std::endl;
    }

    std::vector<Cartesian3> mesh;
    std::vector<Cartesian3> faces;

    CreateFaceIndex(argv[1], mesh, faces);

    // File processing
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

    std::ofstream indexFile(strippedName + ".face");
    
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

    for (int i = 0; i < faces.size(); i++)
    {
        indexFile << "Face " << i << "  " << faces[i] << "\n";
    }

    return 0;
}

void CreateFaceIndex(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces)
{
    // File initialisation and setup
    
    // File processing
    //std::vector<Cartesian3> mesh;
    std::vector<int> vertexIndices; 
    //std::vector<Cartesian3> faces;
    std::string currentLine;
    std::ifstream inputFile(filename);
    int count = 0;
    while (getline(inputFile, currentLine))
    {
        //std::cout << currentLine;

        std::istringstream coordString(currentLine);
        float x, y, z;
        if (coordString >> x >> y >> z)
        {
            Cartesian3 v = Cartesian3(x,y,z);

            auto iter = std::find(vertices.begin(), vertices.end(), v);

            if (iter == vertices.end())
            {
                std::cout << "duplicate vertex: " << v << "\n" << std::endl;
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
    std::cout << vertexIndices.size();
    for (int i = 0; i < vertexIndices.size() / 3; i++)
    {
        //indexFile << "Face " << i << "  " << vertexIndices[i*3] << " " << vertexIndices[i*3+1] << " " << vertexIndices[i*3+2] << "\n";
        faces.push_back(Cartesian3(vertexIndices[i*3], vertexIndices[i*3+1], vertexIndices[i*3+2]));
    }


    inputFile.close();
}