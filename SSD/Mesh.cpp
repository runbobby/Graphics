#include "Mesh.h"

using namespace std;

///parses filename (an .obj file) and forms bindVertices, currentVertices, and faces
void Mesh::load( const char* filename )
{
	ifstream ifs(filename);
	char line[128];
	while(ifs.getline(line,128))
	{
		stringstream ss(line);
		string token;
		ss>>token;
		if(token == "v")
		{
			Vector3f v;
			ss >> v[0] >> v[1] >> v[2];
			bindVertices.push_back(v);
		}
		if(token == "f")
		{
			Tuple3u t;
			ss >> t[0] >> t[1] >> t[2];
			faces.push_back(t);
		}
	}
	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

///draw the Mesh. 
///We use geometric normals for the triangles, so the appearance is faceted
void Mesh::draw()
{
	for(int i=0; i<faces.size(); i++)
	{ 
		//calculating normal to triangle as if it were flat (no interpolation)
		Vector3f normal = Vector3f::cross(currentVertices[faces[i][0]-1]-currentVertices[faces[i][1]-1],currentVertices[faces[i][0]-1]-currentVertices[faces[i][2]-1]);
		if(normal.abs()==0){normal=Vector3f(0,0,1);}
		else{normal.normalize();}
		float nx, ny, nz;
		nx=normal.x();ny=normal.y();nz=normal.z();
		glBegin(GL_TRIANGLES);
		glNormal3f(nx,ny,nz);
		glVertex3f(currentVertices[faces[i][0]-1][0],currentVertices[faces[i][0]-1][1],currentVertices[faces[i][0]-1][2]);
		glNormal3f(nx,ny,nz);
		glVertex3f(currentVertices[faces[i][1]-1][0],currentVertices[faces[i][1]-1][1],currentVertices[faces[i][1]-1][2]);
		glNormal3f(nx,ny,nz);
		glVertex3f(currentVertices[faces[i][2]-1][0],currentVertices[faces[i][2]-1][1],currentVertices[faces[i][2]-1][2]);
		glEnd();
	}
}

///parses filename (an .attach file) to form attachments
///Expects numJoints-1 floats per line, but doesn't check!
void Mesh::loadAttachments( const char* filename, int numJoints )
{
	ifstream ifs(filename);
	char line[256]; ///some lines are long!
	while(ifs.getline(line,256))
	{
		std::vector<float> v;
		stringstream ss(line);
		float w;
		while(ss>>w)
		{
			v.push_back(w);
		}
		attachments.push_back(v);
	}
}