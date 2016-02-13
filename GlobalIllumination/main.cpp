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
#include "SampleRay.h" 
#include "SamplePhoton.h"
#include "3dtree.h"

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
	char* depth_filename;
	bool depth_used = false;
	float depth_min=1;
	float depth_max=10;
	char* normals_filename;
	bool normals_used = false;
	int bounces = 4;
	int samples = 10; // samples (trials) per pixel in supersample OR a related multiplier when photon mapping
	int neighbors = 100;
	float photonMultiplier = 1;
	bool MCRayTracing = true;

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
		if (!strcmp(argv[argNum], "-bounces"))
		{
			//int temp = stoi(argv[argNum + 1]);
			//if (temp > 10) { cout << "More than 10 bounces not implemented.";  }
			bounces = stoi(argv[argNum + 1]);
		}
		if (!strcmp(argv[argNum], "-samples"))
		{
			samples = stoi(argv[argNum + 1]);
		}
		if (!strcmp(argv[argNum], "-photonMultiplier"))
		{
			photonMultiplier = stof(argv[argNum + 1]);
		}
		if (!strcmp(argv[argNum], "-method"))
		{
			if (strcmp((argv[argNum+1]), "mc"))
			{
				MCRayTracing = false;
			}
		}
		if (!strcmp(argv[argNum], "-neighbors"))
		{
			neighbors = stof(argv[argNum + 1]);
		}
	}

	srand(time(NULL));

	int w3 = 3 * width;
	int h3 = 3 * height;
	SceneParser sp(input_filename);
	Group* g = sp.getGroup();
	Camera* c = sp.getCamera();
	Vector3f center = c->getCenter();
	float kernel[5] = { .12f, .2336f, .2928f, .2336f, .12f };
	vector<Vector3f> supersample_colors{};
	vector<Vector3f> supersample_colors_1{}; // s_c blurred horizontally
	vector<Vector3f> supersample_colors_2{}; // s_c_1 blurred vertically
	Image imaged(width, height);
	Image imagen(width, height);
	int numMaterials = sp.getNumMaterials();
	float tmin = c->getTMin();
	int numLightSources = sp.getNumLightSources();

	SampleRay rt(&sp, bounces);
	Vector3f ambientLight = sp.getAmbientLight();
	int numLights = sp.getNumLights();

	//cout << numLightSources << " " << sp.getLightSource(0)->getPhotonRatio()*photonMultiplier*height*width;
	//int z; cin >> z;

	if (!MCRayTracing)
	{
		vector<Departure> departures;
		
		
		for (int l = 0; l < numLightSources; l++)
		{
			LightSource* lS = sp.getLightSource(l);
			for (int j = 0; j < (lS->getPhotonRatio()*photonMultiplier*height*width); j++)
			{
				Ray r = lS->emitPhoton();
				//r.getDirection().print();
				//r.getOrigin().print();
				SamplePhoton sPh = SamplePhoton(&sp, r, bounces, lS);
				sPh.mapPhoton(departures);
				1 + 1;
				int a;
			}
		}/**/

		cout << "Mapping Photons done." << endl;

		///This is a test
		/*for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				Departure d;
				d.wavelength = (int)(39 + 32 * (h + w) / (height + width));
				//cout << d.wavelength;
				d.material = sp.getMaterial(0);
				d.normal = Vector3f(0, 0, 1);
				d.colored = false;
				d.position = Vector3f(12.0f * h / height - 6 + .0001f * rand() / RAND_MAX, 12.0f * w / width - 6 + .0001f * rand() / RAND_MAX , 0 + .0001f * rand() / RAND_MAX);
				//d.position.print();
				departures.push_back(d);
			}
		}/**/

		node3D* root = node3D::create3DTree(departures);

		///vectors that store, for each pixel in the supersample, the ray for that pixel, the first hit for that ray, the t value (distance) of that hit,
		///and the MDSUB which is calculated as below
		vector<Ray> supersample_rays{};
		vector<Hit> supersample_hits{};
		vector<float> supersample_ts{};
		vector<float> supersample_MDSUB{}; 

		for (int w = 0; w < w3; w++) {
			for (int h = 0; h < h3; h++) {
				Hit hit = Hit();
				//Ray r = c->generateRay(Vector2f(2.0f*(w + (float)rand() / RAND_MAX) / w3 - 1, 2.0f*(h + (float)rand() / RAND_MAX) / h3 - 1));
				Ray r = c->generateRay(Vector2f(2.0f*(w + 0.5f) / w3 - 1, 2.0f*(h + 0.5f) / h3 - 1));
				supersample_rays.push_back(r);
				g->intersect(r, hit, tmin);
				supersample_hits.push_back(hit);
				supersample_ts.push_back(hit.getT());
			}
		}
		///hack given that we've only implemented PerspectiveCameras
		float angle = (static_cast<PerspectiveCamera*> (c))->getAngle();
		float M = std::sqrt(((1 / cos(angle)) - 1) / 2);
		float M2 = M*M;
		float N = .2f * std::sqrt(1.0f / (width*width) + 1.0f / (height*height));
		///adjust these parameters to change search stuff
		///mSDUB is (corresponding) t N / sqrt(1+M^2 ((2w/w3-1)^2+(2h/h3-1)^2)) + nearestT
		for (int w = 0; w < w3; w++) {
			for (int h = 0; h < h3; h++) {
				float nearestTDifference = FLT_MAX;
				float thisT = supersample_ts[h + h3*w];
				if (h > 0) { nearestTDifference = __min(nearestTDifference, abs(supersample_ts[h - 1 + h3 * w] - thisT)); }
				if (h < h3) { nearestTDifference = __min(nearestTDifference, abs(supersample_ts[h + 1 + h3 * w] - thisT)); }
				if (w > 0) { nearestTDifference = __min(nearestTDifference, abs(supersample_ts[h + h3 * (w - 1)] - thisT)); }
				if (w < w3) { nearestTDifference = __min(nearestTDifference, abs(supersample_ts[h + h3 * (w + 1)] - thisT)); }
				float x = (2.0f * w / w3 - 1);
				float y = (2.0f * h / h3 - 1);
				float M3 = supersample_ts[h + h3 * w] * N / std::sqrt(1 + M2 * (x*x + y*y));
				supersample_MDSUB.push_back(2*(nearestTDifference * nearestTDifference + M3 * M3)
					);
			}
		}

		/*Vector3f Table[32] = {
			Vector3f(.20f,0,.65f),Vector3f(.16f,0,.72f),Vector3f(.12f,0,.79f),Vector3f(.08f,0,.86f),
			Vector3f(.04f,0,.93f),Vector3f(0,0,1.0f), Vector3f(0, .2f, 1.0f),Vector3f(0, .4f, 1.0f),
			Vector3f(0, .6f, 1.0f),Vector3f(0, .8f, 1.0f), Vector3f(0, 1.0f, 1.0f),Vector3f(0, 1.0f, .5f),
			Vector3f(0, 1.0f, 0),Vector3f(.14f, 1.0f, 0), Vector3f(.29f, 1.0f, 0),Vector3f(.43f, 1.0f, 0),
			Vector3f(.57f, 1.0f, 0),Vector3f(.71f, 1.0f, 0), Vector3f(.86f, 1.0f, 0),Vector3f(1.0f, 1.0f, 0),
			Vector3f(1.0f, .83f, 0),Vector3f(1.0f, .67f, 0), Vector3f(1.0f, .5f, 0),Vector3f(1.0f, .33f, 0),
			Vector3f(1.0f, .17f, 0),Vector3f(1.0f, 0, 0), Vector3f(.87f, 0, 0),Vector3f(.73f, 0, 0),
			Vector3f(.6f, 0, 0),Vector3f(.47f, 0, 0), Vector3f(.33f, 0, 0),Vector3f(.2f, 0, 0) };/**/

		//cout << g->getGroupSize();
		//int z;
		//cin >> z;

		///store distances in a t-buffer. One distance per supersample! (jittered)
		///use t-buffers to determine distances between hits that represent each supersample
		///which are used as an upper bound for determining nearest neighbors
		///color is based on neighbours found in that upper bound, actual (neighbors)th largest distance
		///and sum of the radiances
		std::cout << "Out of " << w3 << " columns, ";

		int supersample_index = 0;
		for (int w = 0; w < w3; w++) {
			for (int h = 0; h < h3; h++) {
				Vector3f color{};
				Hit hit = supersample_hits[supersample_index];
				if (hit.getMaterial() == nullptr) { supersample_colors.push_back(Vector3f::ZERO); }
				else
				{
					Ray r = supersample_rays[supersample_index];
					Vector3f position = r.pointAtParameter(supersample_ts[supersample_index]);
					float mDSActual = 0;
					int neighborsFound = 0;
					float mDSUpperBound = supersample_MDSUB[supersample_index];
					vector<int> hit_neighbors{};
					node3D::nearestKNeighbors(root, position, neighbors, hit_neighbors, departures, mDSActual, mDSUpperBound, neighborsFound);

					Vector3f incoming = r.getDirection();
					Vector3f temp{};
					for (int n = 0; n < neighborsFound; n++) {
						///Only add getColor() for those whose distanceSquared is less than mDSUpperBound!!!!!!
						temp += departures[hit_neighbors[n]].getColor(incoming);
					}
					float fluxMultiplier = mDSUpperBound / (mDSActual * photonMultiplier);
					temp = temp * fluxMultiplier; // /neighbors
					color += temp; ///
					supersample_colors.push_back(color);
				}
				supersample_index++;
			}
			std::cout << "column " << w << " done." << endl;
		}
		std::cout << "Photon gathering done." << endl;
	}
	else
	{
		std::cout << "Out of " << w3 << " columns, ";

		int q = 0;
		for (int w = 0; w < w3; w++) {
			for (int h = 0; h < h3; h++) {
				Vector3f color{};
				for (int i = 0; i < samples; i++) {
					Hit hit = Hit();
					q++;
					Ray r = c->generateRay(Vector2f(2.0f*(w + (float)rand() / RAND_MAX) / w3 - 1, 2.0f*(h + (float)rand() / RAND_MAX) / h3 - 1));
					Vector3f a = rt.traceRay(r, tmin, bounces, 1, hit, q);
					color += a;
				}
				color = color / samples;
				supersample_colors.push_back(color);
			}
			std::cout << "column " << w << " done." << endl;
		}

		std::cout << "supersampling done." << endl;
	}
	if (MCRayTracing)
	{
		///filter horizontally by convolving with kernel
		for (int w = 0; w < w3; w++)
		{
			for (int h = 0; h < h3; h++)
			{
				supersample_colors_1.push_back(
					kernel[0] * supersample_colors[h + h3 * __max(0, w - 2)]
					+ kernel[1] * supersample_colors[h + h3 * __max(0, w - 1)]
					+ kernel[2] * supersample_colors[h + h3 * w]
					+ kernel[3] * supersample_colors[h + h3 * __min(w3 - 1, w + 1)]
					+ kernel[4] * supersample_colors[h + h3 * __min(w3 - 1, w + 2)]
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
		Image image(width, height);
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				Vector3f v = (supersample_colors_2[3 * h + h3 * 3 * w] + supersample_colors_2[3 * h + 1 + h3 * 3 * w] + supersample_colors_2[3 * h + 2 + h3 * 3 * w] +
					supersample_colors_2[3 * h + h3 * (3 * w + 1)] + supersample_colors_2[3 * h + 1 + h3 * (3 * w + 1)] + supersample_colors_2[3 * h + 2 + h3 * (3 * w + 1)] +
					supersample_colors_2[3 * h + h3 * (3 * w + 2)] + supersample_colors_2[3 * h + 1 + h3 * (3 * w + 2)] + supersample_colors_2[3 * h + 2 + h3 * (3 * w + 2)]
					) / 9;
				image.SetPixel(w, h, v);
			}
		}
		image.SaveImage(output_filename);
	}
	else
	{
		supersample_colors_2 = supersample_colors;
		Image image = Image(w3, h3);
		for (int w = 0; w < w3; w++)
		{
			for (int h = 0; h < h3; h++)
			{
				Vector3f v = supersample_colors_2[h + h3 * w];
				float magnitude = v.abs();
				if (magnitude > 3.0f) { v = v * 3.0f / magnitude; } 
				///clamp magnitudes because I couldn't figure out the parameters in my scene texts to get reasonable colors
				image.SetPixel(w, h, v);
			}
		}
		image.SaveImage(output_filename);
	}

	if (depth_used) { imaged.SaveImage(depth_filename); }
	if (normals_used) { imagen.SaveImage(normals_filename); }
	std::cout << "Done.";
	///pause
	std::cin >> height;
	return 0;
}

