#include "MeshSubdivider.h"

Mesh* MeshSubdivider::subdivideCatmullClark(Mesh* mesh, int nSubdivisions) {
	Mesh* newMesh = new Mesh(mesh);

	// This will divide any topological mesh into a quad mesh.
	
	// For now, we only subdivide the vertices and faces. We will add support for normals and textures later.
	// std::vector<Vector3> normals = mesh->normals;
	// std::vector<Texture> texCoords = mesh->textures;

	if (!mesh->quadMesh) { // Triangle topology
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
			std::vector<Triangle> oldTriangles = newMesh->triangles;
			std::vector<std::pair<int, int>> oldEdges;

			std::map<int, std::vector<int>> incidentFaces;
			std::map<int, std::vector<std::pair<int, int>>> incidentEdges;
			std::map<std::pair<int, int>, std::vector<int>> incidentFacesForEdges;
			std::map<std::pair<int, int>, Vector3> edgeMidpoints;
			std::vector<Vector3> facePoints;
			for (int i = 0; i < oldTriangles.size(); i++) 
			{	
				// Any quad has 4 edges
				std::pair<int, int> edge1 = getEdgePair(oldTriangles[i].vIndex[0], oldTriangles[i].vIndex[1]);
				std::pair<int, int> edge2 = getEdgePair(oldTriangles[i].vIndex[1], oldTriangles[i].vIndex[2]);
				std::pair<int, int> edge3 = getEdgePair(oldTriangles[i].vIndex[2], oldTriangles[i].vIndex[3]);

				if (std::find(oldEdges.begin(), oldEdges.end(), edge1) == oldEdges.end()) {
					oldEdges.push_back(edge1);
				}
				if (std::find(oldEdges.begin(), oldEdges.end(), edge2) == oldEdges.end()) {
					oldEdges.push_back(edge2);
				}
				if (std::find(oldEdges.begin(), oldEdges.end(), edge3) == oldEdges.end()) {
					oldEdges.push_back(edge3);
				}

				// VERTICES
				incidentFaces[oldTriangles[i].vIndex[0]].push_back(i);
				incidentFaces[oldTriangles[i].vIndex[1]].push_back(i);
				incidentFaces[oldTriangles[i].vIndex[2]].push_back(i);

				// MIDPOINTS OF EDGES
				edgeMidpoints[edge1] = (oldVertices[edge1.first] + oldVertices[edge1.second]) / 2.0f;
				edgeMidpoints[edge2] = (oldVertices[edge2.first] + oldVertices[edge2.second]) / 2.0f;
				edgeMidpoints[edge3] = (oldVertices[edge3.first] + oldVertices[edge3.second]) / 2.0f;

				// INCIDENT EDGES FOR VERTICES
				std::vector<std::pair<int, int>> incidentEdgesForVertex0 = incidentEdges[oldTriangles[i].vIndex[0]];
				std::vector<std::pair<int, int>> incidentEdgesForVertex1 = incidentEdges[oldTriangles[i].vIndex[1]];
				std::vector<std::pair<int, int>> incidentEdgesForVertex2 = incidentEdges[oldTriangles[i].vIndex[2]];
				if (std::find(incidentEdgesForVertex0.begin(), incidentEdgesForVertex0.end(), edge1) == incidentEdgesForVertex0.end()) {
					incidentEdges[oldTriangles[i].vIndex[0]].push_back(edge1);
				}
				if (std::find(incidentEdgesForVertex0.begin(), incidentEdgesForVertex0.end(), edge3) == incidentEdgesForVertex0.end()) {
					incidentEdges[oldTriangles[i].vIndex[0]].push_back(edge3);
				}
				if (std::find(incidentEdgesForVertex1.begin(), incidentEdgesForVertex1.end(), edge1) == incidentEdgesForVertex1.end()) {
					incidentEdges[oldTriangles[i].vIndex[1]].push_back(edge1);
				}
				if (std::find(incidentEdgesForVertex1.begin(), incidentEdgesForVertex1.end(), edge2) == incidentEdgesForVertex1.end()) {
					incidentEdges[oldTriangles[i].vIndex[1]].push_back(edge2);
				}
				if (std::find(incidentEdgesForVertex2.begin(), incidentEdgesForVertex2.end(), edge2) == incidentEdgesForVertex2.end()) {
					incidentEdges[oldTriangles[i].vIndex[2]].push_back(edge2);
				}
				if (std::find(incidentEdgesForVertex2.begin(), incidentEdgesForVertex2.end(), edge3) == incidentEdgesForVertex2.end()) {
					incidentEdges[oldTriangles[i].vIndex[2]].push_back(edge3);
				}
				
				// INCIDENT FACES FOR EDGES
				incidentFacesForEdges[edge1].push_back(i);
				incidentFacesForEdges[edge2].push_back(i);
				incidentFacesForEdges[edge3].push_back(i);

				// Face points
				Vector3 facePoint = calculateFacePoint({oldTriangles[i].vIndex[0], oldTriangles[i].vIndex[1], oldTriangles[i].vIndex[2]}, newMesh);
				facePoints.push_back(facePoint);
			}

			// 2. Compute the edge points
			std::vector<Vector3> edgePoints;
			for (int i = 0; i < oldEdges.size(); i++) {
				Vector3 incidentFace1 = facePoints[incidentFacesForEdges[oldEdges[i]][0]];
				Vector3 incidentFace2 = facePoints[incidentFacesForEdges[oldEdges[i]][1]];
				Vector3 edgeMidpoint2 = edgeMidpoints[oldEdges[i]] * 2.0f;
				Vector3 edgePoint = (incidentFace1 + incidentFace2 + edgeMidpoint2) / 4.0f;
				edgePoints.push_back(edgePoint);
			}
			

			// 3. Compute the new vertices
			/* Notes:
				-> For each vertex, we must find the average of the face points and edge points adjacent to the old vertex, and the old vertex itself.
				-> Formula:
					New vertex = (Q + 2R + S(n - 3)) / n

					Where:
					-> Q: average of the new face points of all faces adjacent to the old vertex.
					-> R: average of the midpoints of all old edges incident on the old vertex. NOT the new edge points.
					-> S: the old vertex.
					-> n: number of faces adjacent to the old vertex. (valence of the vertex)

				-> After computing the new vertices, new edges formed by:
					- Connecting each new face point to the new edge points of the edges defining the old face.
					- Connecting each new vertex point tot he new edge points of all old edges incident on the old vertex.
			*/
			for (int i = 0; i < oldVertices.size(); i++) {
				Vector3 avg_face_points = Vector3(0.0f, 0.0f, 0.0f);
				Vector3 avg_mid_edges = Vector3(0.0f, 0.0f, 0.0f);
				Vector3 old_coords = oldVertices[i];
				std::vector<int> incidentFaceIndices = incidentFaces[i];
				int n = incidentFaceIndices.size(); // valence of the vertex
				std::vector<std::pair<int, int>> incidentEdgeIndices = incidentEdges[i];

				// Both can be done in a single loop because the number of faces and edges adjacent to a vertex are the same.
				for (int j = 0; j < n; j++) {
					// New face points
					avg_face_points += facePoints[incidentFaceIndices[j]];

					// Midpoint of the old edge
					avg_mid_edges += edgeMidpoints[incidentEdgeIndices[j]];
				}
				avg_face_points /= n;
				avg_mid_edges /= n;

				float m1 = 1.0f / n;
				float m2 = 2.0f / n;
				float m3 = (n - 3.0f) / n;

				Vector3 newVertex = m1 * avg_face_points + m2 * avg_mid_edges + m3 * old_coords;

				newMesh->vertices[i] = newVertex;
			}

			// Add all edge points to the new vertices, then store the indices of the edge points in a map for later use.
			std::map<std::pair<int, int>, int> edgePointIndices;
			for (int i = 0; i < edgePoints.size(); i++) {
				newMesh->vertices.push_back(edgePoints[i]);
				edgePointIndices[oldEdges[i]] = newMesh->vertices.size() - 1;
			}

			// 4. Reconstruct the mesh with the new vertices and faces
			std::vector<Triangle> newTriangles;
			for (int i = 0; i < oldTriangles.size(); i++)
			{
				Vector3 facePoint = facePoints[i];
				newMesh->vertices.push_back(facePoint);
				int vFIndex = newMesh->vertices.size() - 1;
				

				// Get the incident edge points
				std::pair<int, int> edge1 = getEdgePair(oldTriangles[i].vIndex[0], oldTriangles[i].vIndex[1]);
				std::pair<int, int> edge2 = getEdgePair(oldTriangles[i].vIndex[1], oldTriangles[i].vIndex[2]);
				std::pair<int, int> edge3 = getEdgePair(oldTriangles[i].vIndex[2], oldTriangles[i].vIndex[3]);

				int vE01Index = edgePointIndices[edge1];
				int vE12Index = edgePointIndices[edge2];
				int vE23Index = edgePointIndices[edge3];

				// Get vertex indices
				int v0Index = oldTriangles[i].vIndex[0];
				int v1Index = oldTriangles[i].vIndex[1];
				int v2Index = oldTriangles[i].vIndex[2];

				// Add the new faces
				newTriangles.push_back(Triangle({ vFIndex, v0Index, vE01Index }));
				newTriangles.push_back(Triangle({ vFIndex, vE01Index, v1Index }));
				newTriangles.push_back(Triangle({ vFIndex, v1Index, vE12Index }));
				newTriangles.push_back(Triangle({ vFIndex, vE12Index, v2Index }));
				newTriangles.push_back(Triangle({ vFIndex, v2Index, vE23Index }));
				newTriangles.push_back(Triangle({ vFIndex, vE23Index, v0Index }));
			}

			newMesh->triangles = newTriangles;
			newMesh->UpdateMesh();

			// Clear caches
			incidentFaces.clear();
			incidentEdges.clear();
			incidentFacesForEdges.clear();
			edgeMidpoints.clear();
			facePoints.clear();
			oldEdges.clear();
			newTriangles.clear();

			return newMesh;
		}
	}
	else // Quad topology
	{
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
			std::vector<Quad> oldQuads = newMesh->quads;
			std::vector<std::pair<int, int>> oldEdges;

			// Calculate incident faces for each vertex in a map
			std::map<int, std::vector<int>> incidentFaces;

			// Calculate incident edges for each vertex in a map
			std::map<int, std::vector<std::pair<int, int>>> incidentEdges;

			//Calculate indicent faces for each edges in a map
			std::map<std::pair<int, int>, std::vector<int>> incidentFacesForEdges;

			// Calculate midpoints of edges in a map
			std::map<std::pair<int, int>, Vector3> edgeMidpoints;

			for (int i = 0; i < oldQuads.size(); i++) 
			{	
				// Any quad has 4 edges
				std::pair<int, int> edge1 = getEdgePair(oldQuads[i].vIndex[0], oldQuads[i].vIndex[1]);
				std::pair<int, int> edge2 = getEdgePair(oldQuads[i].vIndex[1], oldQuads[i].vIndex[2]);
				std::pair<int, int> edge3 = getEdgePair(oldQuads[i].vIndex[2], oldQuads[i].vIndex[3]);
				std::pair<int, int> edge4 = getEdgePair(oldQuads[i].vIndex[3], oldQuads[i].vIndex[0]);

				if (std::find(oldEdges.begin(), oldEdges.end(), edge1) == oldEdges.end()) {
					oldEdges.push_back(edge1);
				}
				if (std::find(oldEdges.begin(), oldEdges.end(), edge2) == oldEdges.end()) {
					oldEdges.push_back(edge2);
				}
				if (std::find(oldEdges.begin(), oldEdges.end(), edge3) == oldEdges.end()) {
					oldEdges.push_back(edge3);
				}
				if (std::find(oldEdges.begin(), oldEdges.end(), edge4) == oldEdges.end()) {
					oldEdges.push_back(edge4);
				}

				// VERTICES
				incidentFaces[oldQuads[i].vIndex[0]].push_back(i);
				incidentFaces[oldQuads[i].vIndex[1]].push_back(i);
				incidentFaces[oldQuads[i].vIndex[2]].push_back(i);
				incidentFaces[oldQuads[i].vIndex[3]].push_back(i);

				// MIDPOINTS OF EDGES
				edgeMidpoints[edge1] = (oldVertices[edge1.first] + oldVertices[edge1.second]) / 2.0f;
				edgeMidpoints[edge2] = (oldVertices[edge2.first] + oldVertices[edge2.second]) / 2.0f;
				edgeMidpoints[edge3] = (oldVertices[edge3.first] + oldVertices[edge3.second]) / 2.0f;
				edgeMidpoints[edge4] = (oldVertices[edge4.first] + oldVertices[edge4.second]) / 2.0f;

				// INCIDENT EDGES FOR VERTICES
				std::vector<std::pair<int, int>> incidentEdgesForVertex0 = incidentEdges[oldQuads[i].vIndex[0]];
				std::vector<std::pair<int, int>> incidentEdgesForVertex1 = incidentEdges[oldQuads[i].vIndex[1]];
				std::vector<std::pair<int, int>> incidentEdgesForVertex2 = incidentEdges[oldQuads[i].vIndex[2]];
				std::vector<std::pair<int, int>> incidentEdgesForVertex3 = incidentEdges[oldQuads[i].vIndex[3]];

				if (std::find(incidentEdgesForVertex0.begin(), incidentEdgesForVertex0.end(), edge1) == incidentEdgesForVertex0.end()) {
					incidentEdges[oldQuads[i].vIndex[0]].push_back(edge1);
				}
				if (std::find(incidentEdgesForVertex1.begin(), incidentEdgesForVertex1.end(), edge1) == incidentEdgesForVertex1.end()) {
					incidentEdges[oldQuads[i].vIndex[1]].push_back(edge1);
				}
				if (std::find(incidentEdgesForVertex1.begin(), incidentEdgesForVertex1.end(), edge2) == incidentEdgesForVertex1.end()) {
					incidentEdges[oldQuads[i].vIndex[1]].push_back(edge2);
				}
				if (std::find(incidentEdgesForVertex2.begin(), incidentEdgesForVertex2.end(), edge2) == incidentEdgesForVertex2.end()) {
					incidentEdges[oldQuads[i].vIndex[2]].push_back(edge2);
				}
				if (std::find(incidentEdgesForVertex2.begin(), incidentEdgesForVertex2.end(), edge3) == incidentEdgesForVertex2.end()) {
					incidentEdges[oldQuads[i].vIndex[2]].push_back(edge3);
				}
				if (std::find(incidentEdgesForVertex3.begin(), incidentEdgesForVertex3.end(), edge3) == incidentEdgesForVertex3.end()) {
					incidentEdges[oldQuads[i].vIndex[3]].push_back(edge3);
				}
				if (std::find(incidentEdgesForVertex3.begin(), incidentEdgesForVertex3.end(), edge4) == incidentEdgesForVertex3.end()) {
					incidentEdges[oldQuads[i].vIndex[3]].push_back(edge4);
				}
				if (std::find(incidentEdgesForVertex0.begin(), incidentEdgesForVertex0.end(), edge4) == incidentEdgesForVertex0.end()) {
					incidentEdges[oldQuads[i].vIndex[0]].push_back(edge4);
				}
				
				// INCIDENT FACES FOR EDGES
				incidentFacesForEdges[edge1].push_back(i);
				incidentFacesForEdges[edge2].push_back(i);
				incidentFacesForEdges[edge3].push_back(i);
				incidentFacesForEdges[edge4].push_back(i);
			}

			// 1. Compute the face points
			std::vector<Vector3> facePoints;
			for (int i = 0; i < oldQuads.size(); i++) {
				Vector3 facePoint = calculateFacePoint({oldQuads[i].vIndex[0], oldQuads[i].vIndex[1], oldQuads[i].vIndex[2], oldQuads[i].vIndex[3] }, newMesh);
				facePoints.push_back(facePoint);
			}

			
			// 2. Compute the edge points
			std::vector<Vector3> edgePoints;
			for (int i = 0; i < oldEdges.size(); i++) {
				Vector3 incidentFace1 = facePoints[incidentFacesForEdges[oldEdges[i]][0]];
				Vector3 incidentFace2 = facePoints[incidentFacesForEdges[oldEdges[i]][1]];
				Vector3 edgeMidpoint2 = edgeMidpoints[oldEdges[i]] * 2.0f;
				Vector3 edgePoint = (incidentFace1 + incidentFace2 + edgeMidpoint2) / 4.0f;
				edgePoints.push_back(edgePoint);
			}
			

			// 3. Compute the new vertices
			/* Notes:
				-> For each vertex, we must find the average of the face points and edge points adjacent to the old vertex, and the old vertex itself.
				-> Formula:
					New vertex = (Q + 2R + S(n - 3)) / n

					Where:
					-> Q: average of the new face points of all faces adjacent to the old vertex.
					-> R: average of the midpoints of all old edges incident on the old vertex. NOT the new edge points.
					-> S: the old vertex.
					-> n: number of faces adjacent to the old vertex. (valence of the vertex)

				-> After computing the new vertices, new edges formed by:
					- Connecting each new face point to the new edge points of the edges defining the old face.
					- Connecting each new vertex point tot he new edge points of all old edges incident on the old vertex.
			*/
			for (int i = 0; i < oldVertices.size(); i++) {
				Vector3 avg_face_points = Vector3(0.0f, 0.0f, 0.0f);
				Vector3 avg_mid_edges = Vector3(0.0f, 0.0f, 0.0f);
				Vector3 old_coords = oldVertices[i];
				std::vector<int> incidentFaceIndices = incidentFaces[i];
				int n = incidentFaceIndices.size(); // valence of the vertex
				std::vector<std::pair<int, int>> incidentEdgeIndices = incidentEdges[i];

				// Both can be done in a single loop because the number of faces and edges adjacent to a vertex are the same.
				for (int j = 0; j < n; j++) {
					// New face points
					avg_face_points += facePoints[incidentFaceIndices[j]];

					// Midpoint of the old edge
					avg_mid_edges += edgeMidpoints[incidentEdgeIndices[j]];
				}
				avg_face_points /= n;
				avg_mid_edges /= n;

				float m1 = 1.0f / n;
				float m2 = 2.0f / n;
				float m3 = (n - 3.0f) / n;

				Vector3 newVertex = m1 * avg_face_points + m2 * avg_mid_edges + m3 * old_coords;

				newMesh->vertices[i] = newVertex;
			}

			// Add all edge points to the new vertices, then store the indices of the edge points in a map for later use.
			std::map<std::pair<int, int>, int> edgePointIndices;
			for (int i = 0; i < edgePoints.size(); i++) {
				newMesh->vertices.push_back(edgePoints[i]);
				edgePointIndices[oldEdges[i]] = newMesh->vertices.size() - 1;
			}

			// 4. Reconstruct the mesh with the new vertices and faces
			std::vector<Quad> newQuads;
			for (int i = 0; i < oldQuads.size(); i++)
			{
				Vector3 facePoint = facePoints[i];
				newMesh->vertices.push_back(facePoint);
				int vFIndex = newMesh->vertices.size() - 1;
				// Get the incident edge points
				std::pair<int, int> edge1 = getEdgePair(oldQuads[i].vIndex[0], oldQuads[i].vIndex[1]);
				std::pair<int, int> edge2 = getEdgePair(oldQuads[i].vIndex[1], oldQuads[i].vIndex[2]);
				std::pair<int, int> edge3 = getEdgePair(oldQuads[i].vIndex[2], oldQuads[i].vIndex[3]);
				std::pair<int, int> edge4 = getEdgePair(oldQuads[i].vIndex[3], oldQuads[i].vIndex[0]);

				int vE01Index = edgePointIndices[edge1];
				int vE12Index = edgePointIndices[edge2];
				int vE23Index = edgePointIndices[edge3];
				int vE30Index = edgePointIndices[edge4];

				// Get vertex indices
				int v0Index = oldQuads[i].vIndex[0];
				int v1Index = oldQuads[i].vIndex[1];
				int v2Index = oldQuads[i].vIndex[2];
				int v3Index = oldQuads[i].vIndex[3];

				// Add the new faces
				newQuads.push_back(Quad({ v0Index, vE01Index, vFIndex, vE30Index }));
				newQuads.push_back(Quad({ vE01Index, v1Index, vE12Index, vFIndex }));
				newQuads.push_back(Quad({ vFIndex, vE12Index, v2Index, vE23Index }));
				newQuads.push_back(Quad({ vE30Index, vFIndex, vE23Index, v3Index }));
			}

			newMesh->quads = newQuads;
			newMesh->UpdateMesh();

			// Clear caches
			incidentFaces.clear();
			incidentEdges.clear();
			incidentFacesForEdges.clear();
			edgeMidpoints.clear();
			facePoints.clear();
			oldEdges.clear();
			newQuads.clear();
			
			return newMesh;
		}
	}
}

Vector3 MeshSubdivider::calculateFacePoint(const std::vector<int>& faceVertexIndices,const Mesh* mesh) {
	Vector3 facePoint = Vector3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < faceVertexIndices.size(); i++) {
		facePoint += mesh->vertices[faceVertexIndices[i]];
	}
	facePoint /= faceVertexIndices.size();
	return facePoint;
}
std::pair<int, int> MeshSubdivider::getEdgePair(int v1, int v2) {
	return std::make_pair(std::min(v1, v2), std::max(v1, v2));
}