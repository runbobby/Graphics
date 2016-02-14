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
	void setParameters(float mass, float gravity, float drag, float structurek, float sheark, float flexionk);


private:
	int height, width; ///should be at least 2
	float dx, mass, gravity, drag, structurek, sheark, flexionk;
	int corner_x_scale; ///integer [25-195]. 110-|c-110| is the horizontal distance between the endpoints as a proportion of the rest length
};


#endif
