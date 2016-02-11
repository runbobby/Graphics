#include "Material.h"

Material::Material(const Vector3f& d_color, const Vector3f& s_color, float s) :
	diffuseColor(d_color), specularColor(s_color), shininess(s)
{

}

///Assumes dir is normalized
Vector3f Material::Shade(const Ray& ray, const Hit& hit, const Vector3f& dirToLight, const Vector3f& lightColor)
{
	///diffuse and Phong shading
	///Excuse the parentheses
	///Explanation: let clamp(t)=max(0,t), and let (a0,a1,a2)*(b0,b1,b2)=(a0 b0, a1 b1, a2 b2).
	///Then the diffuse color is clamp(dot(L,N))*color*dColor
	///the specular color is clamp(dot(L, reflected ray))^shininess*color*sColor
	///We calculate the reflected ray as ray.getDir - 2*normal*dot(normal,ray.getDir)
	///if there is texture, we use that for diffuse color
	Vector3f dc = diffuseColor;
	if (t.valid() && hit.hasTex)
	{
		dc = t(hit.texCoord[0],hit.texCoord[1]);
	}
	
	return (__max(Vector3f::dot(dirToLight, hit.getNormal()), 0))*dc*lightColor
		+ std::pow(__max(0, Vector3f::dot(dirToLight,ray.getDirection()-2*hit.getNormal()*Vector3f::dot(hit.getNormal(), ray.getDirection()))),shininess)
		*specularColor*lightColor;
}

void Material::loadTexture(const char * filename) {
	t.load(filename);
}