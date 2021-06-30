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
	BaseVertexColor,	// ����ɫ
	NormalColor,		// ������ɫ
	BlinnPhongShading,	// Blinn-Phong��ɫ

	TextureShading,		// ��ͼ��ɫ
	TextureWithBump,	// �߶�ͼ��ɫ
	TextureWithNormal	// ������ͼ��ɫ
};
//Shader�࣬�����������Ϣ���㷵�ص���ɫֵ
class Shader
{
public:
	Shader();

	Vector3f setShaderType();
	//����shader����ɫ��Ϣ
	void setColor(const Vector3f& _pointColor);
	//���ؼ������ɫ
	Vector3f baseVertexColor();
	//����shader�ķ��ߣ�4άת3ά��
	void setNormal(const Vector4f& _pointNormal);
	//���ؼ���ķ���
	Vector3f normalShader();
	//���ù�Դ
	void setLight(const std::vector<Light>& _lightLists);
	//��������
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
