
#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Global.h"
#include "OBJ_Loader.h"


constexpr int width = 700;
constexpr int height = 700;

std::vector<Object> objectList;
Camera camera;

int frameCount = 0;
/*
void ShowFPS(cv::Mat& dstImage) {
	char str[20];	//存放字符串化的帧率
	double fps;		//帧率
	double t_front, t_now;					//用于中间计算

	t_front = (double)cv::getTickCount();		//返回从操作系统启动到当前所经过的毫秒数
	t_now = ((double)cv::getTickCount() - t_front) / cv::getTickFrequency();	//getTickFrequency返回每秒的计时周期数
	fps = 1.0 / t_now;

	std::string FPSstring("FPS:");
	sprintf_s(str, "%.2f", fps);
	FPSstring += str;
	//在帧上显示"FPS:XXXX"
	putText(dstImage, FPSstring, cv::Point(5, 20),
		cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255));
}
*/
/*
void setObject()
{
	Object o1;
	Triangle t1;
	t1.setVertex(Vector3f(2, 0, -2), Vector3f(0, 2, -2), Vector3f(-2, 0, -2));
	t1.setColor(Vector3f(255, 0, 0), Vector3f(0, 225, 0), Vector3f(0, 0, 255));
	o1.triangles.push_back(t1);
	o1.position = Vector3f(0, 0, 0);
	o1.rotation = Vector3f(0, 0, 0);
	o1.scale = Vector3f(1, 1, 1);
	objectList.push_back(o1);
	
	//o2
	Object o2;
	Triangle t2;
	t2.setVertex(Vector3f(3.5, -1, -5), Vector3f(2.5, 1.5, -5), Vector3f(-1, 0.5, -5));
	t2.setColor(Vector3f(225, 0, 0), Vector3f(0, 255, 0), Vector3f(0, 0, 255));
	o2.triangles.push_back(t2);
	o2.position = Vector3f(0, 0, 0);
	o2.rotation = Vector3f(0, 0, 0);
	o2.scale = Vector3f(1, 1, 1);
	objectList.push_back(o2);
	
}
*/

void setModel(const std::string& objName)
{
	objl::Loader loader;
	bool bLoad = loader.LoadFile(objName);

	if (bLoad)
	{
		for (const auto& mesh : loader.LoadedMeshes)
		{
			Object* o = new Object();
			for (int i = 0; i < mesh.Vertices.size(); i += 3)
			{
				Triangle* t = new Triangle();
				for (int j = 0; j < 3; j++)
				{
					t->setVertex(j, Vector3f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z));
					t->setColor(j, Vector3f(255, 255, 255));
					t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z));
					t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
				}
				o->triangles.push_back(*t);
			}
			o->position = Vector3f(0, 0, 0);
			o->rotation = Vector3f(0, 0, 0);
			//angle = o->rotation.y();
			o->scale = Vector3f(1, 1, 1);
			//scale = 2;
			objectList.push_back(*o);
		}
	}
	else 
		std::cout << "load failed" << std::endl;
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
	setModel("bunnyTest.obj");
	//setObject();
	setCamera();
	
	Rasterizer r(width, height);
	//r.setMSAAState();

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