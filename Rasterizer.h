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

	void setModelMatrix(const Object& o);
	void setViewMatrix(const Camera& c);
	void setProjectionMatrix(const Camera& c);

	void vertexShader(std::vector<Object>& objectList, const Camera& c);

	void fragmentShader(std::vector<Object>& objectList);

	void setPixelColor(const Vector2i point, const Vector3f color);

	int getPixelIndex(int x, int y);

	std::vector<Vector3f>& getFrameBuffer();

	void clearBuffer();

private:
	int height;
	int width;

	Matrix4f modelMatrix;
	Matrix4f viewMatrix;
	Matrix4f projectionMatrix;

	Matrix4f mvpMatrix;

	Matrix4f viewPortMatrix;

	std::vector<Vector3f>	frameBuffer;
	std::vector<float>		depthBuffer;

	Shader shader;
};




#endif // !RASTERIZER

