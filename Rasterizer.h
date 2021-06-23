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

	//void setViewMatrix(const Camera& c);
	//void setProjectionMatrix(const Camera& c);

	void setViewMatrix(const Matrix4f& v);
	void setProjectionMatrix(const Matrix4f& p);

	void setSSAAState();

private:
	int height;						//��Ļ�߶�
	int width;						//��Ļ���

	Matrix4f model_matrix;			//model����
	Matrix4f view_matrix;			//view����
	Matrix4f projection_matrix;		//projection����

	Matrix4f mvp_matrix;				//mvp����

	Matrix4f viewport_matrix;		//�ӿڱ任����

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

