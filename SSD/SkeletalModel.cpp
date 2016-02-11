#include "SkeletalModel.h"
#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

///Expects parent joints to precede children
///parses filename (a .skel file) to initialize joints
void SkeletalModel::loadSkeleton( const char* filename )
{
	ifstream ifs(filename);
	char line[128];
	while(ifs.getline(line, 128))
	{
		stringstream ss(line);
		float x, y, z;
		int index;
		ss >> x >> y >> z >> index;
		Joint* joint = new Joint;
		joint->transform = Matrix4f(1,0,0,x,0,1,0,y,0,0,1,z,0,0,0,1);
		m_joints.push_back(joint);
		if(index == -1)
		{
			m_rootJoint = joint;
		}
		else
		{
			m_joints[index]->children.push_back(joint);
		}

	}
}

///Draws a sphere at each joint using our own matrix stack and recursion
void SkeletalModel::drawJoints( )
{
	drawChildrenJoints(m_rootJoint);
}

///Draws the joint index and its children, where index's transform hasn't been applied to the stack

void SkeletalModel::drawChildrenJoints(Joint* joint)
{
	m_matrixStack.push(joint->transform);
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere( .025, 12, 12);
	for(int i = 0; i < joint->children.size(); i++)
	{
		drawChildrenJoints(joint->children[i]);
	}
	m_matrixStack.pop();
}


///draws the skeleton by drawing thin rect. prisms between joints which are connected;
///uses recursion
void SkeletalModel::drawSkeleton( )
{
	drawChildrenSkeleton(m_rootJoint);
}

///draws all bones from joints k to its parent, where k ranges over all children of joint
///except there is no bone from the root joint to its "parent" (because it doesn't have one)
void SkeletalModel::drawChildrenSkeleton( Joint* joint)
{
	///Calculation of the transform to be applied to the cube
	///uses an ad hoc method to get arbitrary perpendicular vectors which are also perpendicular to z

	if(joint != m_rootJoint)
	{
		Vector3f z = (joint->transform*Vector4f(0,0,0,1)).xyz();
		if(z != Vector3f(0,0,0))
		{
			Vector3f rnd;
			if(z.x()!=0 || z.y()!=0){rnd = Vector3f(0,0,1);}
			else{rnd = Vector3f(0,1,0);}
			Vector3f x, y;
			y = Vector3f::cross(z, rnd).normalized();
			x = Vector3f::cross(y, z).normalized();
			glLoadMatrixf(m_matrixStack.top()*
				Matrix4f(x.x(),y.x(),20*z.x(),z.x()/2,x.y(),y.y(),20*z.y(),z.y()/2,x.z(),y.z(),20*z.z(),z.z()/2,0,0,0,1));
			glutSolidCube(0.05);
		}
	}
	m_matrixStack.push(joint->transform);
	for(int i = 0; i < joint->children.size(); i++)
	{
		drawChildrenSkeleton(joint->children[i]);
	}
	m_matrixStack.pop();
}

// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
///First, reset the rotation part
///Our convention is that the rotation part equals that of rX (about X) times 
///that of rY times that of rZ, where angles are in radians (this matches the sample)
///This is pre-computed in mathematica
void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, Matrix3f::identity());

	m_joints[jointIndex]->transform = (m_joints[jointIndex]->transform)*
		Matrix4f(cos(rY)*cos(rZ),cos(rY)*sin(rZ),sin(rY),0,-cos(rZ)*sin(rX)*sin(rY)-cos(rX)*sin(rZ),cos(rX)*cos(rZ)-sin(rX)*sin(rY)*sin(rZ),cos(rY)*sin(rX),0,-cos(rX)*cos(rZ)*sin(rY)+sin(rX)*sin(rZ),-cos(rZ)*sin(rX)-cos(rX)*sin(rY)*sin(rZ),cos(rX)*cos(rY),0,0,0,0,1);
}

// 2.3.1. Implement this method to compute a per-joint transform from
// world-space to joint space in the BIND POSE.
// This method should update each joint's bindWorldToJointTransform.
///(computed only once;) uses a recursion
///We assume that this function is called before any animation.
void SkeletalModel::computeBindWorldToJointTransforms()
{
	m_matrixStack.clear();
	computeChildrenBWTJT(m_rootJoint);
}

void SkeletalModel::computeChildrenBWTJT(Joint* joint)
{
	///this expression inverts joint->transform efficiently, given assumptions
	m_matrixStack.push(Matrix4f(1,0,0,-(joint->transform(0,3)),0,1,0,-(joint->transform(1,3)),0,0,1,-(joint->transform(2,3)),0,0,0,1));
	joint->bindWorldToJointTransform = m_matrixStack.top();
	for(int i=0; i<joint->children.size(); i++)
	{
		computeChildrenBWTJT(joint->children[i]);
	}
	m_matrixStack.pop();
}

// 2.3.2. Implement this method to compute a per-joint transform from
// joint space to world space in the CURRENT POSE.
// The current pose is defined by the rotations you've applied to the
// joints and hence needs to be *updated* every time the joint angles change.
// This method should update each joint's currentJointToWorldTransform.
///uses recursion
void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	m_matrixStack.clear();
	updateChildrenCJTWT(m_rootJoint);
}

void SkeletalModel::updateChildrenCJTWT(Joint* joint)
{
	m_matrixStack.push(joint->transform);
	joint->currentJointToWorldTransform = m_matrixStack.top();
	for(int i=0; i<joint->children.size(); i++)
	{
		updateChildrenCJTWT(joint->children[i]);
	}
	m_matrixStack.pop();
}

// 2.3.2. This is the core of SSD.
// Implement this method to update the vertices of the mesh
// given the current state of the skeleton.
// You will need both the bind pose world --> joint transforms.
// and the current joint --> world transforms.
///We use j+1 because the convention for the data is that the weight of joint 0 (root) is 0
///and skipped in the data.
///omg, beware the order of operations. float*Matrix4f isn't well defined!
void SkeletalModel::updateMesh()
{
	for(int i=0; i<m_mesh.currentVertices.size(); i++)
	{
		m_mesh.currentVertices[i] = Vector3f(0,0,0);
		for(int j=0; j<m_mesh.attachments[i].size(); j++)
		{
			m_mesh.currentVertices[i] = m_mesh.currentVertices[i] + 
				(m_mesh.attachments[i][j]
				*((m_joints[j+1]->currentJointToWorldTransform)*(m_joints[j+1]->bindWorldToJointTransform)
				*Vector4f(m_mesh.bindVertices[i],1))).xyz();
		}
	}
}

