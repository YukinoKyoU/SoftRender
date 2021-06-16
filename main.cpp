
#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Global.h"


constexpr int width = 700;
constexpr int height = 700;

std::vector<Object> objectList;
Camera camera;

int frameCount = 0;

void setObject()
{
	Object o1;
	Triangle t1;
	t1.setVertex(Vector3f(0, 2, -2), Vector3f(-2, 0, -2), Vector3f(1, 0, -2));
	t1.setColor(Vector3f(225, 0, 0), Vector3f(0, 225, 0), Vector3f(0, 0, 225));
	o1.triangles.push_back(t1);
	o1.position = Vector3f(0, 0, 0);
	o1.rotation = Vector3f(0, 0, 0);
	o1.scale = Vector3f(2, 2, 1);
	objectList.push_back(o1);

	//o2
	Object o2;
	Triangle t2;
	t2.setVertex(Vector3f(2, 1, 0), Vector3f(-2, 4, -5), Vector3f(-1, -3, -5));
	t2.setColor(Vector3f(100, 100, 100), Vector3f(100, 100, 100), Vector3f(100, 100, 100));
	o2.triangles.push_back(t2);
	o2.position = Vector3f(0, 0, 0);
	o2.rotation = Vector3f(0, 0, 0);
	o2.scale = Vector3f(1, 1, 1);
	objectList.push_back(o2);
}

void setCamera()
{
	camera.position = Vector3f(0, 0, 10);
	camera.lookAt = Vector3f(0, 0, -1).normalized();
	camera.up = Vector3f(0, 1, 0).normalized();
	camera.fov = 45.f;
	camera.nNear = 0.1f;
	camera.nFar = 50.f;
	camera.aspectRatio = width / height;
}

int main()
{
	setObject();
	setCamera();

	Rasterizer r(width, height);

	do
	{
		r.clearBuffer();
		std::vector<Object> list = objectList;

		r.vertexShader(list, camera);
		r.fragmentShader(list);

		cv::Mat image(height, width, CV_32FC3, r.getFrameBuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);
		cv::waitKey(0);
		std::cout << frameCount++ << std::endl;
	} while (0);

	return 0;
}