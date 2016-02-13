#include "Triangle.h"

///When ray is in the plane of the triangle, returns false. 
///There may be float errors. For example, a triangular mesh may have holes in it.
///These should be eliminated by jittering.

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin)
{
	Vector3f direction = ray.getDirection();
	Vector3f origin = ray.getOrigin();
	float list[9] = { (a.x()) - (b.x()),(a.y()) - (b.y()),(a.z()) - (b.z()), (a.x()) - (c.x()),(a.y()) - (c.y()),(a.z()) - (c.z()),  (a[0]) - (origin[0]),(a[1]) - (origin[1]),(a[2]) - (origin[2]), };
	Matrix3f A = Matrix3f(list[0], list[3], direction[0], list[1], list[4], direction[1], list[2], list[5], direction[2]);
	float determinant = A.determinant();
	if (determinant == 0) { return false; }
	float t = Matrix3f::determinant3x3(list[0], list[3], list[6], list[1], list[4], list[7], list[2], list[5], list[8]) / determinant;
	if (t <= tmin || t >= hit.getT()) { return false; }
	float beta = Matrix3f::determinant3x3(list[6], list[3], direction[0], list[7], list[4], direction[1], list[8], list[5], direction[2]) / determinant;
	float gamma = Matrix3f::determinant3x3(list[0], list[6], direction[0], list[1], list[7], direction[1], list[2], list[8], direction[2]) / determinant;
	
	if ((beta >= 0 && gamma >= 0 && beta + gamma <= 1) )
	{
		if(hasTex)
		{
			hit.hasTex = true; hit.setTexCoord((1 - beta - gamma)*texCoords[0] + beta*texCoords[1] + gamma*texCoords[2]);
		}
		hit.set(t, this->material, ((1-beta-gamma)*normals[0]+beta*normals[1]+gamma*normals[2]).normalized()); 
		return true; 
	}
	return false;
}

bool Triangle::any_intersect(const Ray& ray, Hit& hit, float tmin)
{
	Vector3f direction = ray.getDirection();
	Vector3f origin = ray.getOrigin();
	float list[9] = { (a.x()) - (b.x()),(a.y()) - (b.y()),(a.z()) - (b.z()), (a.x()) - (c.x()),(a.y()) - (c.y()),(a.z()) - (c.z()),  (a[0]) - (origin[0]),(a[1]) - (origin[1]),(a[2]) - (origin[2]), };
	Matrix3f A = Matrix3f(list[0], list[3], direction[0], list[1], list[4], direction[1], list[2], list[5], direction[2]);
	float determinant = A.determinant();
	if (determinant == 0) { return false; }
	float t = Matrix3f::determinant3x3(list[0], list[3], list[6], list[1], list[4], list[7], list[2], list[5], list[8]) / determinant;
	if (t <= tmin || t >= hit.getT()) { return false; }
	float beta = Matrix3f::determinant3x3(list[6], list[3], direction[0], list[7], list[4], direction[1], list[8], list[5], direction[2]) / determinant;
	float gamma = Matrix3f::determinant3x3(list[0], list[6], direction[0], list[1], list[7], direction[1], list[2], list[8], direction[2]) / determinant;

	if ((beta >= 0 && gamma >= 0 && beta + gamma <= 1))
	{
		return true;
	}
	return false;
}


Triangle::Triangle(const Vector3f& aa, const Vector3f& bb, const Vector3f& cc, Material* m){
	this->material = m;
	hasTex = false; a = aa; b = bb; c = cc;
	Vector3f n = Vector3f::cross(bb - aa, cc - aa).normalized();
	normals[0] = n;
	normals[1] = n;
	normals[2] = n;

}