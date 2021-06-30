
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "Triangle.h"
#include "Camera.h"

constexpr float PI = 3.1415926;

static float angleToRadian(float angle)
{
	return (angle / 180) * PI;
}

//�ж����ص��Ƿ�����������
static bool isInsideTriangle(const float x, const float y, const Triangle& t)
{
	//�������ص㣨��Σ�����
	Vector3f temPixel(x, y, 1.0f);
	//����������������������꣨��zֵ��һ��
	std::vector<Vector3f> vertexArray(3);
	vertexArray[0] = { t.vertex[0].x(), t.vertex[0].y(), 1.0f };
	vertexArray[1] = { t.vertex[1].x(), t.vertex[1].y(), 1.0f };
	vertexArray[2] = { t.vertex[2].x(), t.vertex[2].y(), 1.0f };

	//���������ߵ���������ʱ�뷽��
	Vector3f edge1, edge2, edge3;
	edge1 = vertexArray[1] - vertexArray[0];
	edge2 = vertexArray[2] - vertexArray[1];
	edge3 = vertexArray[0] - vertexArray[2];

	//���ص㵽�������������
	Vector3f p2Ver1, p2Ver2, p2Ver3;
	p2Ver1 = temPixel - vertexArray[0];
	p2Ver2 = temPixel - vertexArray[1];
	p2Ver3 = temPixel - vertexArray[2];

	//������ڲ������������������Ĳ��ȫ��ͬ��
	if (p2Ver1.cross(edge1).z() > 0 && p2Ver2.cross(edge2).z() > 0 && p2Ver3.cross(edge3).z() > 0
		|| p2Ver1.cross(edge1).z() < 0 && p2Ver2.cross(edge2).z() < 0 && p2Ver3.cross(edge3).z() < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//��һ�������������
static std::tuple<float, float, float> Barycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

//���������ֵ������ȶ�ά���꣩
static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//���������ֵ����ɫ����ά���꣩
static Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Vector3f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//���������ֵ�����꣬���ߵ����ά���꣩
static Vector4f interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//���������ֵ�����ֵ��
static float interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}

static void ViewingFrustumPlanes(std::vector<Vector4f>& result, const Matrix4f & vp)
{
	
	//���  
	result[0].x() = vp(3, 0) + vp(0, 0);
	result[0].y() = vp(3, 1) + vp(0, 1);
	result[0].z() = vp(3, 2) + vp(0, 2);
	result[0].w() = vp(3, 3) + vp(0, 3);
	//�Ҳ�
	result[1].x() = vp(3, 0) - vp(0, 0);
	result[1].y() = vp(3, 1) - vp(0, 1);
	result[1].z() = vp(3, 2) - vp(0, 2);
	result[1].w() = vp(3, 3) - vp(0, 3);
	//�ϲ�
	result[2].x() = vp(3, 0) - vp(1, 0);
	result[2].y() = vp(3, 1) - vp(1, 1);
	result[2].z() = vp(3, 2) - vp(1, 2);
	result[2].w() = vp(3, 3) - vp(1, 3);
	//�²�
	result[3].x() = vp(3, 0) + vp(1, 0);
	result[3].y() = vp(3, 1) + vp(1, 1);
	result[3].z() = vp(3, 2) + vp(1, 2);
	result[3].w() = vp(3, 3) + vp(1, 3);
	//Near
	result[4].x() = vp(3, 0) + vp(2, 0);
	result[4].y() = vp(3, 1) + vp(2, 1);
	result[4].z() = vp(3, 2) + vp(2, 2);
	result[4].w() = vp(3, 3) + vp(2, 3);
	//Far
	result[5].x() = vp(3, 0) - vp(2, 0);
	result[5].y() = vp(3, 1) - vp(2, 1);
	result[5].z() = vp(3, 2) - vp(2, 2);
	result[5].w() = vp(3, 3) - vp(2, 3);
}

/*
static void ViewingFrustumPlanes(std::vector<Vector4f>& result, const Matrix4f& vp)
{

	//���  
	result[0].x() = vp(0, 3) + vp(0, 0);
	result[0].y() = vp(1, 3) + vp(1, 0);
	result[0].z() = vp(2, 3) + vp(2, 0);
	result[0].w() = vp(3, 3) + vp(3, 0);
	//�Ҳ�
	result[1].x() = vp(0, 3) - vp(0, 0);
	result[1].y() = vp(1, 3) - vp(1, 0);
	result[1].z() = vp(2, 3) - vp(2, 0);
	result[1].w() = vp(3, 3) - vp(3, 0);
	//�ϲ�
	result[2].x() = vp(0, 3) - vp(0, 1);
	result[2].y() = vp(1, 3) - vp(1, 1);
	result[2].z() = vp(2, 3) - vp(2, 1);
	result[2].w() = vp(3, 3) - vp(3, 1);
	//�²�
	result[3].x() = vp(0, 3) + vp(0, 1);
	result[3].y() = vp(1, 3) + vp(1, 1);
	result[3].z() = vp(2, 3) + vp(2, 1);
	result[3].w() = vp(3, 3) + vp(3, 1);
	//Near
	result[4].x() = vp(0, 3) + vp(0, 2);
	result[4].y() = vp(1, 3) + vp(1, 2);
	result[4].z() = vp(2, 3) + vp(2, 2);
	result[4].w() = vp(3, 3) + vp(3, 2);
	//Far
	result[5].x() = vp(0, 3) - vp(0, 2);
	result[5].y() = vp(1, 3) - vp(1, 2);
	result[5].z() = vp(2, 3) - vp(2, 2);
	result[5].w() = vp(3, 3) - vp(3, 2);
}
*/

static bool Point2Plane(const Vector3f & v, const Vector4f& p) {
	//�㵽ƽ����� d =  Ax + By + Cz + D;
	// d < 0 ����ƽ�淨�򷴷�����ָ������
	// d > 0 ����ƽ�淨����ָ������
	// d = 0 ��ƽ����
	// d < 0Ϊ false
	return p.x() * v.x() + p.y() * v.y() + p.z() * v.z() + p.w() >= 0;
}
#endif // !GLOBAL_H_
