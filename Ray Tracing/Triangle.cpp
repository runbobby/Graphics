#include "Triangle.h"

///When ray is in the plane of the triangle, returns false. 
///There may be float errors. For example, a triangular mesh may have holes in it.

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin)
{
	Vector3f direction = ray.getDirection();
	Vector3f origin = ray.getOrigin();
	float list[9] = { (a.x()) - (b.x()),(a.y()) - (b.y()),(a.z()) - (b.z()), (a.x()) - (c.x()),(a.y()) - (c.y()),(a.z()) - (c.z()),  (a[0]) - (origin[0]),(a[1]) - (origin[1]),(a[2]) - (origin[2]), };
	//int list[9] = { 1,2,3,4,5,6,7,8,9 };
	Matrix3f A = Matrix3f(list[0], list[3], direction[0], list[1], list[4], direction[1], list[2], list[5], direction[2]);
	float determinant = A.determinant();
	//(a-b).print();
	//(a - c).print();
	//direction.print();
	//cout << A(0, 0) << " " << A(0, 1) << " " << A(0, 2) << " " << A(1, 0) << " " << A(1,1) << " " << A(1,2) << " " << A(2, 0) << " " << A(2,1) << " " << A(2,2) << " ";
	//cout << A.determinant();
	//Vector3f::cross(a - b, a - c).print();
	//cout << 3;
	if (determinant == 0) { return false; }
	//cout << 1;
	float t = Matrix3f::determinant3x3(list[0], list[3], list[6], list[1], list[4], list[7], list[2], list[5], list[8]) / determinant;
	if (t <= tmin || t >= hit.getT()) { return false; }
	//cout << 2;
	float beta = Matrix3f::determinant3x3(list[6], list[3], direction[0], list[7], list[4], direction[1], list[8], list[5], direction[2]) / determinant;
	float gamma = Matrix3f::determinant3x3(list[0], list[6], direction[0], list[1], list[7], direction[1], list[2], list[8], direction[2]) / determinant;
	
	if ((beta >= 0 && gamma >= 0 && beta + gamma <= 1) )
	{
		//cout << "Hello. ";
		//cout << ((this->material)==nullptr)<< " " ;
		if(hasTex)
		{
			hit.hasTex = true; hit.setTexCoord((1 - beta - gamma)*texCoords[0] + beta*texCoords[1] + gamma*texCoords[2]);
		}
		hit.set(t, this->material, ((1-beta-gamma)*normals[0]+beta*normals[1]+gamma*normals[2]).normalized()); 
		return true; 
	}
	return false;
}

Triangle::Triangle(const Vector3f& aa, const Vector3f& bb, const Vector3f& cc, Material* m){
	this->material = m;
	hasTex = false; a = aa; b = bb; c = cc;
	//cout << (this->material == NULL);
	//cout << (m == nullptr);
	//cout << true;

	//int d; cin >> d;
	//normal = Vector3f::cross(b - a, c - a).normalized();
	//d = Vector3f::dot(a, normal);
}