#include "SampleRay.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"
#include <vecmath.h>
#include "Departure.h"
#include "LightSource.h"

///Maybe smaller
#define EPSILON 0.0001f

///returns the reflection of incoming on the plane with normal normal. incoming is into the plane, and outgoing is away.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
	Vector3f n = normal.normalized();
	float ni = Vector3f::dot(n, incoming);
	return incoming - 2 * ni * n;
}

///if the ray starts inside the object, dot(incoming,normal)>0
///returns false if there is total internal reflection
///otherwise, updates transmitted to the direction of the transmitted ray
///n, nt are the refraction indices of the originating medium and the transmitting medium, resp.
bool transmittedDirection(const Vector3f& normal, const Vector3f& incoming, float n, float nt, Vector3f& transmitted)
{
	Vector3f normal2 = normal.normalized();
	float in = Vector3f::dot(normal2, incoming);
	float r = n / nt;
	if (in < 0)
	{
		float a = 1 - r*r*(1 - in*in);
		if (a < 0) { return false; }
		transmitted = r*(incoming - in * normal2) - normal2 * std::sqrt(a);
		return true;
	}
	else
	{
		float a = 1 - r*r*(1 - in*in);
		if (a < 0) { return false; }
		transmitted = r*(incoming - in * normal2) + normal2 * std::sqrt(a);
		return true;
	}
	
}

///return: random unit vector with positive dot product with normal, sampled uniformly in the hemisphere
Vector3f randomDirection(const Vector3f& normal)
{
	float u = 2.0f * rand() / RAND_MAX - 1;
	float t = 6.2831853f * rand() / RAND_MAX;
	float a = std::sqrt(1 - u*u);
	Vector3f v{ a*cos(t), a*sin(t), u };
	if (Vector3f::dot(v, normal)>0) { return v; }
	else { return -v; }
}

Vector3f randomDirection()
{
	float u = 2.0f * rand() / RAND_MAX - 1;
	float t = 6.2831853f * rand() / RAND_MAX;
	float a = std::sqrt(1 - u*u);
	return Vector3f{ a*cos(t), a*sin(t), u };
}

///return: random unit vector with positive dot product with normal,
///with probability distribution proportional to this dot product
Vector3f randomDiffuseDirection(const Vector3f& normal)
{
	float u = ((float)rand() / RAND_MAX);
	u = std::sqrt(u);
	float t = 6.2831853f * rand() / RAND_MAX;
	float d = std::sqrt(1 - u*u);
	Vector3f v{ d*cos(t), d*sin(t), u };
	///arbitrarily rotate so that Vector3f(0,0,1) becomes normal
	float a = normal[0];
	float b = normal[1];
	float c = normal[2];
	float bb = b*b;
	float aa = a*a;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa+f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;
	return Matrix3f(0, f * u2, a, -c * u1, -a*b*u2, b, b * u1, -a*c*u2, c)*v;
}

///return: random unit vector v with positive dot product with normal, 
///with p.d. proportional to the sth power of dot(v,i)
///There is some ad hoc reflection for edge cases
///we assume that s is positive and dot(i,n) is negative and normal, i are normalized
///specular direction; close to the reflection of i about n with variance related to s
Vector3f randomSpecularDirection(const Vector3f& normal, const Vector3f& i, float s)
{
	Vector3f o = mirrorDirection(normal, i);
	float u = ((float)rand() / RAND_MAX);
	u = std::pow(u, 1/(1+s));
	float t = 6.2831853f * rand() / RAND_MAX;
	float d = std::sqrt(1 - u*u);
	Vector3f v{ d*cos(t), d*sin(t), u };
	///arbitrarily rotate so that Vector3f(0,0,1) becomes i //!!!!! Seems like (0,0,1) should become o. check!!
	float a = o[0];
	float b = o[1];
	float c = o[2];
	float bb = b*b;
	float aa = a*a;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa + f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;
	v = Matrix3f(0, f * u2, a, -c * u1, -a*b*u2, b, b * u1, -a*c*u2, c)*v;
	if (Vector3f::dot(v, normal) >= 0) { return v; }
	else { return v - 2 * normal * Vector3f::dot(normal, v); }
}

SampleRay::SampleRay( SceneParser * scene, int max_bounces) :m_scene(scene)
{
  g=scene->getGroup();
  m_maxBounces = max_bounces;
  num_lights = scene->getNumLights();
  int a_2 = 1; 
  int a_3 = 1;
  for (int i = 0; i < max_bounces+1; i++)
  {
	  powers3.push_back(a_3);
	  a_3 = 3 * a_3;
	  powers2.push_back(a_2);
	  a_2 = 2 * a_2;
  }
}

SampleRay::~SampleRay(){}

