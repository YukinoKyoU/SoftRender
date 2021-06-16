#ifndef OBJECT_H_
#define OBJECT_H_

#include <Eigen/Eigen>
#include "Triangle.h"

using namespace Eigen;

struct Object
{
	std::vector<Triangle> triangles;
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;

};


#endif // !OBJECT_H_
