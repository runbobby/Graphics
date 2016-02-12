#include "SamplePhoton.h"

#define EPSILON 0.0001f

SamplePhoton::SamplePhoton(SceneParser* scene, const Ray& ray, int mb, LightSource* l):
	current_ray(ray)
{
	m_scene = scene;
	g = scene->getGroup();
	m_maxBounces = mb;
	lightSource = l;
}

void SamplePhoton::mapPhoton(vector<Departure>& departures)
{
	Departure d0;
	bool v = lightSource->isVisible();
	if (v)
	{
		d0.colored = false;
		d0.isLight = false;
		d0.wavelength = current_ray.getWavelength();
		d0.incoming = current_ray.getDirection();
		d0.position = current_ray.getOrigin();
		departures.push_back(d0);
	}
	///map throughout scene
	int bounces = 0;
	bool notYetAbsorbed = true;
	while( notYetAbsorbed && bounces<m_maxBounces)
	{
		Hit hit = Hit();
		g->intersect(current_ray, hit, EPSILON);
		Vector3f incoming = current_ray.getDirection();
		if (hit.getMaterial() == nullptr)
		{
			return;
		}
		Material* M = hit.getMaterial();
		Ray next_ray = Ray();
		Departure nextDeparture = Departure();
		bool notAbsorbed = M->interactPhoton(current_ray, hit, next_ray, nextDeparture);
		if(notAbsorbed)
		{
			departures.push_back(nextDeparture);
			current_ray = next_ray;
		}
		else
		{
			return;
		}
		bounces++;
	}
	return;
}