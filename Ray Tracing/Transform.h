#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
  Transform(const Matrix4f& mm, Object3D* obj);
  ~Transform(){
  }
  virtual bool intersect(const Ray& r, Hit& h, float tmin);

 protected:
  Object3D* o; //un-transformed object	
private:
	Matrix4f m;
	Matrix4f mInverse;
	Matrix4f mInverseT;
	Matrix3f mInverse3;
	Matrix3f mInverseT3;

};

#endif //TRANSFORM_H
