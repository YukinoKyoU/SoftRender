#include "Triangle.h"

Triangle::Triangle()
{
	for (int i = 0; i < 3; i++)
	{
		vertex[i] << 0, 0, 0, 1;
		color[i] << 0, 0, 0;
		normal[i] << 0, 0, 0, 1;
		texCoord[1] << 0, 0;
	}
}
//ÉèÖÃ¶¥µã×ø±ê
void Triangle::setVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2)
{
	vertex[0] = Vector4f(v0.x(), v0.y(), v0.z(), 1.0f);
	vertex[1] = Vector4f(v1.x(), v1.y(), v1.z(), 1.0f);
	vertex[2] = Vector4f(v2.x(), v2.y(), v2.z(), 1.0f);
}
void Triangle::setVertex(const int& i, const Vector3f& ver)
{
	vertex[i] = Vector4f(ver.x(), ver.y(), ver.z(), 1.f);
}

void Triangle::setColor(const Vector3f& c0, const Vector3f& c1, const Vector3f& c2)
{
	color[0] = c0;
	color[1] = c1;
	color[2] = c2;
}
void Triangle::setColor(const int& i, const Vector3f& col)
{
	color[i] = col;
}

void Triangle::setNormal(const int& i, const Vector3f& n)
{
	normal[i] = Vector4f(n.x(), n.y(), n.z(), 0.f);
}

void Triangle::setTexCoord(const int& i, const Vector2f& tC)
{
	texCoord[i] = tC;
}