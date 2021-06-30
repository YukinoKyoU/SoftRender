#ifndef RASTERIZER_H_
#define RASTERIZER_H_

#include <iostream>
#include <optional>

#include <Eigen/Eigen>
#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Global.h"


#include "Texture.h"


using namespace Eigen;

enum SSAAState
{
	close,
	open
};

class Rasterizer
{
public:

	Rasterizer(const int& w, const int& h);
	~Rasterizer();

	void SetTexture(Texture tex);
	void SetBumpMap(Texture bTex);
	void SetNormalMap(Texture nTex);

	//������ɫ�����������ת��Ϊ��Ļ�ռ���
	void vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightList, Camera& camera);

	//ƬԪ��ɫ��
	void fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightList);

	//�������ص���ɫ
	void setPixelColor(const Vector2i point, const Vector3f color);

	//��ȡ���ص�index
	int getPixelIndex(int x, int y);

	//��ȡ������ɫ��Ϣ�Ļ���
	std::vector<Vector3f>& getFrameBuffer();

	void clearBuffer();

	void setModelMatrix(const Object & object);

	void setViewMatrix(const Camera& c);
	void setProjectionMatrix(const Camera& c);

	//void setViewMatrix(const Matrix4f& v);
	//void setProjectionMatrix(const Matrix4f& p);

	void setSSAAState();

	void UpdateViewPlanes();
	bool ViewCull(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	
private:
	int height;						//��Ļ�߶�
	int width;						//��Ļ���

	std::vector<Vector4f>	viewPlanes;

	Matrix4f modelMatrix;			//model����
	Matrix4f viewMatrix;			//view����
	Matrix4f projectionMatrix;		//projection����
	
	Matrix4f mvpMatrix;				//mvp����

	Matrix4f viewportMatrix;		//�ӿڱ任����

	std::vector<Vector3f>	frameBuffer;		//��ɫ��Ϣ�Ļ���
	std::vector<Vector3f>   colorBuffer;
	std::vector<float>		depthBuffer;		//�����Ϣ����

	Shader shader;								//shader��Ϣ
	SSAAState ssaaState;						//MSAA״̬

	std::optional<Texture> texture;
	std::optional<Texture> bumpMap;
	std::optional<Texture> normalMap;
};

#endif // !RASTERIZER

