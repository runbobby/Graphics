#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object3D* 

class Group:public Object3D
{
public:

  Group(){

  }
	
  Group(int n){ num_objects = n; objects = vector<Object3D*>(n, nullptr); }

  ~Group(){
   
  }

  virtual bool intersect(const Ray& r, Hit& h, float tmin);

  void addObject( int i , Object3D* obj ){
	  objects[i] = obj;
  }

  int getGroupSize();

 private:
	 vector<Object3D*> objects;
	 int num_objects;
};

#endif
	
