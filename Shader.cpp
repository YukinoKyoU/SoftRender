#include "Shader.h"


void Shader::setColor(const Vector3f& c)
{
	color = c;
}
Vector3f Shader::baseVertexColor()
{
	return color;
}

void Shader::setNormal(const Vector4f& n)
{
	normal =  { n.x(), n.y(), n.z() };
}

Vector3f Shader::normalShader()
{
	Vector3f color = (normal.normalized() + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
	return color * 255;
}