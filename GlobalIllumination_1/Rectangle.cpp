#include "Rectangle.h"

Rectangle::Rectangle(Vector3f v0, Vector3f s1, Vector3f s2, Material* m, Vector3f n): Object3D(m)
{
	vertex0 = v0; side_1 = s1; side_2 = s2;
	Vector3f v = Vector3f::cross(s1, s2).normalized();
	if (Vector3f::dot(v, n) >= 0) { normal = v; }
	else { normal = -v; }
}

bool Rectangle::intersect(const Ray& r, Hit& h, float tmin)
{
	Vector3f a = vertex0; 
	Vector3f b = vertex0 + side_1;
	Vector3f c = vertex0 + side_2;
	Vector3f direction = r.getDirection();
	Vector3f origin = r.getOrigin();
	float list[9] = { (a.x()) - (b.x()),(a.y()) - (b.y()),(a.z()) - (b.z()), (a.x()) - (c.x()),(a.y()) - (c.y()),(a.z()) - (c.z()),  (a[0]) - (origin[0]),(a[1]) - (origin[1]),(a[2]) - (origin[2]), };
	Matrix3f A = Matrix3f(list[0], list[3], direction[0], list[1], list[4], direction[1], list[2], list[5], direction[2]);
	float determinant = A.determinant();
	if (determinant == 0) { return false; }
	float t = Matrix3f::determinant3x3(list[0], list[3], list[6], list[1], list[4], list[7], list[2], list[5], list[8]) / determinant;
	if (t <= tmin || t >= h.getT()) { return false; }
	float beta = Matrix3f::determinant3x3(list[6], list[3], direction[0], list[7], list[4], direction[1], list[8], list[5], direction[2]) / determinant;
	float gamma = Matrix3f::determinant3x3(list[0], list[6], direction[0], list[1], list[7], direction[1], list[2], list[8], direction[2]) / determinant;

	if ((beta >= 0 && gamma >= 0 && beta <= 1 && gamma <= 1))
	{
		h.set(t, this->material, normal);
		return true;
	}
	return false;
}

bool Rectangle::any_intersect(const Ray& r, Hit& h, float tmin)
{
	Vector3f a = vertex0;
	Vector3f b = vertex0 + side_1;
	Vector3f c = vertex0 + side_2;
	Vector3f direction = r.getDirection();
	Vector3f origin = r.getOrigin();
	float list[9] = { (a.x()) - (b.x()),(a.y()) - (b.y()),(a.z()) - (b.z()), (a.x()) - (c.x()),(a.y()) - (c.y()),(a.z()) - (c.z()),  (a[0]) - (origin[0]),(a[1]) - (origin[1]),(a[2]) - (origin[2]), };
	Matrix3f A = Matrix3f(list[0], list[3], direction[0], list[1], list[4], direction[1], list[2], list[5], direction[2]);
	float determinant = A.determinant();
	if (determinant == 0) { return false; }
	float t = Matrix3f::determinant3x3(list[0], list[3], list[6], list[1], list[4], list[7], list[2], list[5], list[8]) / determinant;
	if (t <= tmin || t >= h.getT()) { return false; }
	float beta = Matrix3f::determinant3x3(list[6], list[3], direction[0], list[7], list[4], direction[1], list[8], list[5], direction[2]) / determinant;
	float gamma = Matrix3f::determinant3x3(list[0], list[6], direction[0], list[1], list[7], direction[1], list[2], list[8], direction[2]) / determinant;

	if ((beta >= 0 && gamma >= 0 && beta <= 1 && gamma <= 1))
	{
		return true;
	}
	return false;
}