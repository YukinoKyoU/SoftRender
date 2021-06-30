#ifndef OBJECT_H_
#define OBJECT_H_

#include <Eigen/Eigen>
#include "Triangle.h"

using namespace Eigen;

// 物体类，由三角形图元组成
struct Object
{
	std::vector<Triangle> trianglesInObject;		//包含的三角形图元
	Vector3f objectPosition;						//物体位置
	Vector3f objectRotation;						//物体旋转
	Vector3f objectScale;							//物体的缩放
};

#endif // !OBJECT_H_
