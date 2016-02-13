#ifndef DEPARTURE_H
#define DEPARTURE_H
#include <vecmath.h>
#include "Material.h"
#include "LightSource.h"

///This class is for Photon mapping.
///It is like an extended Hit, but I decided not to go crazy with OOP
///It represents a single instance of a photon departing from a surface i.e. not being absorbed
///A photon/ray during the mapping phase may yield many departures
///position: location of point where photon departed
///incoming: direction of ray before position
///wavelength: * 10nm. Not sure about this
///color: Starts as 0; calculated with material's BDRF, incoming, and direction to camera
///first time it's retrieved during rendering
///colored: bool for whether Departure has been retrieved yet
///Does not store outgoing direction, as our sampling method looks at departures
///It is not obvious that this sampling should yield the correct averages, but it does

struct Departure
{
	Departure() { colored = false; }
	Vector3f incoming;
	int wavelength; /// TENS OF NANOMETERS
	Vector3f position;
	Material* material;
	Vector3f color;
	bool colored;
	bool isLight;
	Vector3f normal;
	Vector3f getColor(const Vector3f& incoming);
};
#endif

