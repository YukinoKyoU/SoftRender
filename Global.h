
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "Triangle.h"
#include "Camera.h"

constexpr float PI = 3.1415926;

static float angleToRadian(float angle)
{
	return (angle / 180) * PI;
}

static Eigen::Matrix4f getViewMatrix(const Camera& c)
{
	Matrix4f camera_trans;			//�ƶ�����
	Matrix4f camera_rotation;		//��ת����

	Vector3f w = -c.lookAt_dir.normalized();	//�������z��
	Vector3f v = c.up_dir;						//�������y��
	Vector3f u = v.cross(w);					//�������x��

	camera_trans << 1, 0, 0, -c.camera_position.x(),
		0, 1, 0, -c.camera_position.y(),
		0, 0, 1, -c.camera_position.z(),
		0, 0, 0, 1;

	camera_rotation << u.x(), u.y(), u.z(), 0,
		v.x(), v.y(), v.z(), 0,
		w.x(), w.y(), w.z(), 0,
		0, 0, 0, 1;

	//������˼������ͼ����
	return  camera_rotation * camera_trans;
}

static Eigen::Matrix4f getProjectionMatrix(const Camera& c)
{
	//͸��ͶӰ����
	Matrix4f perspective;							//��͸����׶ת����������ռ��У�͸��ͶӰ��
	Matrix4f orthoTrans, orthoScale, ortho;			//��������ռ�ͶӰ����μ��ÿռ���

	float t, r;			//��ƽ����ϱ߽���ұ߽�

	float radFov = angleToRadian(c.fov);
	t = tan(radFov / 2) * c.near;
	r = c.aspectRatio * t;

	float n = c.near;
	float f = c.far;

	perspective << n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, -1, 0;

	orthoTrans << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, -(n + f) / 2,
		0, 0, 0, 1;

	orthoScale << 1 / r, 0, 0, 0,
		0, 1 / t, 0, 0,
		0, 0, 2 / (n - f), 0,
		0, 0, 0, 1;

	ortho = orthoScale * orthoTrans;

	//������˼����͸��ͶӰ����
	return ortho * perspective;
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

#endif // !GLOBAL_H_
