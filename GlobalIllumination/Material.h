#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
#include "Noise.h"
#include "Departure.h"
#include <vector>

///We implement uniform lighting on materials. To generalize this, one should probably use texture maps
///I am not so ambitious here
///The LightColor can be very bright

struct Departure;

class Material
{
public:
	
	Material(const vector<float>& dRatio, const vector<float>& aRatio, const Vector3f& d_color, const Vector3f& s_color = Vector3f::ZERO, float s = 0,
		float r = 0, Vector3f l = Vector3f::ZERO, float b = -1, float c=0);
	//Material(float s , float b , float c , float* dRatio, float* aRatio);
	///Material(int a);

    virtual ~Material();

    virtual Vector3f getDiffuseColor() const ;

    Vector3f Shade( const Ray& ray, const Hit& hit,
            const Vector3f& dirToLight, const Vector3f& lightColor ) ;

	static  Vector3f pointwiseDot( const Vector3f& v1 , const Vector3f& v2 );

	float clampedDot( const Vector3f& L , const Vector3f& N )const;
	void loadTexture(const char * filename);
	float getRefractionIndex();
	Vector3f getDiffuseColor();
	Vector3f getSpecularColor();
	//bool getIsLight();
	float getShininess();
	
	Vector3f getLightColor();

	Vector3f BDRF(Vector3f i, Vector3f N, Vector3f d);
	///The following assume the Phong model and are used for better sampling, hopefully
	Vector3f diffuseBDRF(Vector3f i, Vector3f N, Vector3f d);
	Vector3f specularBDRF(Vector3f i, Vector3f N, Vector3f d);
	
	void setNoise(const Noise & n);
	///I've made this public for convenience. sorry I am bad :(
	Texture t;

	///For photon mapping, finalgathering
	Vector3f radiance(const Departure & d, const Vector3f & outgoing);

	bool interactPhoton(const Ray& incoming, const Hit& hit, Ray& outgoing, Departure& departure);
	Vector3f wavelengthToColor(int w);

protected:
  float shininess;
  ///For Monte Carlo ray tracing
  float refractionIndex;
  Vector3f specularColor;
  Vector3f lightColor;
  Vector3f diffuseColor;
  Noise noise;
  ///For Photon mapping
  vector<float> diffuseRatio;
  vector<float> absorbRatio;
  float bCoefficient;
  float cCoefficient;

  ///For monte carlo integration, rI is the refraction index.
  ///For photon mapping, it is b+c/wavelength^2, where wavelength is in 10nm.
 
private:
	static Vector3f RGBTable[32];

};

#endif // MATERIAL_H
