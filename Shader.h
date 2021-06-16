#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>

using namespace Eigen;

//Shader类，根据输入的信息计算返回的颜色值
class Shader
{
public:
	//设置shader的颜色信息
	void setColor(const Vector3f& c);
	//返回计算的颜色
	Vector3f baseVertexColor();
	//设置shader的法线（4维转3维）
	void setNormal(const Vector4f& n);
	//返回计算的法线
	Vector3f normalShader();

private:
	Vector3f color;
	Vector3f normal;
};

#endif // !Shader
