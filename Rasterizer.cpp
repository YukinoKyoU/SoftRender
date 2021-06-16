#include "Rasterizer.h"

Rasterizer::Rasterizer(const int& w, const int& h) : width(w), height(h)
{
	//设置缓冲大小
	frameBuffer.resize(width * height);
	depthBuffer.resize(width * height);

	//设置视口变换矩阵
	viewPortMatrix << width / 2, 0, 0, width / 2,
					0, height / 2, 0, height / 2,
					0, 0, 1, 0,
					0, 0, 0, 1;
}

Rasterizer::~Rasterizer()
{
	clearBuffer();
}

void Rasterizer::setModelMatrix(const Object& o)
{
	Matrix4f rX, rY, rZ;		//XYZ轴的旋转矩阵
	float radX, radY, radZ;		//xyz轴的旋转弧度
	Matrix4f scale;				//缩放矩阵
	Matrix4f trans;				//位移矩阵

	radX = toRadian(o.rotation.x());
	radY = toRadian(o.rotation.y());
	radZ = toRadian(o.rotation.z());

	rX << 1, 0, 0, 0,
		  0, cos(radX), -sin(radX), 0,
		  0, sin(radX), cos(radX), 0,
		  0, 0, 0, 1;
	rY << cos(radY), 0, sin(radY), 0,
		  0, 1, 0, 0,
		  -sin(radY), 0, cos(radY), 0,
		  0, 0, 0, 1;
	rZ << cos(radZ), -sin(radZ), 0, 0,
		  sin(radZ), cos(radZ), 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1;

	scale << o.scale.x(), 0, 0, 0,
		     0, o.scale.y(), 0, 0,
			 0, 0, o.scale.z(), 0,
			 0, 0, 0, 1;

	trans << 1, 0, 0, -o.position.x(),
			 0, 1, 0, -o.position.y(),
			 0, 0, 1, -o.position.z(),
			 0, 0, 0, 1;
	//矩阵左乘计算出模型矩阵
	modelMatrix =  scale * rZ * rY * rX * trans;
}

void Rasterizer::setViewMatrix(const Camera& c)
{
	//将摄像机移动到原点，然后使用旋转矩阵的正交性让摄像机摆正
	Matrix4f t;		//移动矩阵
	Vector3f cX;	//摄像机的x轴
	Matrix4f r;		//旋转矩阵的旋转矩阵

	t << 1, 0, 0, -c.position.x(),
		 0, 1, 0, -c.position.y(),
		 0, 0, 1, -c.position.z(),
		 0, 0, 0, 1;

	//计算x轴方向
	cX = c.lookAt.cross(c.up);

	r << cX.x(),   cX.y(),   cX.z(), 0,
		 c.up.x(), c.up.y(), c.up.z(), 0,
		 c.lookAt.x(), c.lookAt.y(), c.lookAt.z(), 0,
		 0, 0, 0, 1;
	//矩阵左乘计算出视图矩阵
	viewMatrix = r * t;
}

void Rasterizer::setProjectionMatrix(const Camera& c)
{
	//透视投影矩阵
	Matrix4f projection;							//将透视视锥转化到剪裁空间中
	Matrix4f orthoTrans, orthoScale, ortho;			//剪裁空间转化为CVV

	float t, r;			//近平面的上边界和右边界
	float radFov;		//视野的弧度制

	radFov = toRadian(c.fov);
	t = tan(radFov / 2) * c.nNear;
	r = c.aspectRatio * t;

	float n = -c.nNear;
	float f = -c.nFar;

	projection << n, 0, 0, 0,
		          0, n, 0, 0,
		          0, 0, n + f, -n * f,
		          0, 0, -1, 0;

	orthoTrans << 1, 0, 0, 0,
		          0, 1, 0, 0,
				  0, 0, 1, -(n + f) / 2,
				  0, 0, 0, 1;

	orthoScale << 1 / r, 0, 0, 0,
				  0, 1 / t, 0, 0,
				  0, 0, 2 / (n - f), 0,
				  0, 0, 0, 1;

	ortho = orthoScale * orthoTrans;

	//矩阵左乘计算出透视投影矩阵
	projectionMatrix = ortho * projection;

}

void Rasterizer::vertexShader(std::vector<Object>& objectList, const Camera& c)
{
	//读取物体列表中的所有物体
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		setViewMatrix(c);
		setProjectionMatrix(c);

		//设置MVP函数
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		//读取每个物体中的所有三角形图元
		for (Triangle& tri : object.triangles)
		{
			//读取每个三角形图元的顶点
			for (auto& vec : tri.vertex)
			{
				//求出CVV空间下坐标
				vec = mvpMatrix * vec;

				//透视除法，将w值归一，获得齐次坐标
				vec.x() /= vec.w();
				vec.y() /= vec.w();
				vec.z() /= vec.w();
				vec.w() /= vec.w();

				//将坐标点投影到屏幕
				vec = viewPortMatrix * vec;

				
			}
		}
	}
}

