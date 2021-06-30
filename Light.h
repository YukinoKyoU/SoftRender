#ifndef LIGHT_H_
#define LIGHT_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Light
{
	Light(Vector4f _lightPos, Vector3f _lightIntensity, Vector3f _ambLight) :
		lightPosition(_lightPos), lightIntensity(_lightIntensity), ambLightIntensity(_ambLight) {}

	Vector4f lightPosition;
	Vector3f lightIntensity;
	Vector3f ambLightIntensity;

};

#endif