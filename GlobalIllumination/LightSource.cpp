#define EPSILON .0001f

#include "LightSource.h"
#include <algorithm>

LightSource::LightSource(float ratio, bool v)
{
	photonRatio = ratio;
	visible = v;
}

float LightSource::getPhotonRatio() { return photonRatio; }

RectangularLightSource::RectangularLightSource(Vector3f v0, Vector3f s1, Vector3f s2, Vector3f n, float ratio, vector<float>& s, float a, bool v):
	vertex0(v0),side_1(s1),side_2(s2), LightSource(ratio, v), spectrum(s)
{
	angle = __max(0, __min(90.f, a));
	length_1 = s1.abs();
	length_2 = s2.abs();
	normal = n.normalized();
	///pre-calculate a totation matrix that takes (0,0,1) to normal. I know this is ugly :(
	float a1 = normal[0];
	float b = normal[1];
	float c = normal[2];
	float bb = b*b;
	float aa = a1*a1;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa + f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;
	rotationMatrix =  Matrix3f(0, f * u2, a1, -c * u1, -a1*b*u2, b, b * u1, -a1*c*u2, c);
	
	cumulativeSpectrum = vector<float>{};
	float total = 0;
	for (int i = 0; i < 32; i++)
	{
		spectrum[i] = __max(0, spectrum[i]);
		total += spectrum[i];
	}
	cumulativeSpectrum.push_back(spectrum[0]/total);
	for (int i = 1; i < 31; i++)
	{
		cumulativeSpectrum.push_back(cumulativeSpectrum[i - 1] + spectrum[i] / total);
	}
	//cS is only 31 elements long
}/**/

Vector3f randomDirection(const Vector3f& normal);
/*{
	float u = 2.0f * rand() / RAND_MAX - 1;
	float t = 6.2831853f * rand() / RAND_MAX;
	float a = std::sqrt(1 - u*u);
	Vector3f v{ a*cos(t), a*sin(t), u };
	if (Vector3f::dot(v, normal)>0) { return v; }
	else { return -v; }
}/**/

Vector3f RectangularLightSource::randomConeDirection(const Vector3f& normal, float angle)
{
	float u = 1 - ((float)rand() / RAND_MAX) * (1-cos(angle));
	float t = 6.2831853f * rand() / RAND_MAX;
	float d = std::sqrt(1 - u*u);
	Vector3f v{ d*cos(t), d*sin(t), u };
	///arbitrarily rotate so that Vector3f(0,0,1) becomes normal
	/*float a = normal[0];
	float b = normal[1];
	float c = normal[2];
	float bb = b*b;
	float aa = a*a;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa + f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;/**/
	return rotationMatrix*v;
}

bool LightSource::isVisible(){ return visible; }

Ray RectangularLightSource::emitPhoton()
{
	///choose wavelength using cumulativeSpectrum
	float r = (float)rand() / RAND_MAX;
	std::vector<float>::iterator up = std::upper_bound(cumulativeSpectrum.begin(), cumulativeSpectrum.end(), r);
	int w = 39 + (up - cumulativeSpectrum.begin());


	if (angle == 0)
	{
		return Ray(vertex0 + (float)rand() / RAND_MAX * side_1 + (float)rand() / RAND_MAX * side_2 + EPSILON*normal,
			normal, w);
	}
	if (angle == 90)
	{
		Vector3f d = randomDirection(normal);
		return Ray(vertex0 + (float)rand() / RAND_MAX * side_1 + (float)rand() / RAND_MAX * side_2 + EPSILON*d,
			d, w);
	}
	Vector3f d = randomConeDirection(normal, angle);
	return Ray(vertex0 + (float)rand() / RAND_MAX * side_1 + (float)rand() / RAND_MAX * side_2 + EPSILON*d,
		d, w);

}



