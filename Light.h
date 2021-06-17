#ifndef LIGHT_H_
#define LIGHT_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Light
{
	Light(Vector4f _pos, Vector3f i, Vector3f I) : position(_pos), intensity(i), amb_light_intensity(I) {}

	Vector4f position;
	Vector3f intensity;
	Vector3f amb_light_intensity;

};

#endif