#include "Group.h"


bool Group::intersect(const Ray& r, Hit& h, float tmin) 
{
	bool result = false;
	for (int i = 0; i < num_objects; i++)
	{
		result = objects[i]->intersect(r, h, tmin) || result;
	}
	return result;
}

bool Group::any_intersect(const Ray& r, Hit& h, float tmin)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (objects[i]->any_intersect(r, h, tmin))
		{
			return true;
		}
	}
	return false;
}

int Group::getGroupSize() { return num_objects; }

