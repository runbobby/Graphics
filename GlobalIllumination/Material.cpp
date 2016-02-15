#include "Material.h"
Material::Material(const vector<float>& dRatio, const vector<float>& aRatio, const Vector3f& d_color, const Vector3f& s_color, float s,
	float r, Vector3f l, float b, float c) :
	diffuseColor(d_color), refractionIndex(r), shininess(s), specularColor(s_color), lightColor(l), bCoefficient(b), cCoefficient(c), diffuseRatio(dRatio), absorbRatio(aRatio)
{
}

Material::~Material(){}

Vector3f Material::getDiffuseColor() const { return  diffuseColor;}

Vector3f Material::Shade( const Ray& ray, const Hit& hit, const Vector3f& dirToLight, const Vector3f& lightColor )
{
    Vector3f kd;
	if(t.valid() && hit.hasTex){
		Vector2f texCoord = hit.texCoord;
		Vector3f texColor = t(texCoord[0],texCoord[1]);
		kd = texColor;
	}
	else{kd = this->diffuseColor;}
	Vector3f n = hit.getNormal().normalized();
	
	///Diffuse + Specular Shading
	
	if(noise.valid())
	{
		kd = noise.getColor(ray.getOrigin()+ray.getDirection()*hit.getT());
	}
	Vector3f outgoing = ray.getDirection() + 2 * n * Vector3f::dot(n, ray.getDirection());
	Vector3f color = clampedDot(dirToLight, n)*(lightColor*kd) + specularColor * lightColor * std::pow(clampedDot(outgoing, dirToLight), shininess) / shininess;
	return color;
}

Vector3f Material::pointwiseDot( const Vector3f& v1 , const Vector3f& v2 ) {
	Vector3f out = Vector3f( v1[0]*v2[0] , v1[1]*v2[1] , v1[2]*v2[2]);
	return out;
}

float Material::clampedDot( const Vector3f& L , const Vector3f& N )const {
	float d = Vector3f::dot(L,N);
	return (d>0)? d : 0 ; 
}
void Material::loadTexture(const char * filename){
	t.load(filename);
}
float Material::getRefractionIndex(){
	return refractionIndex;
}
Vector3f Material::getDiffuseColor(){
	return diffuseColor;
}
Vector3f Material::getSpecularColor(){
	return specularColor;
}
float Material::getShininess()
{
	return shininess;
}

///for convenience, this is already multiplied by cos(theta_i)
///input: dot(i,N), dot(N, r) > 0. This means N may be the negative of the normal
///i is the incoming ray, negated. r is the reflected ray. All are normalized
///note: The mathematically correct multiplier seems to be shininess+1 in the case which is not close to the edge

Vector3f Material::BDRF(Vector3f i, Vector3f N, Vector3f r)
{
	float in = Vector3f::dot(i, N);
	return in*diffuseColor + std::pow(Vector3f::dot(r, -i + 2 * N*in), shininess)*specularColor*shininess;
}

Vector3f Material::diffuseBDRF(Vector3f i, Vector3f N, Vector3f r)
{
	float in = Vector3f::dot(i, N);
	return in*diffuseColor;
}

Vector3f Material::specularBDRF(Vector3f i, Vector3f N, Vector3f r)
{
	float in = Vector3f::dot(i, N);
	return std::pow(Vector3f::dot(r, -i + 2 * N*in), shininess)*specularColor*shininess;
}


Vector3f Material::getLightColor(){ return lightColor; }

void Material::setNoise(const Noise & n)
{
	noise=n;
}

//==========================================================================
Vector3f mirrorDirection(const Vector3f& normal, const Vector3f& incoming);
/*{
	Vector3f n = normal.normalized();
	float ni = Vector3f::dot(n, incoming);
	return incoming - 2 * ni * n;
	//return ((ni < 0) ? incoming - 2 * ni * n : incoming - 2 * ni * n);
}/**/

