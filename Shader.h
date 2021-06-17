#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>
#include "Light.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>

using namespace Eigen;

//Shader类，根据输入的信息计算返回的颜色值
class Shader
{
public:
	Shader();
	//设置shader的颜色信息
	void setColor(const Vector3f& _pointColor);
	//返回计算的颜色
	Vector3f baseVertexColor();
	//设置shader的法线（4维转3维）
	void setNormal(const Vector4f& _pointNormal);
	//返回计算的法线
	Vector3f normalShader();
	//设置光源
	void setLight(const std::vector<Light>& _lightLists);
	//计算坐标
	void setPosition(const Vector4f& _pointPosition);
	
	void setTexCoord(const Vector2f& tC)
	{
		pointTexCoord = tC;
	}
	void SetTexture(Texture* t)
	{
		pointTexture = t;
	}
	void SetBumpMap(Texture* bM)
	{
		pointBumpMap = bM;
	}
	void SetNormalMap(Texture* nM)
	{
		pointNormalMap = nM;
	}

	Vector3f BlinnPhongShader();
	Vector3f TextureShader();
	Vector3f TextureWithBump();
	Vector3f TextureWithNormal();

private:
	Vector3f pointColor;
	Vector3f pointNormal;
	const std::vector<Light> *lightLists;
	Vector3f pointPosition;
	Vector2f pointTexCoord;
	Texture* pointTexture;
	Texture* pointBumpMap;
	Texture* pointNormalMap;
};

#endif // !Shader