//判断像素点是否在三角形内
static bool isInsideTriangle(const float x, const float y, const Triangle& t)
{
	//储存像素点（齐次）坐标
	Vector3f temPixel(x, y, 1.0f);
	//储存三角形三个顶点的坐标（将z值归一）
	std::vector<Vector3f> vertexArray(3);
	vertexArray[0] = { t.vertex[0].x(), t.vertex[0].y(), 1.0f};
	vertexArray[1] = { t.vertex[1].x(), t.vertex[1].y(), 1.0f};
	vertexArray[2] = { t.vertex[2].x(), t.vertex[2].y(), 1.0f};

	//储存三条边的向量。逆时针方向
	Vector3f edge1, edge2, edge3;
	edge1 = vertexArray[1] - vertexArray[0];
	edge2 = vertexArray[2] - vertexArray[1];
	edge3 = vertexArray[0] - vertexArray[2];
	
	//像素点到各个顶点的向量
	Vector3f p2Ver1, p2Ver2, p2Ver3;
	p2Ver1 = temPixel - vertexArray[0];
	p2Ver2 = temPixel - vertexArray[1];
	p2Ver3 = temPixel - vertexArray[2];

	//如果在内部，则与三条边向量的叉乘全部同号
	if (p2Ver1.cross(edge1).z() > 0 && p2Ver2.cross(edge2).z() > 0 && p2Ver3.cross(edge3).z() > 0
		|| p2Ver1.cross(edge1).z() < 0 && p2Ver2.cross(edge2).z() < 0 && p2Ver3.cross(edge3).z() < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//求一个点的重心坐标
static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

//重心坐标插值（坐标点，颜色等）
static Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Vector3f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}
//重心坐标插值（深度值）
static float interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

//片元着色器
void Rasterizer::fragmentShader(std::vector<Object>& objectList)
{
	//遍历物体列表中的所有物体
	for (Object& object : objectList)
	{
		//遍历每个物体中的三角形图元
		for (Triangle& t : object.triangles)
		{
			//计算包围盒
			float minXf, maxXf, minYf, maxYf;
			minXf = width;
			maxXf = 0;
			minYf = height;
			maxYf = 0;
			//遍历三角形顶点
			for (const auto& ver : t.vertex)
			{
				if (ver.x() < minXf) minXf = ver.x();
				if (ver.x() > maxXf) maxXf = ver.x();
				if (ver.y() < minYf) minYf = ver.y();
				if (ver.y() > maxYf) maxYf = ver.y();
			}
			if (minXf < 0) minXf = 0;
			if (maxXf > width) maxXf = width;
			if (minYf < 0) minYf = 0;
			if (maxYf > height) maxYf = height;
			//取整
			int minX, maxX, minY, maxY;
			minX = floor(minXf);
			maxX = ceil(maxXf);
			minY = floor(minYf);
			maxY = ceil(maxYf);

			//对包围盒中的每一个像素
			for (int y = minY; y < maxY; ++y)
			{
				for (int x = minX; x < maxX; ++x)
				{
					//判断像素中心是否在三角形内(像素点在正方形中间)
					if (isInsideTriangle((float)x + 0.5, (float)y + 0.5, t))
					{
						//计算重心坐标
						float alpha, beta, gamma;
						std::tie(alpha, beta, gamma) = computeBarycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);
						//透视矫正插值
						float Z = 1.0 / (alpha / t.vertex[0].w() + beta / t.vertex[1].w() + gamma / t.vertex[2].w());
						//插值计算各个像素点深度
						float interpolate_z = interpolate(alpha, beta, gamma, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());
						//透视矫正后计算正确的深度值
						interpolate_z /= Z;
						//对深度值做反转，保证深度值越小，离相机越远
						interpolate_z *= -1;

						//判断深度值（小的记录）
						if (interpolate_z < depthBuffer[getPixelIndex(x, y)] )
						{
							//深度更近的话插值出颜色，然后更新深度信息
							Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2]);
							//将颜色值记录到shader中
							shader.setColor (interpolateColor);
							//将颜色值赋给pixel
							Vector3f pixelColor = shader.BaseVertexColor();
							setPixelColor(Vector2i(x, y), pixelColor);
							//将深度值赋给buffer
							depthBuffer[getPixelIndex(x, y)] = interpolate_z;
						}
					}
				}
			}
		}
	}
}

void Rasterizer::setPixelColor(const Vector2i point, const Vector3f color)
{
	int ind = (height - point.y() - 1) * width + point.x();
	frameBuffer[ind] = color;
}

int Rasterizer::getPixelIndex(int x, int y)
{
	return (height - y - 1) * width + x;
}

std::vector<Vector3f>& Rasterizer::getFrameBuffer()
{
	return frameBuffer;
}

void Rasterizer::clearBuffer()
{
	std::fill(frameBuffer.begin(), frameBuffer.end(), Vector3f(0, 0, 0));
	std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
}