bool transmittedDirection(const Vector3f& normal, const Vector3f& incoming, float n, float nt, Vector3f& transmitted);
/*{
	Vector3f normal2 = normal.normalized();
	float in = Vector3f::dot(normal2, incoming);
	float r = n / nt;
	if (in < 0)
	{
		float a = 1 - r*r*(1 - in*in);
		if (a < 0) { return false; }
		transmitted = r*(incoming - in * normal2) - normal2 * std::sqrt(a);
		return true;
	}
	else
	{
		float a = 1 - r*r*(1 - in*in);
		if (a < 0) { return false; }
		transmitted = r*(incoming - in * normal2) + normal2 * std::sqrt(a);
		return true;
	}
}/**/

Vector3f randomDirection(const Vector3f& normal);/*
{
	float u = 2.0f * rand() / RAND_MAX - 1;
	float t = 6.2831853f * rand() / RAND_MAX;
	float a = std::sqrt(1 - u*u);
	Vector3f v{ a*cos(t), a*sin(t), u };
	if (Vector3f::dot(v, normal)>0) { return v; }
	else { return -v; }
}/**/

Vector3f randomDiffuseDirection(const Vector3f& normal);
/*{
	float u = ((float)rand() / RAND_MAX);
	u = std::sqrt(u);
	float t = 6.2831853f * rand() / RAND_MAX;
	float d = std::sqrt(1 - u*u);
	Vector3f v{ d*cos(t), d*sin(t), u };
	float a = normal[0];
	float b = normal[1];
	float c = normal[2];
	float bb = b*b;
	float aa = a*a;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa+f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;
	return Matrix3f(0, f * u2, a, -c * u1, -a*b*u2, b, b * u1, -a*c*u2, c)*v;
}/**/

Vector3f randomSpecularDirection(const Vector3f& normal, const Vector3f& i, float s);
/*{
	Vector3f o = mirrorDirection(normal, i);
	float u = ((float)rand() / RAND_MAX);
	u = std::pow(u, 1/(1+s));
	float t = 6.2831853f * rand() / RAND_MAX;
	float d = std::sqrt(1 - u*u);
	Vector3f v{ d*cos(t), d*sin(t), u };
	float a = i[0];
	float b = i[1];
	float c = i[2];
	float bb = b*b;
	float aa = a*a;
	float cc = c*c;
	float f = bb + cc;
	float t1 = sqrt(f);
	float t2 = sqrt((aa + f)*f);
	float u1 = 1 / t1;
	float u2 = 1 / t2;
	v = Matrix3f(0, f * u2, a, -c * u1, -a*b*u2, b, b * u1, -a*c*u2, c)*v;
	if (Vector3f::dot(v, normal) >= 0) { return v; }
	else { return v - 2 * normal * Vector3f::dot(normal, v); }
}/**/


