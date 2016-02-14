#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "vecmath.h"
#include <vector>
#include "particleSystem.h"

///updates particleSystem's state vector to the estimation of its state after a time step of stepSize
class TimeStepper
{
public:
	virtual void takeStep(ParticleSystem* particleSystem,float stepSize)=0;
};


class ForwardEuler:public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

class Trapezoidal:public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

/////////////////////////

//Provided
///freakin incompatible with VS2015. I had to install VS2010 on another computer to get this darned
///stepper to work. Let students do the work! (Being serious here, this is an uncommon instance in 
///which it probably would have been better here.)
class RK4:public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

#endif
