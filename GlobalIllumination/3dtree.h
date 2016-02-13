#pragma once
#include <vector>
#include "Departure.h"

///Note: Departures is a struct. All that matters here is that it has a position field
///Vector3f is a class that has obvious methods x(), y(), z(), absSquared... everything should be obvious, I think


struct node3D
{
	float minx, maxx, miny, maxy, minz, maxz;
	node3D* child0;
	node3D* child1;
	node3D(std::vector<int>& i);
	node3D(float x0, float x1, float y0, float y1, float z0, float z1, std::vector<int>& i);
	node3D(float x0, float x1, float y0, float y1, float z0, float z1, std::vector<int>& i, const vector<float>& xs, const vector<float>& ys, const vector<float>& zs);
	std::vector<int> indices; 
	static void create3DSubtree(node3D* node, const vector<float> & xs, const vector<float> & ys, const vector<float> & zs);
	static node3D* create3DTree(const vector<Departure>& ds);

	///this method finds the nearest neighbors of position in the 3dtree for root, up to k neighbors and at a squared distance up to mDSUB. 
	///The vectors searched are the positions of elements in departures, which should also be the vector used to create the 3dtree for root.
	///(conceptually, this vector should be stored with the root)
	///the actual mDS is mDSActual (and can be mDSUB)
	///neighborsFound is set equal to the number of neighbors found within mDSUB
	///h is set equal to a vector of indices of the nearest neighbors (no repeats; no others; possibly size < k)
	static void nearestKNeighbors(node3D* root, const Vector3f& position, int k, vector<int>& h, const vector<Departure>& departures, float& maxDistanceSquaredActual, float maxDistanceSquaredUpperBound, int& neighborsFound);
	static void nearestKNeighborsRecursion(node3D* node, const Vector3f& position, vector<int>& h, vector<float>& hSD, const vector<float>& rSD);
};
