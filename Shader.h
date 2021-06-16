#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>

using namespace Eigen;

class Shader
{
public:
	void setColor(const Vector3f& c)
	{
		color = c;
	}

	Vector3f BaseVertexColor()
	{
		return color;
	}

private:
	Vector3f color;
};

#endif // !Shader
