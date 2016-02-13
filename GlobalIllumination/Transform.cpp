#include "Transform.h"

Transform::Transform(const Matrix4f& mm, Object3D* obj) :o(obj) {
	m = mm;
	mInverse = m.inverse(nullptr, 0);
	mInverse3 = mInverse.getSubmatrix3x3(0, 0);
	mInverseT = mInverse.transposed();
	mInverseT3 = (m.getSubmatrix3x3(0, 0)).inverse(nullptr,0).transposed();
}


bool Transform::intersect(const Ray& r, Hit& h, float tmin)
{

	bool b = o->intersect(Ray((mInverse*Vector4f(r.getOrigin(),1)).xyz(), mInverse3*r.getDirection()), h, tmin);
	if (b) { h.setNormal((mInverseT3*(h.getNormal())).normalized()); }
	return b;

}

bool Transform::any_intersect(const Ray& r, Hit& h, float tmin)
{

	return o->any_intersect(Ray((mInverse*Vector4f(r.getOrigin(), 1)).xyz(), mInverse3*r.getDirection()), h, tmin);
}