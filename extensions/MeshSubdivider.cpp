#include "MeshSubdivider.h"

Mesh* MeshSubdivider::subdivideCatmullClark(Mesh* mesh, int nSubdivisions) {
	Mesh* newMesh = new Mesh(mesh);
	
	// For now, we only subdivide the vertices and faces. We will add support for normals and textures later.
	// std::vector<Vector3> normals = mesh->normals;
	// std::vector<Texture> texCoords = mesh->textures;

	for (int i = 0; i < nSubdivisions; i++) {
		/* STEPS:
			0. Caching
			1. Compute the face points
			2. Compute the edge points
			3. Compute the new vertices
			4. Reconstruct the mesh with the new vertices and faces
			5. Update the mesh with the new vertices and faces
		*/

		// Cache the old vertices and faces
		std::vector<Vector3> oldVertices = newMesh->vertices;
		std::vector<Triangle> oldFaces = newMesh->faces;
		std::vector<std::pair<int, int>> oldEdges;
		for (int i = 0; i < oldFaces.size(); i++) {
			std::vector<int> vertices;
			
		}

		std::vector<int> valences = calculateValences(newMesh);

		// Calculate incident faces for each vertex in a map
		std::map<int, std::vector<int>> incidentFaces;

		// Calculate incident edges for each vertex in a map
		std::map<int, std::vector<std::pair<int, int>>> incidentEdges;

		//Calculate indicent faces for each edges in a map
		std::map<std::pair<int, int>, std::vector<int>> incidentFacesForEdges;
		for (int i = 0; i < oldFaces.size(); i++) 
		{	
			// VERTICES
			incidentFaces[oldFaces[i].vIndex[0]].push_back(i);
			incidentFaces[oldFaces[i].vIndex[1]].push_back(i);
			incidentFaces[oldFaces[i].vIndex[2]].push_back(i);

			// Get the edges of the face
			std::pair<int, int> edge1 = getEdgePair(oldFaces[i].vIndex[0], oldFaces[i].vIndex[1]);
			std::pair<int, int> edge2 = getEdgePair(oldFaces[i].vIndex[1], oldFaces[i].vIndex[2]);
			std::pair<int, int> edge3 = getEdgePair(oldFaces[i].vIndex[2], oldFaces[i].vIndex[0]);

			// INCIDENT EDGES FOR VERTICES
			incidentEdges[oldFaces[i].vIndex[0]].push_back(edge1);
			incidentEdges[oldFaces[i].vIndex[0]].push_back(edge3);

			incidentEdges[oldFaces[i].vIndex[1]].push_back(edge1);
			incidentEdges[oldFaces[i].vIndex[1]].push_back(edge2);

			incidentEdges[oldFaces[i].vIndex[2]].push_back(edge2);
			incidentEdges[oldFaces[i].vIndex[2]].push_back(edge3);
			
			// INCIDENT FACES FOR EDGES

			incidentFacesForEdges[edge1].push_back(i);
			incidentFacesForEdges[edge2].push_back(i);
			incidentFacesForEdges[edge3].push_back(i);
		}

		// 1. Compute the face points
		std::vector<Vector3> facePoints;
		for (int i = 0; i < oldFaces.size(); i++) {
			Vector3 facePoint = calculateFacePoint({ oldFaces[i].vIndex[0], oldFaces[i].vIndex[1], oldFaces[i].vIndex[2] }, newMesh);
			facePoints.push_back(facePoint);
			// // Debug face points
			// std::cout << "Face point " << i << ": ";
			// printVector3(facePoint);
		}

		
		// 2. Compute the edge points
		std::vector<Vector3> edgePoints;
		for (int i = 0; i < oldEdges.size(); i++) {
			Vector3 incidentFace1 = facePoints[incidentFacesForEdges[oldEdges[i]][0]];
			Vector3 incidentFace2 = facePoints[incidentFacesForEdges[oldEdges[i]][1]];
			int v1 = oldEdges[i].first;
			int v2 = oldEdges[i].second;
			Vector3 edgePoint = calculateEdgePoint(v1, v2, incidentFace1, incidentFace2, newMesh);
			edgePoints.push_back(edgePoint);
			// // Debug edge points
			// std::cout << "Edge point " << i << ": ";
			// printVector3(edgePoint);
		}
		

		// 3. Compute the new vertices
		/* Notes:
			-> For each vertex, we must find the average of the face points and edge points adjacent to the old vertex, and the old vertex itself.
			-> Formula:
				New vertex = (Q + 2R + S(n - 3)) / n

				Where:
				-> Q: average of the new face points of all faces adjacent to the old vertex.
				-> R: average of the midpoints of all old edges incident on the old vertex.
				-> S: the old vertex.
				-> n: number of faces adjacent to the old vertex. (valence of the vertex)

			-> After computing the new vertices, new edges formed by:
				- Connecting each new face point to the new edge points of the edges defining the old face.
				- Connecting each new vertex point tot he new edge points of all old edges incident on the old vertex.
		*/
		

		// 4. Reconstruct the mesh with the new vertices and faces
		
		// 5. Update the mesh with the new vertices and faces

		// Update the mesh
		newMesh->UpdateMesh();

		// Debug new vertices
		std::cout << "New vertices: " << std::endl;
		for (int i = 0; i < newMesh->vertices.size(); i++) {
			printVector3(newMesh->vertices[i]);
		}

	}
	return newMesh;
}

Vector3 MeshSubdivider::calculateFacePoint(const std::vector<int>& faceVertexIndices, const Mesh* mesh) {
	Vector3 facePoint = Vector3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < faceVertexIndices.size(); i++) {
		facePoint += mesh->vertices[faceVertexIndices[i]];
	}
	facePoint /= faceVertexIndices.size();
	return facePoint;
}

Vector3 MeshSubdivider::calculateEdgePoint(int v1Index, int v2Index, Vector3 f1Point, Vector3 f2Point, const Mesh* mesh)
{
	Vector3 edgePoint = Vector3(0.0f, 0.0f, 0.0f);
	edgePoint = (mesh->vertices[v1Index] + mesh->vertices[v2Index] + f1Point + f2Point) / 4.0f;

	return edgePoint;
}

std::vector<int> MeshSubdivider::calculateValences(const Mesh* mesh) {
	std::vector<int> valences(mesh->vertices.size(), 0);
	for (int i = 0; i < mesh->faces.size(); i++) {
		valences[mesh->faces[i].vIndex[0]]++;
		valences[mesh->faces[i].vIndex[1]]++;
		valences[mesh->faces[i].vIndex[2]]++;
	}
	return valences;
}

std::pair<int, int> MeshSubdivider::getEdgePair(int v1, int v2) {
	return std::make_pair(std::min(v1, v2), std::max(v1, v2));
}