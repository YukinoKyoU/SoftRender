
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

void SetTriangles()
{
	//°®ÐÄ
	Object o1;
	Triangle t1, t2, t3, t4, t5;
	t1.setVertex(Vector3f(0, -1, 0), Vector3f(3, 3, 0), Vector3f(-3, 3, 0));
	t1.setColor(Vector3f(225, 0, 0), Vector3f(255, 0, 100), Vector3f(255, 0, 100));
	o1.triangles.push_back(t1);
	t2.setVertex(Vector3f(0, 3, 0), Vector3f(3, 3, 0), Vector3f(1.5, 4, 0));
	t2.setColor(Vector3f(255, 0, 100), Vector3f(255, 0, 100), Vector3f(255, 0, 255));
	o1.triangles.push_back(t2);
	t3.setVertex(Vector3f(0, 3, 0), Vector3f(-1.5, 4, 0), Vector3f(-3, 3, 0));
	t3.setColor(Vector3f(255, 0, 100), Vector3f(255, 0, 255), Vector3f(255, 0, 100));
	o1.triangles.push_back(t3);
	t4.setVertex(Vector3f(0, -1, 0), Vector3f(2.5, 1.5, 0), Vector3f(3, 3, 0));
	t4.setColor(Vector3f(255, 0, 0), Vector3f(255, 0, 0), Vector3f(255, 0, 100));
	o1.triangles.push_back(t4);
	t5.setVertex(Vector3f(0, -1, 0), Vector3f(-3, 3, 0), Vector3f(-2.5, 1.5, 0));
	t5.setColor(Vector3f(255, 0, 0), Vector3f(255, 0, 100), Vector3f(255, 0, 0));
	o1.triangles.push_back(t5);

	o1.object_position = Vector3f(0, 0, 0);
	o1.object_rotation = Vector3f(0, 0, 0);
	o1.object_scale = Vector3f(1, 1, 1);
	objectList.push_back(o1);
}

void setObject()
{
	Object o1;
	Triangle t1;
	t1.setVertex(Vector3f(2, 0, -2), Vector3f(0, 2, -2), Vector3f(-2, 0, -2));
	t1.setColor(Vector3f(255, 0, 0), Vector3f(0, 225, 0), Vector3f(0, 0, 255));
	o1.triangles.push_back(t1);
	o1.object_position = Vector3f(0, 0, 0);
	o1.object_rotation = Vector3f(0, 0, 0);
	o1.object_scale = Vector3f(1, 1, 1);
	objectList.push_back(o1);
	
	//o2
	Object o2;
	Triangle t2;
	t2.setVertex(Vector3f(3.5, -1, -5), Vector3f(2.5, 1.5, -5), Vector3f(-1, 0.5, -5));
	t2.setColor(Vector3f(225, 0, 0), Vector3f(0, 255, 0), Vector3f(0, 0, 255));
	o2.triangles.push_back(t2);
	o2.object_position = Vector3f(0, 0, 0);
	o2.object_rotation = Vector3f(0, 0, 0);
	o2.object_scale = Vector3f(1, 1, 1);
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
			o->object_position = Vector3f(0, 0, 0);
			o->object_rotation = Vector3f(0, 135, 0);
			//angle = o->rotation.y();
			o->object_scale = Vector3f(1.5f, 1.5f, 1.5f);
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
	std::string bumpName = "hmap.jpg";
	//std::string normalName = "hmap.jpg";
	r.SetTexture(texPath + texName);
	r.SetBumpMap(texPath + bumpName);
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

	setModel("./models/spot/Model.obj");
	//setObject();
	//SetTriangles();
	Rasterizer r(width, height);
	//r.setMSAAState();
	SetTexture(r);

	while(1)
	{
		r.clearBuffer();
		std::vector<Object> olist = objectList;
		std::vector<Light> lList = lightList;

		r.vertexShader(olist, lList, camera);
		r.fragmentShader(olist, lList);
		

		cv::Mat image(height, width, CV_32FC3, r.getFrameBuffer().data());
		
		image.convertTo(image, CV_8UC3, 1.0f);
	
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		
		cv::imshow("image", image);

		cv::waitKey(0);

		std::cout << frameCount++ << std::endl;
	} 

	

	return 0;
}