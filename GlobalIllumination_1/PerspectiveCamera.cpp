#include "Camera.h"

PerspectiveCamera::PerspectiveCamera(const Vector3f& c, const Vector3f& d, const Vector3f& upv, float a)
{
	center = c; 
	direction = d;
	up = upv;
	w = d.normalized();
	u = Vector3f::cross(w, up).normalized();
	v = Vector3f::cross(u, w).normalized();
	//w.print(); u.print(); v.print();
	angle = a;
}

///Expects a Vector2f in [-1,1]^2 as standard
///The first coordinate corresponds to u. The second corresponds to v.
///Return is normalized.
Ray PerspectiveCamera::generateRay(const Vector2f& point)
{
	float M = std::sqrt(((1 / cos(angle)) - 1) / 2);
	//cout << M;
	//center.print();
	//(w + point[0] * u * M + point[1] * v * M).normalized().print();
	//int a; cin >> a;
	return Ray(center, (w + point[0] * u * M + point[1] * v * M).normalized());
}