bool Material::interactPhoton(const Ray& incoming, const Hit& hit, Ray& outgoing, Departure& d)
{
	int w = incoming.getWavelength();
	float tt = hit.getT();
	Vector3f i = incoming.getDirection();
	Vector3f normal = hit.getNormal();
	bool outside = Vector3f::dot(i, normal)<0;///True: From outside. False: from inside
	Vector3f position = incoming.pointAtParameter(tt);
	float c1 = diffuseRatio[w-39];
	float c2 = c1 + absorbRatio[w-39];

	float random = (float) rand()/RAND_MAX;
	if(random <= c1) /// reflected diffusely
	{
		d.colored=false;
		d.isLight=false;
		d.incoming = i;
		d.material = this;
		d.position = position;
		d.wavelength = w;
		d.normal = normal;
		if(outside)
		{ 
			outgoing = Ray(position, randomDiffuseDirection(normal), w); 
		}
		else
		{
			outgoing = Ray(position, randomDiffuseDirection(-normal), w); 
		}
		return true;
	}
	else if(random <= c2 + .000001f) /// absorbed; epsilon to avoid errors
	{
		return false;
	}
	else /// reflected specularly or refractively transmitted; we assume that b,c coefficients give valid answers
	{
		d.colored=false;
		d.isLight=false;
		d.incoming = i;
		d.material = this;
		d.position = position;
		d.wavelength = w;
		d.normal = normal;
		if(outside)
		{
			float m = bCoefficient + cCoefficient /( w*w);
			Vector3f tr(0);
			bool b = transmittedDirection(normal, i, 1, m, tr);
			if (b) ///can transmit
			{
				float rand1 = (float)rand()/RAND_MAX;
				float r0 = (1 - 2 * m + m*m) / (1 + 2 * m + m*m);
				float c = ((1 <= m) ? abs(Vector3f::dot(i, normal)) : abs(Vector3f::dot(tr, normal)));
				float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
				if(rand1 < reflection_weight) /// reflect
				{
					outgoing = Ray(position,
						mirrorDirection(normal, i)
						//randomSpecularDirection(-normal, i, shininess)
						, w);
				}
				else
				{
					outgoing = Ray(position, tr, w);
				}
			}
			else ///total internal reflection
			{
				outgoing = Ray(position,
					mirrorDirection(normal, i)
					//randomSpecularDirection(-normal, i, shininess)
					, w);
			}
		}
		else ///inside
		{
			float m = bCoefficient + cCoefficient / (w*w);
			Vector3f tr = Vector3f::ZERO;
			bool b = transmittedDirection(normal, i, m, 1, tr);
			if (b)
			{
				float rand1 = (float)rand()/RAND_MAX;
				float r0 = (1 - 2 * m + m*m) / (1 + 2 * m + m*m);
				float c = ((m <= 1) ? abs(Vector3f::dot(i, normal)) : abs(Vector3f::dot(tr, normal)));
				float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
				if(rand1 < reflection_weight) /// reflect
				{
					outgoing = Ray(position, 
						mirrorDirection(normal, i)
						//randomSpecularDirection(-normal, i, shininess)
						, w); 
				}
				else
				{
					outgoing = Ray(position, tr, w);
				}
			}
			else
			{
				outgoing = Ray(position, 
					mirrorDirection(normal, i)
					//randomSpecularDirection(-normal, i, shininess)
					,w);
			}
		}/**/
		return true;
	}
}

///hard coded spectral RGB values.
Vector3f Material::RGBTable[32] = {
	Vector3f(.4f,0,1.3f),Vector3f(.32f,0,1.44f),Vector3f(.24f,0,1.58f),Vector3f(.16f,0,1.72f),
	Vector3f(.08f,0,1.86f),Vector3f(0,0,2.0f), Vector3f(0, .39, 1.96f),Vector3f(0, .74f, 1.86f),
	Vector3f(0, 1.03f, 1.71f),Vector3f(0, 1.25f, 1.56f), Vector3f(0, 1.41f, 1.42f),Vector3f(0, 1.6f, 1.2f),
	Vector3f(0, 1.79f, .89f),Vector3f(0, 1.94f, .49f), Vector3f(0, 2.0f, 0),Vector3f(.39f, 1.96f, 0),
	Vector3f(.74f, 1.86f, 0),Vector3f(1.03f, 1.71f, 0), Vector3f(1.25f, 1.56f, 0),Vector3f(1.42f, 1.41f, 0),
	Vector3f(1.54f, 1.28f, 0),Vector3f(1.66f, 1.11f, 0), Vector3f(1.79f, .89f, 0),Vector3f(1.9f, .63f, 0),
	Vector3f(1.97f, .34f, 0),Vector3f(2.0f, 0, 0), Vector3f(1.74f, 0, 0),Vector3f(1.46f, 0, 0),
	Vector3f(1.2f, 0, 0),Vector3f(.94f, 0, 0), Vector3f(.66f, 0, 0),Vector3f(.4f, 0, 0) };

