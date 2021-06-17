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

enum MSAAState
{
	close,
	open
};

class Rasterizer
{
public:

	Rasterizer(const int& w, const int& h);
	~Rasterizer();

	//设置model矩阵
	void setModelMatrix(const Object& o);
	//设置view矩阵
	void setViewMatrix(const Camera& c);
	//设置projection矩阵
	void setProjectionMatrix(const Camera& c);

	void SetTexture(Texture tex);
	void SetBumpMap(Texture bTex);
	void SetNormalMap(Texture nTex);

	//顶点着色器，将坐标点转换为屏幕空间中
	void vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightLists, Camera& camera);

	//片元着色器
	void fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightLists);

	//设置像素点颜色
	void setPixelColor(const Vector2i point, const Vector3f color);

	//获取像素点index
	int getPixelIndex(int x, int y);

	//获取储存颜色信息的缓冲
	std::vector<Vector3f>& getFrameBuffer();

	void clearBuffer();

	void setMSAAState();

private:
	int height;						//屏幕高度
	int width;						//屏幕宽度

	Matrix4f modelMatrix;			//model矩阵
	Matrix4f viewMatrix;			//view矩阵
	Matrix4f projectionMatrix;		//projection矩阵

	Matrix4f mvpMatrix;				//mvp矩阵

	Matrix4f viewPortMatrix;		//视口变换矩阵

	std::vector<Vector3f>	frameBuffer;		//颜色信息的缓冲
	std::vector<Vector3f>   colorBuffer;
	std::vector<float>		depthBuffer;		//深度信息缓冲

	Shader shader;								//shader信息
	MSAAState msaaState;						//MSAA状态


	std::optional<Texture> texture;
	std::optional<Texture> bumpMap;
	std::optional<Texture> normalMap;
};




#endif // !RASTERIZER

