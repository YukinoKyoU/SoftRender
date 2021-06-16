#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>

using namespace Eigen;

//Shader类，根据输入的信息计算返回的颜色值
class Shader
{
public:
	//输入的颜色信息
	void setColor(const Vector3f& c);
	//返回计算的颜色
	Vector3f BaseVertexColor()
	{
		return color;
	}

private:
	Vector3f color;
};

#endif // !Shader