Vector3f Material::wavelengthToColor(int w)
{
	/*Vector3f RGBTable[] = { 
		Vector3f(.20f,0,.65f),Vector3f(.16f,0,.72f),Vector3f(.12f,0,.79f),Vector3f(.08f,0,.86f), 
		Vector3f(.04f,0,.93f),Vector3f(0,0,1.0f), Vector3f(0, .2f, 1.0f),Vector3f(0, .4f, 1.0f),
		Vector3f(0, .6f, 1.0f),Vector3f(0, .8f, 1.0f), Vector3f(0, 1.0f, 1.0f),Vector3f(0, 1.0f, .5f),
		Vector3f(0, 1.0f, 0),Vector3f(.14f, 1.0f, 0), Vector3f(.29f, 1.0f, 0),Vector3f(.43f, 1.0f, 0),
		Vector3f(.57f, 1.0f, 0),Vector3f(.71f, 1.0f, 0), Vector3f(.86f, 1.0f, 0),Vector3f(1.0f, 1.0f, 0),
		Vector3f(1.0f, .83f, 0),Vector3f(1.0f, .67f, 0), Vector3f(1.0f, .5f, 0),Vector3f(1.0f, .33f, 0),
		Vector3f(1.0f, .17f, 0),Vector3f(1.0f, 0, 0), Vector3f(.87f, 0, 0),Vector3f(.73f, 0, 0),
		Vector3f(.6f, 0, 0),Vector3f(.47f, 0, 0), Vector3f(.33f, 0, 0),Vector3f(.2f, 0, 0) };/**/
	return Material::RGBTable[w - 39];
}

///returns color of photon packet (it is proportional to the photon's spectral RGB values)
///assumes perspectivecamera starts in air
///uses diffuse ratio, absorb ratio, and refraction index that corresponds to the departure's wavelength
///Note: diffuseRatio, absorbRatio are size 32 vectors, and w ranges from 39 to 70 (10s of nm)
///Because of our sampling method, we multiply by ratios that assume that the photon is not absorbed
///i.e. .1 diffuse, .8 absorb => .5 diffuse, .5 specular/transmit
Vector3f Material::radiance(const Departure& d, const Vector3f& outgoing)
{

	int w = d.wavelength;
	Vector3f i = d.incoming;
	Vector3f N = d.normal;
	float in = Vector3f::dot(i, N);
	bool outside = in <= 0;///True: Departure is From outside. False: from inside
	Vector3f p = d.position;
	float c2 = absorbRatio[w-39];
	float c1 = diffuseRatio[w - 39];
	float c3 = 1 - c1 - c2; ///sum specular, transmission ratio
	c1 = c1 / (1 - c2); ///I should have c1 = c1/(2 Pi) so that the hemispherical integral is 1, but I can't get the parameters right yet.
	c3 = c3 / (1 - c2);
	if (bCoefficient < 0)
	{///non transparent case
		return wavelengthToColor(w) * (c3 * std::pow(__max(0,Vector3f::dot(outgoing, i - 2 * N*in)), shininess) * shininess
			+ c1);
	}
	if(outside)
	{
		float m = bCoefficient + cCoefficient / (w*w);
		Vector3f tr{};
		bool b = transmittedDirection(N, i, m, 1, tr);
		if (b)
		{ ///can transmit, so the reflection component is not fully added
			float r0 = (1 - 2 * m + m*m) / (1 + 2 * m + m*m);
			float c = ((1 <= m) ? abs(Vector3f::dot(i, N)) : abs(Vector3f::dot(tr, N)));
			float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
			return wavelengthToColor(w) * (c3 * std::pow(__max(0, Vector3f::dot(outgoing, i - 2 * N*in)), shininess) * shininess * reflection_weight
				+ c1);
		}
		else
		{ ///can't transmit, so reflection component is fully added
			return wavelengthToColor(w) * (c3 * std::pow(__max(0, Vector3f::dot(outgoing, i - 2 * N*in)), shininess) * shininess
				+ c1);
		}
	}
	else
	{
		float m = bCoefficient + cCoefficient / (w*w);
		Vector3f tr{};
		bool b = transmittedDirection(N, i, m , 1, tr);
		if (b)
		{ ///can transmit, so the there is a transmitted component
			float r0 = (1 - 2 * m + m*m) / (1 + 2 * m + m*m);
			float c = ((m <= 1) ? abs(Vector3f::dot(i, N)) : abs(Vector3f::dot(tr, N)));
			float reflection_weight = r0 + (1 - r0)*std::pow(1 - c, 5);
			return c3*(1 - reflection_weight)*wavelengthToColor(w)* std::pow(__max(0, Vector3f::dot(outgoing, tr)), shininess) * shininess;
		}
		else
		{ ///can't transmit. nothing.
			return Vector3f::ZERO;
		}
	}
	return Vector3f::ZERO;

}