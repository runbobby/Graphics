#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
	float springk, rest_length, gravity, drag, mass;
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

	void setParameters(float springk, float rest_length, float gravity, float drag, float mass);
	
};

#endif
