#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
////This class assumes that the three points are affinely independent. If not, there will probably be a /0 exception
////Assumes counterclockwise notation.

class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle(const Vector3f& aa, const Vector3f& bb, const Vector3f& cc, Material* m);

	virtual bool intersect(const Ray& ray, Hit& hit, float tmin);



	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	///vertices
	///The triangle lies on the plane dot(v,normal)=d

	Vector3f a, b, c;
	//Vector3f normal;
	//float d;
};

#endif //TRIANGLE_H
