
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

#include "Rasterizer.h"
#include "OBJ_Loader.h"
#include "Texture.h"
#include "Global.h"


constexpr int screenWidth = 600;
constexpr int screenHeight = 600;
int key = 0;
float objectAngleValue = 135;
float objectScaleValue = 1.5f;

Camera camera(Vector3f(0, 0, 10), Vector3f(0, 0, -1), Vector3f(0, 1, 0),
	45.f, 0.1f, 50.f, screenWidth / screenHeight);

//键盘输入
void processInput();
//鼠标输入
void OnMouse(int event, int x, int y, int flags, void* ustc);

std::vector<Object> objectList;
std::vector<Light> lightList;

void setObject()
{
	Object o1;
	Triangle t1;
	t1.setVertex(Vector3f(2, 0, -2), Vector3f(0, 2, -2), Vector3f(-2, 0, -2));
	t1.setColor(Vector3f(255, 0, 0), Vector3f(0, 225, 0), Vector3f(0, 0, 255));
	o1.trianglesInObject.push_back(t1);
	o1.objectPosition = Vector3f(0, 0, 0);
	o1.objectRotation = Vector3f(0, 0, 0);
	o1.objectScale = Vector3f(1, 1, 1);
	objectList.push_back(o1);
	
	//o2
	Object o2;
	Triangle t2;
	t2.setVertex(Vector3f(3.5, -1, -5), Vector3f(2.5, 1.5, -5), Vector3f(-1, 0.5, -5));
	t2.setColor(Vector3f(225, 0, 0), Vector3f(0, 255, 0), Vector3f(0, 0, 255));
	o2.trianglesInObject.push_back(t2);
	o2.objectPosition = Vector3f(0, 0, 0);
	o2.objectRotation = Vector3f(0, 0, 0);
	o2.objectScale = Vector3f(1, 1, 1);
	objectList.push_back(o2);	
}


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
				o->trianglesInObject.push_back(*t);
			}
			o->objectPosition = Vector3f(0, 0, 0);
			o->objectRotation = Vector3f(0, 0, 0);
			//angle = o->rotation.y();
			o->objectScale = Vector3f(1.f, 1.f, 1.f);
			//scale = 2;
			objectList.push_back(*o);
		}
	}
	else 
		std::cout << "load failed" << std::endl;
}

void SetTexture(Rasterizer& r)
{
	std::string texPath = "./models/spot/";
	std::string texName = "spot_texture.png";
	//std::string bumpName = "hmap.jpg";
	//std::string normalName = "normal.jpg";
	r.SetTexture(texPath + texName);
	//r.SetBumpMap(texPath + bumpName);
	//r.SetNormalMap(texPath + normalName);
}

int main()
{

	Light light1({20, 20, 20, 1}, {700, 700, 700}, {10, 10, 10});
	Light light2({-20, 20, 0, 1}, {500, 500, 500}, {10, 10, 10});
	lightList.push_back(light1);
	lightList.push_back(light2);

	setModel("./models/spot/Model.obj");
	//setModel("./Models/Neptune.obj");
	//setObject();
	Rasterizer r(screenWidth, screenHeight);
	//r.setSSAAState();
	SetTexture(r);

	do
	{
		objectList[0].objectRotation = Vector3f(0, objectAngleValue, 0);
		objectList[0].objectScale = Vector3f(objectScaleValue, objectScaleValue, objectScaleValue);

		r.clearBuffer();
		std::vector<Object> copyObjectList = objectList;
		std::vector<Light> copyLightList = lightList;

		r.vertexShader(copyObjectList, copyLightList, camera);

		r.fragmentShader(copyObjectList, copyLightList);
		

		cv::Mat image(screenHeight, screenWidth, CV_32FC3, r.getFrameBuffer().data());
		
		image.convertTo(image, CV_8UC3, 1.0f);
	
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		
		cv::imshow("image", image);
		
		//等待键盘输入
		key = cv::waitKey(0);
		//等待鼠标输入
		cv::setMouseCallback("image", OnMouse);

		processInput();
		system("cls");

	} while (key != 27);

	return 0;
}

void OnMouse(int event, int x, int y, int flags, void* ustc)
{
	float deltaAngle = 0.001f;
	static cv::Point2f p{ -1, -1 };

	if (event == cv::EVENT_LBUTTONDOWN)
	{
		p = cvPoint(x, y);
		std::cout << p.x << " " << p.y << std::endl;

	}

	if (event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON))
	{
		int dx = x - p.x;
		int dy = y - p.y;

		if (x >= 0 && x <= screenWidth - 1 && y >= 0 && y <= screenHeight - 1)
		{
			if (dx > 0)
			{
				camera.RotateYaw(deltaAngle * dx);
			}
			if (dx < 0)
			{
				camera.RotateYaw(deltaAngle * dx);
			}
			if (dy > 0)
			{
				camera.RotatePitch(-deltaAngle * dy);
			}
			else if (dy < 0)
			{
				camera.RotatePitch(-deltaAngle * dy);
			}
		}
	}

}

void processInput()
{
	switch (key)
	{
	case 'q':
		objectAngleValue += -10;
		break;
	case 'e':
		objectAngleValue += 10;
		break;
	case 'z':
		objectScaleValue += 0.2;
		break;
	case 'c':
		objectScaleValue += -0.2;
		break;
	case 'w':
		camera.CameraTrans(Vector3f(0, 0, 0.5));
		break;
	case 's':
		camera.CameraTrans(Vector3f(0, 0.0, -0.5));
		break;
	case 'a':
		camera.CameraTrans(Vector3f(0.5f, 0, 0));
		break;
	case 'd':
		camera.CameraTrans(Vector3f(-0.5f, 0, 0));
		break;

	}
	if (objectScaleValue > 5) objectScaleValue = 5;
	if (objectScaleValue < 0.3) objectScaleValue = 0.3;
}
