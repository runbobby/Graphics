#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <ctime>
#include <random>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "RayTracer.h"

using namespace std;

///We assume that shadows, jitters, and filters are always on

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
	/*char* depth_filename;
	bool depth_used = false;
	float depth_min=1;
	float depth_max=10;
	char* normals_filename;
	bool normals_used = false;/**/
	int bounces = 4;

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
		/*if (!strcmp(argv[argNum], "-depth"))
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
		}/**/
		if (!strcmp(argv[argNum], "-bounces"))
		{
			bounces = stoi(argv[argNum + 1]);
		}
	}

	srand(time(NULL));

	int w3 = 3 * width;
	int h3 = 3 * height;
	SceneParser sp(input_filename);
	Group* g = sp.getGroup();
	Camera* c = sp.getCamera();
	Vector3f center = c->getCenter();
	Image image(width, height);
	float kernel[5] = { .12f, .2336f, .2928f, .2336f, .12f };
	vector<Vector3f> supersample_colors{};
	vector<Vector3f> supersample_colors_1{}; // s_c blurred horizontally
	vector<Vector3f> supersample_colors_2{}; // s_c_1 blurred vertically
	Image imaged(width, height);
	Image imagen(width, height);
	Vector3f ambientLight = sp.getAmbientLight();
	int numLights = sp.getNumLights();
	int numMaterials = sp.getNumMaterials();
	RayTracer rt(&sp, bounces);
	float tmin = c->getTMin();

	cout << "Out of " << w3 << " columns, ";

	///For each pixel in the 3x supersample, generate a ray to a random point in the pixel 
	///Find the color in that direction with traceRay (where the real work of this assignment is)

	for (int w = 0; w < w3; w++) {
		for (int h = 0; h < h3; h++) {
			Hit hit = Hit();
			Ray r = c->generateRay(Vector2f(2.0f*(w + (float)rand()/RAND_MAX ) / w3 - 1, 2.0f*(h + (float)rand() / RAND_MAX) / h3 - 1));
			Vector3f color(0);
			color = rt.traceRay(r, tmin, bounces, 1, hit);
			supersample_colors.push_back(color);
		}
		if (w % 20 == 0) { cout << "column " << w << " done." << endl; }
	}

	cout << "supersampling done." << endl;

	///filter horizontally by convolving with kernel
	for (int w = 0; w < w3; w++)
	{
		for (int h = 0; h < h3; h++)
		{
			supersample_colors_1.push_back(
				kernel[0] * supersample_colors[h + h3 * __max(0, w-2)]
				+ kernel[1] * supersample_colors[h + h3 * __max(0, w-1)]
				+ kernel[2] * supersample_colors[h + h3 * w]
				+ kernel[3] * supersample_colors[h + h3 * __min(w3-1, w+1)]
				+ kernel[4] * supersample_colors[h + h3 * __min(w3-1, w+2)]
				);
		}
	}

	///filter vertically by kernel
	for (int w = 0; w < w3; w++)
	{
		for (int h = 0; h < h3; h++)
		{
			supersample_colors_2.push_back(
				kernel[0] * supersample_colors_1[__max(0, h - 2) + h3 * w]
				+ kernel[1] * supersample_colors_1[__max(0, h - 1) + h3 * w]
				+ kernel[2] * supersample_colors_1[h + h3 * w]
				+ kernel[3] * supersample_colors_1[__min(h3 - 1, h + 1) + h3 * w]
				+ kernel[4] * supersample_colors_1[__min(h3 - 1, h + 2) + h3 * w]
				);
		}
	}

	///average colors in a 3x3 square to get the final image
	///I wrote out everything to save like .0001 seconds.
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			image.SetPixel(w, h,
				(supersample_colors_2[3 * h + h3 * 3 * w] + supersample_colors_2[3 * h + 1 + h3 * 3 * w] + supersample_colors_2[3 * h + 2 + h3 * 3 * w] +
					supersample_colors_2[3 * h + h3 * (3 * w + 1)] + supersample_colors_2[3 * h + 1 + h3 * (3 * w + 1)] + supersample_colors_2[3 * h + 2 + h3 * (3 * w + 1)] +
					supersample_colors_2[3 * h + h3 * (3 * w + 2)] + supersample_colors_2[3 * h + 1 + h3 * (3 * w + 2)] + supersample_colors_2[3 * h + 2 + h3 * (3 * w + 2)]
					) / 9);
		}
	}

	image.SaveImage(output_filename);
	//if (depth_used) { imaged.SaveImage(depth_filename); }
	//if (normals_used) { imagen.SaveImage(normals_filename); }
 
	cout << "Done.";
	///pause
	cin >> height;
	return 0;
}

