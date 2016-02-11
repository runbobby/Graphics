#include <vecmath.h>
#include <cmath>
#include "Sphere.h"
#include <iostream>


bool Sphere::intersect(const Ray& r, Hit& h, float tmin) 
{
	Vector3f d = r.getDirection();
	Vector3f u = center - r.getOrigin(); 
	float du = Vector3f::dot(d, u);
	float dd = d.absSquared();
	float a = (du*du - dd*(u.absSquared()-radius*radius));
	//cout << a;
	if (a < 0) { return false; }
	float aroot = std::sqrt(a);
	float t1 = (du + aroot) / dd;	
	float t2 = (du - aroot) / dd;
	//d.print();
	//u.print();
	//cout << t1 << " " << t2 << " " << radius << "center next" << endl;
	//center.print();
	//cout << h.getT();
	// t2 < t1 are the values of intersection
	//cout << t2 << "  " << t1 << endl;
	//cin >> a;
	if (t2 > tmin && t2 < h.getT()) { h.set(t2, this->material, (t2*d - u).normalized());  return true; }
	if (t1 > tmin && t1 < h.getT()) { h.set(t1, this->material, (t1*d - u).normalized()); return true; }
	return false;
}