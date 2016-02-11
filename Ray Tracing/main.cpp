#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float Depth_to_float(float depthInput, float depthMin, float depthMax)
{
	return (depthMax-__max(__min(depthMax, depthInput), depthMin))/(depthMax-depthMin);
}

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
	char* input_filename;
	int width = 200;
	int height = 200;
	char* output_filename;
	char* depth_filename;
	bool depth_used = false;
	float depth_min=1;
	float depth_max=10;
	char* normals_filename;
	bool normals_used = false;

	for( int argNum = 1; argNum < argc; ++argNum )
	{

		std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
		if (!strcmp(argv[argNum], "-input"))
		{
			input_filename = argv[argNum + 1];
		}
		if (!strcmp(argv[argNum], "-size"))
		{
			width = stoi(argv[argNum + 1]);
			height = stoi(argv[argNum + 2]);
		}
		if (!strcmp(argv[argNum], "-output"))
		{
			output_filename = argv[argNum + 1];
		}
		if (!strcmp(argv[argNum], "-depth"))
		{
			depth_min = stof(argv[argNum + 1]);
			depth_max = stof(argv[argNum + 2]);
			depth_filename = argv[argNum + 3];
			depth_used = true;
		}
		if (!strcmp(argv[argNum], "-normals"))
		{
			normals_filename = argv[argNum + 1];
			normals_used = true;
		}
	}
	
  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.

	SceneParser sp(input_filename);
	Group* g = sp.getGroup();
	Camera* c = sp.getCamera();
	Vector3f center = c->getCenter();
	Image image(width, height);
	Image imaged(width, height);
	Image imagen(width, height);
	Vector3f backgroundColor = sp.getBackgroundColor();
	Vector3f ambientLight = sp.getAmbientLight();
	int numLights = sp.getNumLights();
	int numMaterials = sp.getNumMaterials();

	///We do not implement shadows, I think

	for (int w = 0; w < width; w++) {
		for (int h = 0; h < width; h++) {
//	for (int w = 124; w < 125; w++){
	//	for (int h = 76; h < 77; h++){
			Hit hit = Hit();

			Ray r = c->generateRay(Vector2f(2.0f*w / width - 1, 2.0f*h / height - 1));
			g->intersect(r, hit, c->getTMin());
			if (hit.getMaterial() == nullptr)
			{
				image.SetPixel(w, h, backgroundColor);
			}
			else
			{
				Vector3f p = r.pointAtParameter(hit.getT());
				//Vector3f color = Vector3f::ZERO;
				Vector3f dc = hit.getMaterial()->getDiffuseColor();
				if(hit.hasTex){ dc = (hit.getMaterial()->t)(hit.texCoord[0], hit.texCoord[1]); }
				Vector3f color = ambientLight*(dc);
				for (int l = 0; l < numLights; l++)
				{
					Vector3f dir, col;
					float dis;
					sp.getLight(l)->getIllumination(p, dir, col, dis);
					color += hit.getMaterial()->Shade(r, hit, dir, col);
				}
				image.SetPixel(w, h, color);/**/
				//image.SetPixel(w, h, Vector3f(1.0f, 0, 0));
				imaged.SetPixel(w, h, Vector3f(Depth_to_float(hit.getT(),depth_min,depth_max)));
				imagen.SetPixel(w, h, Vector3f(abs(hit.getNormal()[0]), abs(hit.getNormal()[1]), abs(hit.getNormal()[2])));
			}
		}
	}
	image.SaveImage(output_filename);
	if (depth_used) { imaged.SaveImage(depth_filename); }
	if (normals_used) { imagen.SaveImage(normals_filename); }
 
  /*TODO: below demonstrates how to use the provided Image class
  Should be removed when you start
  Vector3f pixelColor (1.0f,0,0);
  width and height
  Image image( 10 , 15 );
  image.SetPixel( 5,5, pixelColor );
  image.SaveImage("demo.bmp");*/
	cout << "Done.";
	cin >> height;
	return 0;
}

