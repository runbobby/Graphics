#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere() { center = Vector3f(0); radius = 1; }
		//unit ball at the center

	Sphere(Vector3f c, float r, Material* material):Object3D(material) {
		center = c; radius = r;
	};
	
	~Sphere(){}

	virtual bool intersect(const Ray& r, Hit& h, float tmin);

	virtual bool any_intersect(const Ray& r, Hit& h, float tmin);

protected:
	Vector3f center;
	float radius;

};


#endif
