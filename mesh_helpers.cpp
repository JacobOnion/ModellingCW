#include "mesh_helpers.h"


void CreateFaceIndex(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces)
{
    // File initialisation and setup
    
    // File processing
    std::vector<int> vertexIndices; 
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
        }
    }
}


bool checkManifold( const std::vector<Cartesian3>& mesh,
                        std::vector<Cartesian3>& faces,
                        const std::vector<int>& firstDirectedEdge,
                        const std::vector<int>& otherHalf,
                        std::ostringstream& msg)
{
    std::vector<int> deg(mesh.size(), 0);
    int numEdges = 3 * faces.size();

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
    msg << "succeeded: valid manifold mesh!\n";  
    return true;
}


int GetEuler(int vertexCount, int faceCount)
{
    int E = 3 * faceCount / 2;
    return vertexCount - E + faceCount;
}


int GetComponentCount(std::vector<Cartesian3> &faces, std::vector<int> &otherHalf)
{
    // FaceComp stores the component each face belongs to
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
    return compNum;
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


int nextEdge(int e) { return 3 * (e / 3) + (e + 1) % 3; }


int EdgeFrom (int e, std::vector<Cartesian3> &faces) {
    int v = 0;
    const auto t = faces[e / 3];
    int j = e % 3;
    if (j == 0) { v= t.z;}
        else if (j == 1){ v = t.x;}
        else { v = t.y;}
    return v;
};


int EdgeTo (int e, std::vector<Cartesian3> &faces) {
    int v = 0;
    const auto t = faces[e / 3];
    int j = e % 3;
    if (j == 0) { v= t.x;}
        else if (j == 1){ v = t.y;}
        else { v = t.z;}
    return v;
};
