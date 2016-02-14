#include "ClothSystem.h"

///This system is modeled as a height by width array of particles with mass subject to gravity and drag
///(-drag*v) with structure, shear, and flexion springs with rest length based on dx.
///The state vector stores positions and velocities. r(i,j) is at index 2(i*w+j), and v(i,j) is at
///2(i*w+j)+1. Particles (0,0) and (0,w-1) are fixed at first. 

///No collision ( :( :( :(  )
ClothSystem::ClothSystem(int h, int w)
{
	height = h; width = w;
	dx = (float)6/height;
	mass=1;
	corner_x_scale=160;
	gravity=1;
	drag=2; (height+width)/30;
	structurek=height*width/2.0f;
	sheark=structurek/2;
	flexionk=structurek;
	for(int i=0; i<height;i++)
	{
		for(int j=0; j<width; j++)
		{
			m_vVecState.push_back(Vector3f(j*dx*0.5f, 4-i*dx, (float)(rand())/RAND_MAX*.01f/**/));
			m_vVecState.push_back(Vector3f(0));
		}
	}
}


///evalF is evaluated on state, not the object state. This is nonstatic because
///It's convenient to know the current object's structure constants, I think.
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> v;
	int a1[13] = {-1,0,1,0,-1,0,1,0,-1,1,1,-1,-1}; ///hard coded array of displacement amounts
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			if(i==0&j==0){v.push_back(Vector3f(0)); v.push_back(Vector3f(0));}
			else{if(i==0&j==width-1){v.push_back(Vector3f(0)); v.push_back(Vector3f(0));}
			else{
				v.push_back(state[2*(i*width+j)+1]);
///pattern		compute total spring acceleration
/*	1*/			Vector3f springa;
/* 011*/		for(int k=0; k < 4; k++)
/*00X22	*/		{
/* 332*/			///structure springs. check if hypothetical particle exists, then add spring force
/*  3 */			///if so.
					int i1 = i + a1[k];
					int j1 = j + a1[k+3];
					if(0<=i1 && i1 < height && 0<=j1 && j1 < width)
					{
						Vector3f r = state[2*(i*width+j)]-state[2*(i1*width+j1)];
						if(r == Vector3f(0)){ springa += Vector3f(0,0,structurek*dx/mass);}
						else{springa += (r*dx/r.abs()-r)*structurek/mass;}
					}
					i1 = i + a1[k+8];
					j1 = j + a1[k+9];
					if(0<=i1 && i1 < height && 0<=j1 && j1 < width)
					{
						Vector3f r = state[2*(i*width+j)]-state[2*(i1*width+j1)];
						if(r == Vector3f(0)){ springa += Vector3f(0,0,sheark*dx/mass);}
						else{springa += (r*dx*1.4142136f/r.abs()-r)*sheark/mass;}
					}
					i1 = i + 2* a1[k];
					j1 = j + 2* a1[k+3];
					if(0<=i1 && i1 < height && 0<=j1 && j1 < width)
					{
						Vector3f r = state[2*(i*width+j)]-state[2*(i1*width+j1)];
						if(r == Vector3f(0)){ springa += Vector3f(0,0,flexionk*dx/mass);}
						else{springa += (r*dx*2/r.abs()-r)*flexionk/mass;}
					}
				}
				v.push_back(springa+Vector3f(0,-gravity/mass,0)-drag*state[2*(i*width+j)+1]/mass);
			}
			}
		}
	}
	return v;

}

void ClothSystem::moveCorner(Vector3f v)
{
	m_vVecState[2*width-2] = v;
	m_vVecState[2*width-1] = Vector3f(0);
}

///Moves corner between 25 and 110 by steps of 5 (up to 110, then down to 25...)
void ClothSystem::incrementCorner()
{
	int c = corner_x_scale;
	corner_x_scale -=5;
	if(corner_x_scale <= 15){corner_x_scale = 195;}

	for(int i=0; i<m_vVecState.size(); i++)
	{
		m_vVecState[i][0]*=(float) (110-abs(corner_x_scale-110))/(110-abs(c-110));
	}
}

///We have outward facing triangles .001 away from the frame of the cloth.
///normals are an average of flat normals of certain bordering triangles
///This can probably be done more compactly, so I am sorry.

