#ifndef OBJECT_H_
#define OBJECT_H_

#include <Eigen/Eigen>
#include "Triangle.h"

using namespace Eigen;

// �����࣬��������ͼԪ���
struct Object
{
	std::vector<Triangle> triangles;//������������ͼԪ
	Vector3f position;				//����λ��
	Vector3f rotation;				//������ת
	Vector3f scale;					//���������
};


#endif // !OBJECT_H_
