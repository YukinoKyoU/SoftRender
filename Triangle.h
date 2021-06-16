#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <Eigen/Eigen>

using namespace Eigen;

class Triangle
{
public:
	Vector4f vertex[3];		//三个顶点的坐标（齐次坐标）
	Vector3f color[3];		//三个顶点的颜色
	Vector4f normal[3];		//三个法线坐标
	Vector2f texCoord[3];	//三个纹理坐标

	Triangle();

	void setVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);

	void setVertex(const int& i, const Vector3f& vec);

	void setColor(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);

	void setColor(const int& i, const Vector3f& col);
	
	void setNormal(const int& i, const Vector3f& n);
	
	void setTexCoord(const int& i, const Vector2f& tC);

};

#endif // !TRIANGLE_H_
