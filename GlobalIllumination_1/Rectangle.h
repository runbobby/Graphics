#pragma once
#include "Object3D.h"

class Rectangle : public Object3D
{
public:
	Rectangle(Vector3f v0, Vector3f s1, Vector3f s2, Material* m, Vector3f n = Vector3f::ZERO);
	bool intersect(const Ray& r, Hit& h, float tmin);
	bool any_intersect(const Ray& r, Hit& h, float tmin);
private:
	Vector3f vertex0;
	Vector3f side_1;
	Vector3f side_2;
	Vector3f normal;
};