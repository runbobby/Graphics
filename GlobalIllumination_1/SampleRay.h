#ifndef SAMPLE_RAY_H
#define SAMPLE_RAY_H

#include <cassert>
#include <vector>
#include "SceneParser.h"
#include "Ray.h"
#include "Hit.h"

class SampleRay
{
public:
  
  SampleRay()
  {
      assert( false );
  }

  SampleRay( SceneParser* scene, int max_bounces 
  );
  ~SampleRay();
  
  Vector3f traceRay( Ray& ray, float tmin, int bounces, 
                     float refr_index, Hit& hit , int q) const;
private:
  SceneParser* m_scene;
  Group* g;
  int num_lights;
  

  int m_maxBounces;
  vector<int> powers3;
  vector<int> powers2;

};

#endif // SAMPLE_RAY_H
