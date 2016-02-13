#include "Departure.h"

///store color vector with departure once calculated for the first time

Vector3f Departure::getColor(const Vector3f& incoming)
{
	if (colored)
	{
		return color;
	}
	else
	{
		Vector3f c = material->radiance(*this, -incoming);
		colored = true;
		color = c;
		return c;
	}
}