#include "Rasterizer.h"

Rasterizer::Rasterizer(const int& w, const int& h) : width(w), height(h)
{
	frameBuffer.resize(width * height);
	depthBuffer.resize(width * height);

	viewPortMatrix << width / 2, 0, 0, width / 2,
					0, height / 2, 0, height / 2,
					0, 0, 1, 0,
					0, 0, 0, 1;
}

Rasterizer::~Rasterizer()
{
}

void Rasterizer::setModelMatrix(const Object& o)
{
	Matrix4f rX, rY, rZ; //XYZ轴的旋转矩阵
	float radX, radY, radZ; //xyz轴的旋转弧度
	Matrix4f scale; //缩放矩阵
	Matrix4f move; //位移矩阵

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

	move << 1, 0, 0, o.position.x(),
			0, 1, 0, o.position.y(),
			0, 0, 1, o.position.z(),
			0, 0, 0, 1;
		//矩阵左乘计算出模型矩阵
	modelMatrix = move * scale * rZ * rY * rX;
}

void Rasterizer::setViewMatrix(const Camera& c)
{
	//将摄像机移动到原点，然后使用旋转矩阵的正交性让摄像机摆正
	Matrix4f t; //移动矩阵
	Vector3f cX; //摄像机的x轴
	Matrix4f r; //旋转矩阵的旋转矩阵

	t << 1, 0, 0, -c.position.x(),
		0, 1, 0, -c.position.y(),
		0, 0, 1, -c.position.z(),
		0, 0, 0, 1;

	cX = c.lookAt.cross(c.up);

	r << cX.x(), cX.y(), cX.z(), 0,
		 c.up.x(), c.up.y(), c.up.z(), 0,
		-c.lookAt.x(), -c.lookAt.y(), -c.lookAt.z(), 0,
		0, 0, 0, 1;
	//矩阵左乘计算出视图矩阵
	viewMatrix = r * t;
}

void Rasterizer::setProjectionMatrix(const Camera& c)
{
	//透视投影矩阵
	Matrix4f p2o; //将梯台状透视视锥挤成长方体正交投影
	Matrix4f orthoTrans, orthoScale, ortho; //正交投影矩阵的平移和缩放分解
	float t, r; //近平面的上边界和右边界
	float radFov; //视野的弧度制

	radFov = toRadian(c.fov);
	t = tan(radFov / 2) * c.nNear;
	r = c.aspectRatio * t;

	p2o << c.nNear, 0, 0, 0,
		0, c.nNear, 0, 0,
		0, 0, c.nFar + c.nNear, c.nNear* c.nFar,
		0, 0, -1, 0;
	orthoTrans << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, (c.nNear + c.nFar) / 2,
		0, 0, 0, 1;
	orthoScale << 1 / r, 0, 0, 0,
		0, 1 / t, 0, 0,
		0, 0, 2 / (c.nFar - c.nNear), 0,
		0, 0, 0, 1;
	ortho = orthoScale * orthoTrans;
	//矩阵左乘计算出透视投影矩阵
	projectionMatrix = ortho * p2o;

}

void Rasterizer::vertexShader(std::vector<Object>& objectList, const Camera& c)
{
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		setViewMatrix(c);
		setProjectionMatrix(c);

		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		for (Triangle& tri : object.triangles)
		{
			for (auto& vec : tri.vertex)
			{
				vec = mvpMatrix * vec;

				vec = viewPortMatrix * vec;

				vec.x() /= vec.w();
				vec.y() /= vec.w();
				vec.z() /= vec.w();
				vec.w() /= vec.w();
			}
		}
	}
}

static bool insideTriangle(const float x, const float y, const Triangle& t)
{
	Vector3f temPixel(x, y, 1.0f);

	std::vector<Vector3f> vertexArray(3);
	vertexArray[0] = { t.vertex[0].x(), t.vertex[0].y(), 1.0f};
	vertexArray[1] = { t.vertex[1].x(), t.vertex[1].y(), 1.0f};
	vertexArray[2] = { t.vertex[2].x(), t.vertex[2].y(), 1.0f};

	Vector3f edge1, edge2, edge3;
	edge1 = vertexArray[1] - vertexArray[0];
	edge2 = vertexArray[2] - vertexArray[1];
	edge3 = vertexArray[0] - vertexArray[2];
	
	Vector3f p2Ver1, p2Ver2, p2Ver3;
	p2Ver1 = temPixel - vertexArray[0];
	p2Ver2 = temPixel - vertexArray[1];
	p2Ver3 = temPixel - vertexArray[2];

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

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

static Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Vector3f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

static float interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

void Rasterizer::fragmentShader(std::vector<Object>& objectList)
{
	for (Object& object : objectList)
	{
		for (Triangle& t : object.triangles)
		{
			float minXf, maxXf, minYf, maxYf;
			minXf = width;
			maxXf = 0;
			minYf = height;
			maxYf = 0;
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
			//对bounding box中的每一个像素
			for (int y = minY; y < maxY; ++y)
			{
				for (int x = minX; x < maxX; ++x)
				{
					//判断像素中心是否在三角形内
					if (insideTriangle((float)x + 0.5, (float)y + 0.5, t))
					{
						//计算2D重心坐标
						float alpha2D, beta2D, gamma2D;
						std::tie(alpha2D, beta2D, gamma2D) = computeBarycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);
						float w = alpha2D + beta2D + gamma2D;
						float interpolateZ2D = interpolate(alpha2D, beta2D, gamma2D, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());
						interpolateZ2D /= w;

						//判断深度值
						if (depthBuffer[getPixelIndex(x, y)] > interpolateZ2D)
						{
							//深度更近的话插值出颜色，然后更新深度信息
							Vector3f interpolateColor = interpolate(alpha2D, beta2D, gamma2D, t.color[0], t.color[1], t.color[2]);
							shader.setColor (interpolateColor);

							Vector3f pixelColor = shader.BaseVertexColor();
							setPixelColor(Vector2i(x, y), pixelColor);
							depthBuffer[getPixelIndex(x, y)] = interpolateZ2D;
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