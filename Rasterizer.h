#ifndef RASTERIZER_H_
#define RASTERIZER_H_

#include <Eigen/Eigen>
#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "Global.h"

using namespace Eigen;

class Rasterizer
{
public:

	Rasterizer(const int& w, const int& h);
	~Rasterizer();

	//����model����
	void setModelMatrix(const Object& o);
	//����view����
	void setViewMatrix(const Camera& c);
	//����projection����
	void setProjectionMatrix(const Camera& c);

	//������ɫ�����������ת��Ϊ��Ļ�ռ���
	void vertexShader(std::vector<Object>& objectList, const Camera& c);

	//ƬԪ��ɫ��
	void fragmentShader(std::vector<Object>& objectList);

	//�������ص���ɫ
	void setPixelColor(const Vector2i point, const Vector3f color);

	//��ȡ���ص�index
	int getPixelIndex(int x, int y);

	//��ȡ������ɫ��Ϣ�Ļ���
	std::vector<Vector3f>& getFrameBuffer();

	void clearBuffer();

private:
	int height;						//��Ļ�߶�
	int width;						//��Ļ���

	Matrix4f modelMatrix;			//model����
	Matrix4f viewMatrix;			//view����
	Matrix4f projectionMatrix;		//projection����

	Matrix4f mvpMatrix;				//mvp����

	Matrix4f viewPortMatrix;		//�ӿڱ任����

	std::vector<Vector3f>	frameBuffer;		//��ɫ��Ϣ�Ļ���
	std::vector<float>		depthBuffer;		//�����Ϣ����

	Shader shader;								//shader��Ϣ
};




#endif // !RASTERIZER

