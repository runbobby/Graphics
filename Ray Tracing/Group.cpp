#include "Group.h"


bool Group::intersect(const Ray& r, Hit& h, float tmin) 
{
	bool result = false;
	for (int i = 0; i < num_objects; i++)
	{
		result = objects[i]->intersect(r, h, tmin) || result;
		//cout << h.getT();
		//int j; cin >> j;
	}
	return result;
}


int Group::getGroupSize() { return num_objects; }

