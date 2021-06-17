#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>
#include "Light.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>

using namespace Eigen;

//Shader�࣬�����������Ϣ���㷵�ص���ɫֵ
class Shader
{
public:
	Shader();
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
