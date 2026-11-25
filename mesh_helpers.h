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

int next(int e) { return 3 * (e / 3) + (e + 1) % 3; }


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