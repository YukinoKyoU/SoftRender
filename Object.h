#ifndef OBJECT_H_
#define OBJECT_H_

#include <Eigen/Eigen>
#include "Triangle.h"

using namespace Eigen;

// �����࣬��������ͼԪ���
struct Object
{
	std::vector<Triangle> trianglesInObject;		//������������ͼԪ
	Vector3f objectPosition;						//����λ��
	Vector3f objectRotation;						//������ת
	Vector3f objectScale;							//���������
};

#endif // !OBJECT_H_
