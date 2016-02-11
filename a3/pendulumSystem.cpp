#include "pendulumSystem.h"

// fill in code for initializing the state based on the number of particles
///This system has nP particles, 0 through nP-1. Particle 0 is fixed. Particle i is connected to
///Particle i+1 with a spring with k=5, rest length 0.5 (?). Gravity is 1. Masses are 1. 
///Drag is -.1 times the velocity.
///There are no collisions. The initial positions are random in [-5,5]^3. Initial v are 0.
///I think this IDE doesn't support uniform_real_distributions.
///m_vVecState stores position of particle i at index 2i and its velocity at 2i+1.
///force on particle i, (i>0) is sum of one or two Hooke spring forces, gravity, and drag
PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	springk=2*numParticles^2; rest_length=25.0f/numParticles/numParticles; gravity = 1; drag = 0.3f; mass = 1;

	m_vVecState.push_back(Vector3f(1,4,0));
	m_vVecState.push_back(Vector3f(0));

	for (int i = 1; i < m_numParticles; i++) {
		m_vVecState.push_back(Vector3f(((float)rand()/RAND_MAX)*10-5,((float)rand()/RAND_MAX)*10-5,((float)rand()/RAND_MAX)*10-5));
		m_vVecState.push_back(Vector3f(0));
	}
}

// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
	///Particle 0 first
	f.push_back(Vector3f(0));
	f.push_back(Vector3f(0));

	for(int i=1; i<m_numParticles - 1; i++)
	{
		f.push_back(state[2*i+1]);
		///accelerations; we add accelerations directly to find the derivative of v_i
		Vector3f springa1, springa2; 

		Vector3f r1 = state[2*i]-state[2*i-2];
		Vector3f r2 = state[2*i]-state[2*i+2];

		if(r1 == Vector3f(0)){springa1 = Vector3f(0,0,springk*rest_length/mass);}
		else {springa1 = (r1*rest_length/r1.abs()-r1)*springk/mass;}
		if(r2 == Vector3f(0)){springa2 = Vector3f(0,0,springk*rest_length/mass);}
		else {springa2 = (r2*rest_length/r2.abs()-r2)*springk/mass;}

		f.push_back(springa1+springa2+Vector3f(0,-gravity/mass,0)-drag*state[2*i+1]/mass);
	}
	if(m_numParticles>1)
	{
		int i = m_numParticles - 1;
		Vector3f springa1;
		Vector3f r1 = state[2*i]-state[2*i-2];
				
		if(r1 == Vector3f(0)){springa1 = Vector3f(0,0,springk*rest_length/mass);}
		else {springa1 = (r1*rest_length/r1.abs()-r1)*springk/mass;}
		f.push_back(state[2*i+1]);
		f.push_back(springa1+Vector3f(0,-gravity/mass,0)-drag*state[2*i+1]/mass);
	}

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[2*i];
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f+.1f*i/m_numParticles,10,10);
		glPopMatrix();
	}
}
