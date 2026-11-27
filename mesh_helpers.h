#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "string.h"
#include "Cartesian3.h"
#include <vector>
#include<map>
#include <algorithm>

// Calculates indexed vertices and faces from .tri file
void CreateFaceIndex(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces);

// Exract vertices and faces from .face file
void ParseFaceFile(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces);

// Calculate first directed edge and other half from indexed vertices and faces
void CreateDirEdge(std::vector<Cartesian3> &vertices, std::vector<int> &firstDirectedEdges, std::vector<Cartesian3> &faces, std::vector<int> &otherHalf);

// Rebuild unindexed vertices from vertex index and face arrays
void RebuildTri(std::vector<Cartesian3> &vertexIndices, std::vector<Cartesian3> &faces, std::vector<Cartesian3> &vertices);

// Checks if data passed into this function represents a manifold mesh
bool checkManifold( const std::vector<Cartesian3>& mesh, std::vector<Cartesian3>& faces, const std::vector<int>& firstDirectedEdge,
     const std::vector<int>& otherHalf, std::ostringstream& msg);


int GetEuler(int vertexCount, int faceCount);


int nextEdge(int e);


int EdgeFrom (int e, std::vector<Cartesian3> &faces);


int EdgeTo (int e, std::vector<Cartesian3> &faces);