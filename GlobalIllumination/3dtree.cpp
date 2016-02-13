#include "3dtree.h"
#include <algorithm>
#include <vecmath.h>

node3D::node3D(std::vector<int>& i)
{
	child0 = nullptr;
	child1 = nullptr;
	indices = i;
}

node3D::node3D(float x0, float x1, float y0, float y1, float z0, float z1, std::vector<int>& i)
{
	minx = x0;
	maxx = x1;
	miny = y0;
	maxy = y1;
	minz = z0;
	maxz = z1;
	child0 = nullptr;
	child1 = nullptr;
	indices = i;
}

///lol ignoring the first 6 arguments for now
///I calculated the mins/maxs from scratch because building the tree is empirically much faster than final gathering
node3D::node3D(float x0, float x1, float y0, float y1, float z0, float z1, std::vector<int>& i, const vector<float>& xs, const vector<float>& ys, const vector<float>& zs)
{
	minx = 3.4e+38F;
	maxx = -3.4e+38F;
	miny = 3.4e+38F;
	maxy = -3.4e+38F;
	minz = 3.4e+38F;
	maxz = -3.4e+38F;
	child0 = nullptr;
	child1 = nullptr;
	indices = i;
	for (int j = 0; j < i.size(); j++)
	{
		if (xs[i[j]] < minx) { minx = xs[i[j]]; }
		if (ys[i[j]] < miny) { miny = ys[i[j]]; }
		if (zs[i[j]] < minz) { minz = zs[i[j]]; }
		if (xs[i[j]] > maxx) { maxx = xs[i[j]]; }
		if (ys[i[j]] > maxy) { maxy = ys[i[j]]; }
		if (zs[i[j]] > maxz) { maxz = zs[i[j]]; }
	}
}

node3D* node3D::create3DTree(const vector<Departure>& ds)
{
	vector<float> xs, ys, zs;
	vector<int> indices;
	for (int i = 0; i < ds.size(); i++)
	{
		indices.push_back(i);
		Vector3f v = ds[i].position;
		xs.push_back(v.x());
		ys.push_back(v.y());
		zs.push_back(v.z());
	}
	node3D* root = new node3D(-3.e+38F, 3.e+38F, -3.e+38F, 3.e+38F, -3.e+38F, 3.e+38F, indices, xs, ys, zs);
	create3DSubtree(root, xs, ys, zs);
	return root;
}

//watch out for many equal values!!!

void node3D::create3DSubtree(node3D* node, const vector<float> & xs, const vector<float> & ys, const vector<float> & zs)
{
	vector<int> indices = node->indices;
	int s = indices.size();
	if (s < 5) { return; }
	///depth % 3 determines which of the three orthogonal axes are used for space partitioning JK
	///I've decided to use the longest spatial dimension instead of depth
	float xRange = node->maxx - node->minx;
	float yRange = node->maxy - node->miny;
	float zRange = node->maxz - node->minz;
	int direction = 0; ///0: x 1: y 2: z
	if (yRange > xRange)
	{
		if (yRange > zRange) { direction = 1; }
		else { direction = 2; }
	}
	else
	{
		if (zRange > xRange) { direction = 2; }
	}
	if (direction == 0)
	{
		vector<float> node_xs;
		for (int i = 0; i < s; i++)
		{
			node_xs.push_back(xs[indices[i]]);
		}
		std::nth_element(node_xs.begin(), node_xs.begin() + s / 2, node_xs.end());
		float median = node_xs[s / 2];
		vector<int> low_indices, high_indices;
		for (int i = 0; i < s; i++)
		{
			if (xs[indices[i]] < median) { low_indices.push_back(indices[i]); }
			else if (xs[indices[i]] > median) { high_indices.push_back(indices[i]); }
			else
			{
				if (rand() % 2) { low_indices.push_back(indices[i]); }
				else { high_indices.push_back(indices[i]); }
			}
		}
		node->child0 = new node3D(node->minx, median, node->miny, node->maxy, node->minz, node->maxz, low_indices, xs, ys, zs);
		node->child1 = new node3D(median, node->maxx, node->miny, node->maxy, node->minz, node->maxz, high_indices, xs, ys, zs);
		create3DSubtree(node->child0, xs, ys, zs);
		create3DSubtree(node->child1, xs, ys, zs);
	}
	else if (direction == 1)
	{
		vector<float> node_ys;
		for (int i = 0; i < s; i++)
		{
			node_ys.push_back(ys[indices[i]]);
		}
		std::nth_element(node_ys.begin(), node_ys.begin() + s / 2, node_ys.end());
		float median = node_ys[s / 2];
		vector<int> low_indices, high_indices;
		for (int i = 0; i < s; i++)
		{
			if (ys[indices[i]] < median) { low_indices.push_back(indices[i]); }
			else if (ys[indices[i]] > median) { high_indices.push_back(indices[i]); }
			else
			{
				if (rand() % 2) { low_indices.push_back(indices[i]); }
				else { high_indices.push_back(indices[i]); }
			}
		}
		node->child0 = new node3D(node->minx, node->maxx, node->miny, median, node->minz, node->maxz, low_indices, xs, ys, zs);
		node->child1 = new node3D(node->minx, node->maxx, median, node->maxy, node->minz, node->maxz, high_indices, xs, ys, zs);
		create3DSubtree(node->child0, xs, ys, zs);
		create3DSubtree(node->child1, xs, ys, zs);
	}
	else
	{
		vector<float> node_zs;
		for (int i = 0; i < s; i++)
		{
			node_zs.push_back(zs[indices[i]]);
		}
		std::nth_element(node_zs.begin(), node_zs.begin() + s / 2, node_zs.end());
		float median = node_zs[s / 2];
		vector<int> low_indices, high_indices;
		for (int i = 0; i < s; i++)
		{
			if (zs[indices[i]] < median) { low_indices.push_back(indices[i]); }
			else if (zs[indices[i]] > median) { high_indices.push_back(indices[i]); }
			else
			{
				if (rand() % 2) { low_indices.push_back(indices[i]); }
				else { high_indices.push_back(indices[i]); }
			}
		}
		node->child0 = new node3D(node->minx, node->maxx, node->miny, node->maxy, node->minz, median, low_indices, xs, ys, zs);
		node->child1 = new node3D(node->minx, node->maxx, node->miny, node->maxy, median, node->maxz, high_indices, xs, ys, zs);
		create3DSubtree(node->child0, xs, ys, zs);
		create3DSubtree(node->child1, xs, ys, zs);
	}
}