///We do not deal with contact between transparent objects. Instead, we write the scene texts to separate 
///such objects
///returns the color seen from the origin along the direction by adding contributions 
///from all lights directly to the first hit then adding reflected and transmitted rays
///Currently, transmitted rays are not spread out
///This uses monte carlo roulette and importance sampling which assumes a Phong material. the diffuse component is sampled with a 
///random direction (not diffuse, since that seemed too slow), and the specular component is sampled with a random specular direction
///We check for objects in the way to all lights
///For shadow rays, calls any_intersect to detect anything blocking a light
///bounces is the number of remaining bounces. (rays are always traced to lights) Expected to be >=0
///n is the refraction index of the origin, and is positive i.e. origin is in a transparent medium
///tmin is the minimum distance after ray's origin where one searches for hits. 
///Hit& hit is updated to give the information of the nearest hit. 
///Often, it's not used, but it is used for a t-buffer in photon mapping
///q is meant to give uniform sampling for MC Roulette, and is given by the main loop
///by incrementing for each photon. If there are few samples, this may not work well.

Vector3f SampleRay::traceRay( Ray& ray, float tmin, int bounces, float n, Hit& hit, int q ) const
{
	g->intersect(ray, hit, tmin);
	if (hit.getMaterial() == nullptr){ return m_scene->getBackgroundColor(ray.getDirection()); }

	Vector3f N = hit.getNormal();
	Vector3f d = ray.getDirection();
	Vector3f p = ray.pointAtParameter(hit.getT());
	Material* M = hit.getMaterial();
	Vector3f color = (M->getLightColor());

	///Compute contribution of unblocked lights to hit
	for (int i = 0; i < num_lights; i++)
	{
		Vector3f dir, col;
		float dis;
		(m_scene->getLight(i))->getIllumination(p, dir, col, dis);
		Ray shadow_ray(p, dir.normalized());
		Hit shadow_hit = Hit(dis, NULL, Vector3f(0,0,1) );
		if (!(g->any_intersect(shadow_ray, shadow_hit, EPSILON))) { 
			color += M->Shade(ray, hit, dir, col); 
		}
	}/**/

	if (bounces == 0) { return color; }

	///add light of material
	Vector3f sc		= M->getSpecularColor();
	bool is_sc_0	= (sc == Vector3f::ZERO);
	Vector3f dc		= M->getDiffuseColor();
	float s			= M->getShininess();

	///former code. It is shortened below
	/*if (Vector3f::dot(N, d) > 0)
		///from inside to air. Start must be transparent.
	{
		Vector3f t(0);
		bool b = transmittedDirection(N, d, n, 1, t);
		if (b)
			///From physics, b should always be true, but whatever ya know
		{
			float r0 = (1 - 2 * n + n*n) / (1 + 2 * n + n*n);
			float c = ((n <= 1) ? abs(Vector3f::dot(d, N)) : abs(Vector3f::dot(t, N)));
			float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
			Hit hit1 = Hit(); Hit hit2 = Hit();
				///the first instance of r_w here should be based on the direction i, but I've omitted
				///that for simplicity and time considerations
				///Ths random direction i may have different properties such as total internal reflection, 
				///so this is not too justified
				///We assume that sc is always nonzero for transparent materials

			int a = (q % powers3[bounces]) / powers3[bounces - 1];
			//int a = rand() % 3;
			if (a == 0)
			{
				Vector3f i = randomDirection(-N);
				color += 3*dc*traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q)*Vector3f::dot(i, -N);
			}
			else {
				if(a == 1){
					Vector3f i = randomSpecularDirection(-N, d, s);
					color += 3* reflection_weight * sc * traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q);
				}
				else {
					color += 3 * sc * (1 - reflection_weight) * traceRay(Ray(p, t), EPSILON, bounces - 1, n, hit1, q);
				}
			}
		}
		else
		{
			Hit hit1 = Hit();
			int a = (q % powers2[bounces]) / powers2[bounces - 1];
			//int a = rand() % 2;
			if (a)
			{
				Vector3f i = randomDirection(-N);
				color += 2 * dc*traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q)*Vector3f::dot(i,-N);
			}
			else
			{
				Vector3f i = randomSpecularDirection(-N, d, s);
				color += 2 * sc * traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q);
			}
		}
	}
	else
		///from air to inside; n should be 1
	{
		if (M->getRefractionIndex() <= 0)
			///not transparent
		{
			Hit hit1 = Hit();
			if (is_sc_0)
			{//////
				Vector3f i = randomDirection(N);
				color += dc*traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q);
			}
			else
			{
				int a = (q % powers2[bounces]) / powers2[bounces - 1];
				//int a = rand() % 2;
				if (a)
				{
					Vector3f i = randomDirection(N);
					color += 2 * dc*traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q)*Vector3f::dot(i, N);
				}
				else
				{
					Vector3f i = randomSpecularDirection(N, d, s);
					color += 2 * sc * traceRay(Ray(p, i), EPSILON, bounces - 1, n, hit1, q);
				}
			}
		}
		else
		{
			float m = M->getRefractionIndex();
			Vector3f t(0);
			bool b = transmittedDirection(N, d, n, m, t);
			if (b)
			{
				float r0 = (n*n - 2 * m + m*m) / (n*n + 2 * m + m*m);
				float c = ((n <= m) ? abs(Vector3f::dot(d, N)) : abs(Vector3f::dot(t, N)));
				float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
				Hit hit1 = Hit(); Hit hit2 = Hit();
				int a = (q % powers3[bounces]) / powers3[bounces - 1];
				//int a = rand() % 3;
				if (a == 0)
				{
					Vector3f i = randomDirection(N);
					color += 3 * dc*traceRay(Ray(p, i), EPSILON, bounces - 1, m, hit1, q)*Vector3f::dot(i, N);
				}
				else {
					if (a == 1) {
						Vector3f i = randomSpecularDirection(N, d, s);
						color += 3* reflection_weight * sc * traceRay(Ray(p, i), EPSILON, bounces - 1, m, hit1, q);
						1;

					}
					else {
						color += 3 * sc * (1 - reflection_weight) * traceRay(Ray(p, t), EPSILON, bounces - 1, m, hit2, q);
					}
				}
			}
			else
			{
				Hit hit1 = Hit();
				int a = (q % powers2[bounces]) / powers2[bounces - 1];
				//int a = rand() % 2;
				if (a)
				{
					Vector3f i = randomDirection(N);
					color += 2 * dc*traceRay(Ray(p, i), EPSILON, bounces - 1, m, hit1, q)*Vector3f::dot(i, N);
				}
				else
				{
					Vector3f i = randomSpecularDirection(N, d, s);
					color += 2 * sc * traceRay(Ray(p, i), EPSILON, bounces - 1, m, hit1, q);
				}
			}
		}
	}/**/

	Vector3f effectiveNormal{}; //either normal or -normal depending on whether ray is from inside or outside
	float nStart;
	float nEnd;
	float nNext;
	bool transparent = true;
	int mode = 1; ///1, 2, or 3
	Hit hit0 = Hit();
	Vector3f colorMultiplier{};
	Vector3f outgoing{}; 

	if (Vector3f::dot(N, d) > 0)
	{///inside to air
		effectiveNormal = -N;
		nStart = n;
		nEnd = 1.0f;
	}
	else
	{
		effectiveNormal = N;
		nStart = 1.0f;
		nEnd = M->getRefractionIndex();
	}
	nNext = nStart;
	Vector3f t{};
	if (M->getRefractionIndex() <= 0)
	{ ///not transparent
		if (is_sc_0) { outgoing = randomDirection(effectiveNormal); colorMultiplier = dc; }
		else {mode = 2;}
	}
	else
	{ ///transparent
		bool b1 = transmittedDirection(N, d, nStart, nEnd, t);
		if (b1)
		{ ///ray can transmit; mode = 3;
			float t0 = (nStart - nEnd) / (nStart + nEnd);
			float r0 = t0*t0;
			float c = ((nStart <= nEnd) ? abs(Vector3f::dot(d, N)) : abs(Vector3f::dot(t, N)));
			float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
			int a = (q % powers3[bounces]) / powers3[bounces - 1];
			if (a == 0){ ///ray diffusely reflects
				outgoing = randomDirection(effectiveNormal); 
				colorMultiplier = 3 * dc * Vector3f::dot(outgoing, effectiveNormal); 
			} else if (a == 1) { ///specularly reflects
				outgoing = randomSpecularDirection(effectiveNormal, d, s); 
				colorMultiplier = 3 * sc * reflection_weight; 
			} else { ///transmits
				outgoing = t; 
				colorMultiplier = 3 * sc * (1 - reflection_weight); 
				nNext = nEnd;
			}
		}
		else { mode = 2; }
	}
	if (mode == 2)
	{
		int a = (q % powers2[bounces]) / powers2[bounces - 1];
		if (a)
		{
			outgoing = randomDirection(effectiveNormal);
			colorMultiplier = 2 * dc *Vector3f::dot(outgoing, effectiveNormal); 
		}
		else
		{
			outgoing = randomSpecularDirection(effectiveNormal, d, s);
			colorMultiplier = 2 * sc;
		}
	}
	color += colorMultiplier*traceRay(Ray(p, outgoing), EPSILON, bounces - 1, nNext, hit0, q);/**/
	return color;
}
