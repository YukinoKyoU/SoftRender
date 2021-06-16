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

	Triangle();

	void setVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);

	void setVertex(const int& i, const Vector3f& vec);

	void setColor(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);

	void setColor(const int& i, const Vector3f& col);
	
	void setNormal(const int& i, const Vector3f& n);
	
	void setTexCoord(const int& i, const Vector2f& tC);

};

#endif // !TRIANGLE_H_
