#include "Texture.h"

Texture::Texture(std::string name)
{
	texture = cv::imread(name);
	cv::cvtColor(texture, texture, cv::COLOR_RGB2BGR);
	width = texture.cols;
	height = texture.rows;
	if (width <= 0 || height <= 0)
		std::cout << "load fail" << std::endl;
}

Vector3f Texture::GetColor(float u, float v)
{
	u = u - floor(u);
	v = v - floor(v);
	//因为从0开始，所以长宽需要－1
	float uT = u * (width - 1);
	float vT = (1 - v) * (height - 1);

	auto color = texture.at<cv::Vec3b>(vT, uT);
	return Vector3f(color[0], color[1], color[2]);
}
