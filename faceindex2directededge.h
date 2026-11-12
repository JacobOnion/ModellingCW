#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string.h"
#include "triangle_renderer/Cartesian3.h"
#include <vector>
#include<map>
#include <algorithm>

// Exract vertices and faces from index file
void ParseFile(std::string filename, std::vector<Cartesian3> &vertices, std::vector<Cartesian3> &faces);

void CreateDirEdge(std::vector<Cartesian3> &vertices, std::vector<int> &firstDirectedEdges, std::vector<Cartesian3> &faces, std::vector<int> &otherHalf);
