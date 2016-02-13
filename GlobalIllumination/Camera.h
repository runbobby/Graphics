#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}

	///added 
	virtual Vector3f getCenter() {return center;}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up, float angle);

	virtual Ray generateRay(const Vector2f& point);

	virtual float getTMin() const { 
		return 0.0f;
	}

	float getAngle() const { return angle; }

private:
	//w: direction normalized
	//u: w x up normalized
	//v: u x w (normalized)
	Vector3f u, v, w; 
	float angle;
};

#endif //CAMERA_H