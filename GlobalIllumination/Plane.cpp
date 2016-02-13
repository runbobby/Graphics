#include "Plane.h"

///This plane is one-sided
bool Plane::intersect(const Ray& r, Hit& h, float tmin)
{
	float u = Vector3f::dot(r.getDirection(), normal);
	if (u == 0) { return false; }
	float t = (d - Vector3f::dot(r.getOrigin(), normal)) / (u);
	if (t > tmin && t < h.getT()) { h.set(t, this->material, normal); return true; }
	return false;
}

bool Plane::any_intersect(const Ray& r, Hit& h, float tmin)
{
	float u = Vector3f::dot(r.getDirection(), normal);
	if (u == 0) { return false; }
	float t = (d - Vector3f::dot(r.getOrigin(), normal)) / (u);
	if (t > tmin && t < h.getT()) {return true; }
	return false;
}

///Equation is dot(v, normal) = d
Plane::Plane(const Vector3f& n, float dd, Material* m) :Object3D(m) 
{
	d = dd;
	normal = n;
}