///this method finds the nearest neighbors of position in the 3dtree for root, up to k neighbors and at a squared distance up to mDSUB. 
///The vectors searched are the positions of elements in departures, which should also be the vector used to create the 3dtree for root.
///(conceptually, this vector should be stored with the root)
///the actual mDS is mDSActual (and can be mDSUB)
///neighborsFound is set equal to the number of neighbors found within mDSUB
///h is set equal to a vector of indices of the nearest neighbors (no repeats; no others; possibly size < k)
void node3D::nearestKNeighbors(node3D* root, const Vector3f& position, int k, vector<int>& h, const vector<Departure>& departures, float& mDSActual, float mDSUpperBound, int& neighborsFound)
{
	vector<int> heap(k, -1); /// binary max heap; max is at index 0
	vector<float> heapSquaredDistances(k, mDSUpperBound);
	vector<float> relativeSquaredDistances{};
	for (int i = 0; i < departures.size(); i++)
	{
		relativeSquaredDistances.push_back((position - departures[i].position).absSquared());
	}
	nearestKNeighborsRecursion(root, position, heap, heapSquaredDistances, relativeSquaredDistances);
	mDSActual = heapSquaredDistances[0];
	///count number of entries of hSD which are less than mDSUpperBound
	///and add only the corresponding indices to h
	neighborsFound = 0;
	h = vector<int>{};
	for (int i = 0; i < k; i++)
	{
		if (heapSquaredDistances[i] < mDSUpperBound) { neighborsFound++; h.push_back(heap[i]); }
	}
}

void heapify(vector<int>& h, vector<float>& hSD)
{
	int current_index = 0;
	int size = h.size();
	while (current_index < size)
	{
		float key = hSD[current_index];
		int left_index = current_index * 2 + 1;
		int right_index = current_index * 2 + 2;
		float left_key;
		if (left_index >= size) { left_key = -3.e+38F; }
		else { left_key = hSD[left_index]; }
		float right_key;
		if (right_index >= size) { right_key = -3.e+38F; }
		else { right_key = hSD[right_index]; }
		if (key < left_key)
		{
			if (key < right_key)
			{
				if (left_key < right_key)
				{
					int temp0 = h[current_index];
					h[current_index] = h[right_index];
					h[right_index] = temp0;
					float temp1 = hSD[current_index];
					hSD[current_index] = hSD[right_index];
					hSD[right_index] = temp1;
					current_index = right_index;
				}
				else
				{
					int temp0 = h[current_index];
					h[current_index] = h[left_index];
					h[left_index] = temp0;
					float temp1 = hSD[current_index];
					hSD[current_index] = hSD[left_index];
					hSD[left_index] = temp1;
					current_index = left_index;
				}
			}
			else
			{
				int temp0 = h[current_index];
				h[current_index] = h[left_index];
				h[left_index] = temp0;
				float temp1 = hSD[current_index];
				hSD[current_index] = hSD[left_index];
				hSD[left_index] = temp1;
				current_index = left_index;
			}
		}
		else
		{
			if (key < right_key)
			{
				int temp0 = h[current_index];
				h[current_index] = h[right_index];
				h[right_index] = temp0;
				float temp1 = hSD[current_index];
				hSD[current_index] = hSD[right_index];
				hSD[right_index] = temp1;
				current_index = right_index;
			}
			else
			{
				break;
			}
		}
	}
}

void node3D::nearestKNeighborsRecursion(node3D* node, const Vector3f& position, vector<int>& h, vector<float>& hSD, const vector<float>& rSD)
{
	vector<int> indices = node->indices;
	if (indices.size() < 5)
	{
		for (int i = 0; i < indices.size(); i++)
		{
			if (rSD[indices[i]] < hSD[0])
			{
				h[0] = indices[i];
				hSD[0] = rSD[indices[i]];
				heapify(h, hSD);
			}
		}
		return;
	}
	float xDistance = __max(__max(0, position.x() - node->maxx), node->minx - position.x());
	float yDistance = __max(__max(0, position.y() - node->maxy), node->miny - position.y());
	float zDistance = __max(__max(0, position.z() - node->maxz), node->minz - position.z());
	if (hSD[0] < xDistance*xDistance + yDistance*yDistance + zDistance*zDistance)
	{
		return;
	}
	else
	{
		///These comments will randomize the search, but it doesn't seem to help
		//if (rand() % 2){nearestKNeighborsRecursion(node->child1, position, h, hSD, rSD);nearestKNeighborsRecursion(node->child0, position, h, hSD, rSD);}
		//else{
			nearestKNeighborsRecursion(node->child0, position, h, hSD, rSD);
			nearestKNeighborsRecursion(node->child1, position, h, hSD, rSD);
		//}
	}
}