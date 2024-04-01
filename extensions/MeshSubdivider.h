#ifndef __MESH_SUBDIVIDER_H
#define __MESH_SUBDIVIDER_H

#include "../src/typedefs.h"
#include "../src/Mesh.h"
#include "../src/utils.h"
#include <vector>
#include "glm/glm.hpp"
#include <map>
#include <algorithm>

namespace MeshSubdivider {
	Mesh* subdivideCatmullClark(Mesh* mesh, int nSubdivisions);

	Vector3 calculateFacePoint(const std::vector<int>& faceVertexIndices, const Mesh* mesh);
	Vector3 calculateEdgePoint(int v1Index, int v2Index, Vector3 f1Point, Vector3 f2Point, const Mesh* mesh);
	std::vector<int> calculateValences(const Mesh* mesh);

	std::pair<int, int> getEdgePair(int v1, int v2);
}

#endif // __MESH_SUBDIVIDER_H