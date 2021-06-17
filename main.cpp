
#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Global.h"
#include "OBJ_Loader.h"
#include "Texture.h"


constexpr int width = 700;
constexpr int height = 700;

std::vector<Object> objectList;
std::vector<Light> lightList;

int frameCount = 0;

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
			o->rotation = Vector3f(0, 135, 0);
			//angle = o->rotation.y();
			o->scale = Vector3f(1.5, 1.5, 1);
			//scale = 2;
			objectList.push_back(*o);
		}
	}
	else 
		std::cout << "load failed" << std::endl;
}

void SetTexture(Rasterizer& r)
{
	std::string texPath = "./";
	std::string texName = "Texture.png";
	// std::string bumpName = "BumpTest01.png";
	//std::string normalName = "Normal.png";
	r.SetTexture(texPath + texName);
	// r.SetBumpMap(texPath + bumpName);
	//r.SetNormalMap(texPath + normalName);
}

int main()
{
	Camera camera(Vector3f(0, 0, 10), Vector3f(0, 0, -1).normalized(), Vector3f(0, 1, 0).normalized(),
		45.f, 0.1f, 50.f, width / height);

	Light light1({20, 20, 20, 1}, {700, 700, 700}, {10, 10, 10});
	Light light2({-20, 20, 0, 1}, {500, 500, 500}, {10, 10, 10});
	lightList.push_back(light1);
	lightList.push_back(light2);

	setModel("Model.obj");
	//setObject();
	Rasterizer r(width, height);
	//r.setMSAAState();
	SetTexture(r);

	while(1)
	{
		r.clearBuffer();
		std::vector<Object> list = objectList;

		r.vertexShader(list, lightList, camera);
		r.fragmentShader(list, lightList);
		

		cv::Mat image(height, width, CV_32FC3, r.getFrameBuffer().data());
		
		image.convertTo(image, CV_8UC3, 1.0f);
	
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		
		cv::imshow("image", image);

		cv::waitKey(0);

		std::cout << frameCount++ << std::endl;
	} 

	

	return 0;
}