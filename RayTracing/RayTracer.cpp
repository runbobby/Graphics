#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

///Maybe smaller
#define EPSILON 0.0001f

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
///assumes that normal is normalized
///OMG check inside out and outside in cases
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
	Vector3f n = normal.normalized();
	float ni = Vector3f::dot(n, incoming);
	return incoming - 2 * ni * n;
	//return ((ni < 0) ? incoming - 2 * ni * n : incoming - 2 * ni * n);
}

///if the ray starts inside the object, dot(incoming,normal)>0
///returns false if there is total internal reflection
///otherwise, updates transmitted to the direction of the transmitted ray
///n, nt are the refraction indices of the originating medium and the transmitting medium, I think
///check. this seems to be wrong
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

///possibly more arguments
RayTracer::RayTracer( SceneParser * scene, int max_bounces) :m_scene(scene)
{
  g=scene->getGroup();
  m_maxBounces = max_bounces;
  num_lights = scene->getNumLights();
}

RayTracer::~RayTracer()
{
}

///We assume that all surfaces are interfaces between a closed object and air, and that the normal points toward air
///This allows one to tell inside vs outside by the sign of dot(normal, direction)
///returns the color seen from the origin along the direction by adding contributions 
///from all lights directly to the first hit then adding reflected and transmitted rays
///We check for objects in the way to all lights using shadow rays
///uses any intersect for shadow rays, although it's not implemented well for meshes (I don't understand 
///the octree structure)
///bounces is the number of remaining bounces (except rays are always traced to lights) Expected to be >=0
///n is the refraction index of the origin, and is positive i.e. origin is in a transparent medium
///almost all of the Hit& are not used. The only use is really
///to form depth and normal maps, and that only uses the first call's Hit&.

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces, float n, Hit& hit ) const
{
	g->intersect(ray, hit, tmin);
	if (hit.getMaterial() == nullptr)
	{
		return m_scene->getBackgroundColor(ray.getDirection());
	}
	Vector3f dc = hit.getMaterial()->getDiffuseColor();
	if (hit.hasTex) { dc = (hit.getMaterial()->t)(hit.texCoord[0], hit.texCoord[1]); }
	Vector3f color = m_scene->getAmbientLight()*(dc);
	//Vector3f color = m_scene->getAmbientLight();
	Vector3f N = hit.getNormal();
	Vector3f d = ray.getDirection();
	Vector3f p = ray.pointAtParameter(hit.getT());

	///Compute contribution of unblocked lights to hit
	for (int i = 0; i < num_lights; i++)
	{
		Vector3f dir, col;
		float dis;
		(m_scene->getLight(i))->getIllumination(p, dir, col, dis);
		Ray shadow_ray(p, dir.normalized());
		Hit shadow_hit = Hit(dis, NULL, Vector3f(0,0,1) );
		if (!(g->any_intersect(shadow_ray, shadow_hit, EPSILON))) { 
			color += hit.getMaterial()->Shade(ray, hit, dir, col); 
		}
	}

	if (bounces == 0) { return color; }

	Vector3f sc = hit.getMaterial()->getSpecularColor();

	if (Vector3f::dot(N, d) >= 0)
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
			if (sc != Vector3f(0))
			{
				color = color + sc*(reflection_weight * traceRay(Ray(p, mirrorDirection(N, d)), EPSILON, bounces - 1, n, hit1) + (1 - reflection_weight) * traceRay(Ray(p, t), EPSILON, bounces - 1, n, hit2));
			}
		}
		else
		{
			Hit hit1 = Hit();
			if (sc != Vector3f(0))
			{
				color = color + sc*(traceRay(Ray(p, mirrorDirection(N, d)), EPSILON, bounces - 1, n, hit1));
			}
		}
	}
	else
		///from air to inside; n should be 1
	{
		if ((hit.getMaterial())->getRefractionIndex() <= 0)
			///not transparent
		{
			Hit hit1 = Hit();
			if (sc != Vector3f(0))
			{
				color = color + sc*(traceRay(Ray(p, mirrorDirection(N, d)), EPSILON, bounces - 1, n, hit1));
			}
		}
		else
		{
			float m = (hit.getMaterial())->getRefractionIndex();
			Vector3f t(0);
			bool b = transmittedDirection(N, d, n, m, t);
			if (b)
			{
				float r0 = (n*n - 2 * m + m*m) / (n*n + 2 * m + m*m);
				float c = ((n <= m) ? abs(Vector3f::dot(d, N)) : abs(Vector3f::dot(t, N)));
				float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
				Hit hit1 = Hit(); Hit hit2 = Hit();
				if (sc != Vector3f(0))
				{
					color = color + sc*(reflection_weight * traceRay(Ray(p, mirrorDirection(N, d)), EPSILON, bounces - 1, m, hit1) + (1 - reflection_weight) * traceRay(Ray(p, t), EPSILON, bounces - 1, m, hit2));
				}
			}
			else
			{
				Hit hit1 = Hit();
				if (sc != Vector3f(0))
				{
					color = color + sc*(traceRay(Ray(p, mirrorDirection(N, d)), EPSILON, bounces - 1, n, hit1));
				}
			}
		}
	}
	return color;
}
