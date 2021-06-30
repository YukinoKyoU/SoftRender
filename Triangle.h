#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <Eigen/Eigen>

using namespace Eigen;

class Triangle
{
public:
	Vector4f vertex[3];		//������������꣨������꣩
	Vector3f color[3];		//�����������ɫ
	Vector4f normal[3];		//������������
	Vector2f texCoord[3];	//������������

	Vector4f vertexInWorld[3];
	Vector4f vertexInView[3];
	Vector4f vertexInClip[3];


	Triangle();
	//���������εĶ��㣬����άת��Ϊ�������
	void setVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);
	//������Ӧ�����εĶ��㣬����άת��Ϊ�������
	void setVertex(const int& i, const Vector3f& vec);
	//������Ӧ�������ɫ
	void setColor(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);
	//һ�������ö�Ӧ�������ɫ
	void setColor(const int& i, const Vector3f& col);
	//���ö�Ӧ����ķ���
	void setNormal(const int& i, const Vector3f& n);
	//���ö�Ӧ�������������
	void setTexCoord(const int& i, const Vector2f& tC);

};

#endif // !TRIANGLE_H_
