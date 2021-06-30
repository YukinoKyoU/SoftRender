#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <opencv2/opencv.hpp>
#include "Eigen/Eigen"
#include <iostream>

using namespace Eigen;

enum double_lerp
{
	Close,
	Open
};
class Texture
{
public:
	Texture(std::string name);

	Vector3f GetColor(float u, float v);

	int GetWidth() { return width; }
	int GetHeight() { return height; }

private:
	cv::Mat texture;
	int width, height;
	double_lerp doubleLerpState;
};

#endif // !TEXTURE_H_