void ClothSystem::draw()
{
	GLfloat particleColor[] = {0.4f, 0.7f, 1.0f, 1.0f};
    GLfloat normalColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
	float e = .001f;
	vector<Vector3f> normals; ///normals per vertex; (i,j) is at index i*width+j
	int a1[6]={0,-1,0,1,0,-1};
	for(int i=0; i < height; i++){
		for(int j=0; j < width; j++){ 
			Vector3f n;
			for(int k=0; k < 4; k++){
				int i1 = i + a1[k+1];
				int j1 = j + a1[k];
				int i2 = i + a1[k+2];
				int j2 = j + a1[k+1];
				if(0 <= i1 && i1 < height && 0 <= j1 && j1 < width && 0 <= i2 && i2 < height && 0 <= j2 && j2 < width){
					n+=Vector3f::cross(m_vVecState[2*(i1*width+j1)]-m_vVecState[2*(i*width+j)], 
						m_vVecState[2*(i2*width+j2)]-m_vVecState[2*(i*width+j)]);
				}
			}
			if(n == Vector3f(0)){normals.push_back(Vector3f(0,0,1));}
				else{normals.push_back(n.normalized());}
			if(i>0 && j>0)
			{
				glBegin(GL_TRIANGLES);
				glFrontFace(GL_CCW);
				int l1 = (i-1)*width+j-1; int l2 = (i-1)*width+j; int l3 = i*width+j-1; int l4 = i*width+j;

				glNormal3f(-normals[l1][0],-normals[l1][1],-normals[l1][2]);
				glVertex3f(m_vVecState[2*(l1)][0]+e*normals[l1][0],m_vVecState[2*(l1)][1]+e*normals[l1][1],m_vVecState[2*(l1)][2]+e*normals[l1][2]);
				glNormal3f(-normals[l2][0],-normals[l2][1],-normals[l2][2]);
				glVertex3f(m_vVecState[2*(l2)][0]+e*normals[l2][0],m_vVecState[2*(l2)][1]+e*normals[l2][1],m_vVecState[2*(l2)][2]+e*normals[l2][2]);
				glNormal3f(-normals[l4][0],-normals[l4][1],-normals[l4][2]);
				glVertex3f(m_vVecState[2*(l4)][0]+e*normals[l4][0],m_vVecState[2*(l4)][1]+e*normals[l4][1],m_vVecState[2*(l4)][2]+e*normals[l4][2]);
				
				glNormal3f(normals[l1][0],normals[l1][1],normals[l1][2]);
				glVertex3f(m_vVecState[2*(l1)][0]-e*normals[l1][0],m_vVecState[2*(l1)][1]-e*normals[l1][1],m_vVecState[2*(l1)][2]-e*normals[l1][2]);
				glNormal3f(normals[l4][0],normals[l4][1],normals[l4][2]);
				glVertex3f(m_vVecState[2*(l4)][0]-e*normals[l4][0],m_vVecState[2*(l4)][1]-e*normals[l4][1],m_vVecState[2*(l4)][2]-e*normals[l4][2]);
				glNormal3f(normals[l2][0],normals[l2][1],normals[l2][2]);
				glVertex3f(m_vVecState[2*(l2)][0]-e*normals[l2][0],m_vVecState[2*(l2)][1]-e*normals[l2][1],m_vVecState[2*(l2)][2]-e*normals[l2][2]);
				
				
				glNormal3f(normals[l1][0],normals[l1][1],normals[l1][2]);
				glVertex3f(m_vVecState[2*(l1)][0]-e*normals[l1][0],m_vVecState[2*(l1)][1]-e*normals[l1][1],m_vVecState[2*(l1)][2]-e*normals[l1][2]);
				glNormal3f(normals[l3][0],normals[l3][1],normals[l3][2]);
				glVertex3f(m_vVecState[2*(l3)][0]-e*normals[l3][0],m_vVecState[2*(l3)][1]-e*normals[l3][1],m_vVecState[2*(l3)][2]-e*normals[l3][2]);
				glNormal3f(normals[l4][0],normals[l4][1],normals[l4][2]);
				glVertex3f(m_vVecState[2*(l4)][0]-e*normals[l4][0],m_vVecState[2*(l4)][1]-e*normals[l4][1],m_vVecState[2*(l4)][2]-e*normals[l4][2]);
				
				
				glNormal3f(-normals[l1][0],-normals[l1][1],-normals[l1][2]);
				glVertex3f(m_vVecState[2*(l1)][0]+e*normals[l1][0],m_vVecState[2*(l1)][1]+e*normals[l1][1],m_vVecState[2*(l1)][2]+e*normals[l1][2]);
				glNormal3f(-normals[l4][0],-normals[l4][1],-normals[l4][2]);
				glVertex3f(m_vVecState[2*(l4)][0]+e*normals[l4][0],m_vVecState[2*(l4)][1]+e*normals[l4][1],m_vVecState[2*(l4)][2]+e*normals[l4][2]);
				glNormal3f(-normals[l3][0],-normals[l3][1],-normals[l3][2]);
				glVertex3f(m_vVecState[2*(l3)][0]+e*normals[l3][0],m_vVecState[2*(l3)][1]+e*normals[l3][1],m_vVecState[2*(l3)][2]+e*normals[l3][2]);
				glEnd();
			}
		}
	}/**/
}

void ClothSystem::setParameters(float m, float g, float d, float str, float sh, float f)
{
	if (m > 0) { mass = m; }
	if (g > 0) { gravity = g; }
	if (d > 0) { drag = d; }
	if (str > 0) { structurek = str; }
	if (sh > 0) { sheark = sh; }
	if (f > 0) { flexionk = f; }
}