#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "Ray.h"
#include <vecmath.h>
#include "Material.h"
#include "Object3D.h"
#include <vector>

class LightSource
{
public:
	virtual Ray emitPhoton() = 0;
	LightSource(float ratio, bool visible = true);
	float getPhotonRatio();
	bool isVisible();
protected:
	float photonRatio;
	bool visible;
};

class RectangularLightSource : public LightSource
{
	///Represents a Rectangular LightSource with vertices v0, v0+s1, v0+s2, v0+s1+s2
	///and light equally emitted from all points and in all directions in the hemisphere around n
	///where s1, s2, and n are orthogonal, and n is a unit vector
public:
	RectangularLightSource(Vector3f v0, Vector3f s1, Vector3f s2, Vector3f n, float ratio, vector<float>& spectrum, float angle = 90.f, bool visible = true);
	Ray emitPhoton();

private:
	Vector3f randomConeDirection(const Vector3f& normal, float angle);

	Vector3f normal;//really direction of emitting
	Vector3f vertex0;
	Vector3f side_1;
	Vector3f side_2;
	float length_1;
	float length_2;
	vector<float> spectrum;
	vector<float> cumulativeSpectrum;
	//angle of cone around normal in which light emits uniformly and randomly; 
	float angle;
	Matrix3f rotationMatrix;
};

#endif