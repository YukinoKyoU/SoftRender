#ifndef OBJECT_H_
#define OBJECT_H_

#include <Eigen/Eigen>
#include "Triangle.h"

using namespace Eigen;

// �����࣬��������ͼԪ���
struct Object
{
	std::vector<Triangle> triangles_in_object;		//������������ͼԪ
	Vector3f object_position;						//����λ��
	Vector3f object_rotation;						//������ת
	Vector3f object_scale;							//���������
};

#endif // !OBJECT_H_
