#ifndef SAMPLE_PHOTON_H
#define SAMPLE_PHOTON_H
#include "Departure.h"
#include <vector>
#include "SceneParser.h"


class SamplePhoton
{
public:
	SamplePhoton(SceneParser* scene, const Ray& ray, int mb, LightSource* lS);
	void mapPhoton(vector<Departure>& departures);
private:
	Ray current_ray;
	int m_maxBounces;
	SceneParser* m_scene;
	Group* g;
	LightSource* lightSource;
};
#endif

