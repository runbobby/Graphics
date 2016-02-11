#include "TimeStepper.hpp"

///Forward Euler method
///new ps = ps + ss*ps'
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> pS = particleSystem->getState();
	vector<Vector3f> derivative = particleSystem->evalF(pS);
	for(int i=0; i<pS.size(); i++)
	{
		pS[i] = pS[i]+stepSize*derivative[i];
	}
	particleSystem->setState(pS);
}

///Trapezoidal method
///ps1 = ps+ss*ps'
///new ps = ps + ss*(ps' + evalF(ps1))/2
void Trapezoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> pS = particleSystem->getState();
	vector<Vector3f> derivative = particleSystem->evalF(pS);
	vector<Vector3f> pS1 = pS;
	for(int i=0; i<pS.size(); i++)
	{
		pS1[i] = pS[i]+stepSize*derivative[i];
	}
	vector<Vector3f> derivative1 = particleSystem->evalF(pS1);
	vector<Vector3f> pS2 = pS;
	for(int i=0; i<pS.size(); i++)
	{
		pS[i] = pS2[i] + stepSize*(derivative[i]+derivative1[i])/2;
	}
	particleSystem->setState(pS);
}

void RK4::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> pS0 = particleSystem->getState();
	vector<Vector3f> derivative0 = particleSystem->evalF(pS0);
	vector<Vector3f> pS1 = pS0;
	for(int i=0; i<pS0.size(); i++)
	{
		pS1[i] = pS0[i]+stepSize*derivative0[i]/2;
	}
	vector<Vector3f> derivative1 = particleSystem->evalF(pS1);
	vector<Vector3f> pS2 = pS0;
	for(int i=0; i<pS0.size(); i++)
	{
		pS2[i] = pS0[i]+stepSize*derivative1[i]/2;
	}
	vector<Vector3f> derivative2 = particleSystem->evalF(pS2);
	vector<Vector3f> pS3 = pS0;
	for(int i=0; i<pS0.size(); i++)
	{
		pS3[i] = pS0[i]+stepSize*derivative2[i];
	}
	vector<Vector3f> derivative3 = particleSystem->evalF(pS3);
	vector<Vector3f> pS4 = pS0;
	for(int i=0; i<pS0.size(); i++)
	{
		pS4[i] = pS0[i]+stepSize*(derivative0[i]+2*derivative1[i]+2*derivative2[i]+derivative3[i])/6;
	}
	particleSystem->setState(pS4);
}
