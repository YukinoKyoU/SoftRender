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

	cv::Vec3b color;
	if (double_lerp_state == double_lerp :: Open)
	{
		float u_min = std::floor(uT);
		float u_max = std::min((float)(width-1), std::ceil(uT));
		float v_min = std::floor(vT);
		float v_max = std::min((float)(height-1), std::ceil(vT));


		auto Q11 = texture.at<cv::Vec3b>(v_max, u_min);
		auto Q12 = texture.at<cv::Vec3b>(v_max, u_max);

		auto Q21 = texture.at<cv::Vec3b>(v_min, u_min);
		auto Q22 = texture.at<cv::Vec3b>(v_min, u_max);

		float rs = (uT - u_min) / (u_max - u_min);
		float rt = (vT - v_max) / (v_min - v_max);
		auto cBot = (1 - rs) * Q11 + rs * Q12;
		auto cTop = (1 - rs) * Q21 + rs * Q22;

		color = (1 - rt) * cBot + rt * cTop;
		
	}
	else
	{
		color = texture.at<cv::Vec3b>(vT, uT);
	}
	return Vector3f(color[0], color[1], color[2]);
}
