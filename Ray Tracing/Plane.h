#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane(const Vector3f& normal, float d, Material* m);
	~Plane(){}
	virtual bool intersect(const Ray& r, Hit& h, float tmin);

protected:
	Vector3f normal;
	float d;
	//Plane is dot(normal, v)=d
};
#endif //PLANE_H
		

