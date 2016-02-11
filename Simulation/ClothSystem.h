#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{

public:
	ClothSystem(int h, int w);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	void moveCorner(Vector3f v);
	void incrementCorner();


private:
	int height, width; //should be at least 2
	float dx, mass, gravity, drag, structurek, sheark, flexionk;
	int corner_x_scale; // integer [0-110]
};


#endif
