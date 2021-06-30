#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>
#include "Light.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>

using namespace Eigen;

enum ShaderType
{
	BaseVertexColor,	// 顶点色
	NormalColor,		// 法线颜色
	BlinnPhongShading,	// Blinn-Phong着色

	TextureShading,		// 贴图着色
	TextureWithBump,	// 高度图着色
	TextureWithNormal	// 法线贴图着色
};
//Shader类，根据输入的信息计算返回的颜色值
class Shader
{
public:
	Shader();

	Vector3f setShaderType();
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
	
	void setTexCoord(const Vector2f& tC);
	void setTexture(Texture* t);
	void setBumpMap(Texture* bM);
	void setNormalMap(Texture* nM);

	Vector3f blinnPhongShader();
	Vector3f textureShader();
	Vector3f textureWithBump();
	Vector3f textureWithNormal();

private:
	Vector3f pointColor;
	Vector3f pointNormal;
	const std::vector<Light> *lightLists;
	Vector3f pointPosition;
	Vector2f pointTexCoord;
	Texture* pointTexture;
	Texture* pointBumpMap;
	Texture* pointNormalMap;

	ShaderType shaderType;
};

#endif // !Shader
