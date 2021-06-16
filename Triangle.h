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
	//设置三角形的顶点，将三维转化为齐次坐标
	void setVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);
	//设置相应三角形的顶点，将三维转化为齐次坐标
	void setVertex(const int& i, const Vector3f& vec);
	//设置相应顶点的颜色
	void setColor(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2);
	//一次性设置对应顶点的颜色
	void setColor(const int& i, const Vector3f& col);
	//设置对应顶点的法线
	void setNormal(const int& i, const Vector3f& n);
	//设置对应顶点的纹理坐标
	void setTexCoord(const int& i, const Vector2f& tC);

};

#endif // !TRIANGLE_H_
