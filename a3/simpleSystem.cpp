
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem()
{
	m_vVecState = vector<Vector3f>();
	m_vVecState.push_back(Vector3f(1,0,0)); //hard coded start position
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f = vector<Vector3f>();
	f.push_back(Vector3f(-state[0][1],state[0][0],0));

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
		Vector3f pos = m_vVecState[0];
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075,10.0,10.0);
		glPopMatrix();
}
