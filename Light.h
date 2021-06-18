#ifndef LIGHT_H_
#define LIGHT_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Light
{
	Light(Vector4f _light_pos, Vector3f _light_intensity, Vector3f _amb_light) :
		light_position(_light_pos), light_intensity(_light_intensity), amb_light_intensity(_amb_light) {}

	Vector4f light_position;
	Vector3f light_intensity;
	Vector3f amb_light_intensity;

};

#endif