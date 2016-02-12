#include "Noise.h"
#include "PerlinNoise.h"

Vector3f Noise::getColor(const Vector3f & pos)
{
	float M = sin(frequency*pos[0] + amplitude*PerlinNoise::octaveNoise(pos, octaves));
	float M_clamped = __min(__max(-.5f, M), .5f) + .5;
	return M_clamped * color[0] + (1 - M_clamped) * color[1];
}

Noise::Noise(int _octaves,const Vector3f & color1, 
	 const Vector3f &color2,float freq,float amp):
octaves(_octaves),frequency(freq),amplitude(amp){
	color[0] = color1;
	color[1] = color2;
	init = true;
}

Noise::Noise(const Noise & n):
octaves(n.octaves),frequency(n.frequency),
	amplitude(n.amplitude),init(n.init)
{
	color[0] = n.color[0];
	color[1] = n.color[1];
}

bool Noise::valid()
{
	return init;
}

Noise::Noise():
octaves(0),init(false)
{}
