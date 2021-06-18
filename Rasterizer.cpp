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

	texture = std::nullopt;
	bumpMap = std::nullopt;
	normalMap = std::nullopt;
}

Rasterizer::~Rasterizer()
{
	clearBuffer();
}

void Rasterizer::setModelMatrix(const Object& obj)
{
	Matrix4f rX, rY, rZ;		//XYZ轴的旋转矩阵
	float radX, radY, radZ;		//xyz轴的旋转弧度
	Matrix4f scale;				//缩放矩阵
	Matrix4f trans;				//位移矩阵

	radX = angleToRadian(obj.object_rotation.x());
	radY = angleToRadian(obj.object_rotation.y());
	radZ = angleToRadian(obj.object_rotation.z());

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

	scale << obj.object_scale.x(), 0, 0, 0,
		     0, obj.object_scale.y(), 0, 0,
			 0, 0, obj.object_scale.z(), 0,
			 0, 0, 0, 1;

	trans << 1, 0, 0, -obj.object_position.x(),
			 0, 1, 0, -obj.object_position.y(),
			 0, 0, 1, -obj.object_position.z(),
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

	t << 1, 0, 0, -c.camera_position.x(),
		 0, 1, 0, -c.camera_position.y(),
		 0, 0, 1, -c.camera_position.z(),
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

	radFov = angleToRadian(c.fov);
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

void Rasterizer::vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightList, Camera& camera)
{
	//读取物体列表中的所有物体
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		setViewMatrix(camera);
		setProjectionMatrix(camera);

		//设置MVP函数
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		//读取每个物体中的所有三角形图元
		for (Triangle& tri : object.triangles)
		{
			//先将顶点坐标在view空间下保存下来
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp1 = tri.vertex[i];
				tri.vertex_in_view[i] = viewMatrix * modelMatrix * temp1;
			}

			for (int i = 0; i < 3; i++)
			{
				Vector4f temp2 = tri.vertex_in_view[i];
				tri.vertex_in_clip[i] = projectionMatrix * temp2;
			}
			
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
			/*
			for (auto& nor : tri.normal)
			{
				nor = viewMatrix * modelMatrix * nor;
				nor = nor.normalized();
			}
			*/
		}
	}

	for (auto& l : lightList)
	{
		//将光线从世界空间转化为相机空间
		l.light_position = viewMatrix * l.light_position;

		l.light_position.x() /= l.light_position.w();
		l.light_position.y() /= l.light_position.w();
		l.light_position.z() /= l.light_position.w();
		l.light_position.w() /= l.light_position.w();
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
static std::tuple<float, float, float> Barycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

//重心坐标插值（纹理等二维坐标）
static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//重心坐标插值（颜色等三维坐标）
static Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Vector3f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//重心坐标插值（坐标，法线等齐次维坐标）
static Vector4f interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}
//重心坐标插值（深度值）
static float interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3, float Z)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) * Z;
}

//片元着色器
void Rasterizer::fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightList)
{
	shader.SetTexture(texture ? &*texture : nullptr);
	shader.SetBumpMap(bumpMap ? &*bumpMap : nullptr);
	shader.SetNormalMap(normalMap ? &*normalMap : nullptr);
	//遍历物体列表中的所有物体
	for (Object& object : objectList)
	{
		//遍历每个物体中的三角形图元
		for (Triangle& t : object.triangles)
		{
			//计算包围盒
			float minX, maxX, minY, maxY;
			minX = width;
			maxX = 0;
			minY = height;
			maxY = 0;
			//遍历三角形顶点
			for (const auto& ver : t.vertex)
			{
				if (ver.x() < minX) minX = ver.x();
				if (ver.x() > maxX) maxX = ver.x();
				if (ver.y() < minY) minY = ver.y();
				if (ver.y() > maxY) maxY = ver.y();
			}
			if (minX < 0) minX = 0;
			if (maxX > width) maxX = width;
			if (minY < 0) minY = 0;
			if (maxY > height) maxY = height;
			//取整
			int left_x, right_x, bottom_y, top_y;
			left_x = std::floor(minX);
			right_x = std::ceil(maxX);
			bottom_y = std::floor(minY);
			top_y = std::ceil(maxY);

			if (msaaState == close)
			{
				//对包围盒中的每一个像素
				for (int y = bottom_y; y <= top_y; ++y)
				{
					for (int x = left_x; x <= right_x; ++x)
					{
						//判断像素中心是否在三角形内(像素点在正方形中间)
						if (isInsideTriangle((float)x + 0.5f, (float)y + 0.5f, t))
						{
							//计算重心坐标
							float alpha, beta, gamma;
							std::tie(alpha, beta, gamma) = Barycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);

							//透视矫正插值
							alpha /= t.vertex_in_view[0].z();
							beta /= t.vertex_in_view[1].z();
							gamma /= t.vertex_in_view[2].z();
							float normalizer = 1.0f / (alpha + beta  + gamma);

							//计算NDC下的z值深度
							float interpolate_z = interpolate(alpha, beta, gamma, t.vertex_in_clip[0].z(), t.vertex_in_clip[1].z(), t.vertex_in_clip[2].z(), normalizer);
							
							//对深度值做反转，保证深度值越小，离相机越远
							interpolate_z *= -1;
							
							//判断深度值（数值越大代表离相机越近，记录）
							if (interpolate_z < depthBuffer[getPixelIndex(x, y)])
							{
								
								//插值出顶点各个信息
								Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
								Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
								Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertex_in_view[0], t.vertex_in_view[1], t.vertex_in_view[2], normalizer);
								Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

								//将插值信息记录到shader中
								shader.setColor(interpolateColor);
								shader.setNormal(interpolateNormal);
								shader.setLight(lightList);
								shader.setPosition(interpolatePosition);
								shader.setTexCoord(interploateTexcoord);

								//将颜色值赋给pixel
								Vector3f pixelColor = shader.TextureWithBump();
								setPixelColor(Vector2i(x, y), pixelColor);
								//将深度值赋给buffer
								depthBuffer[getPixelIndex(x, y)] = interpolate_z;
							}
						}
					}
				}
			}
			//开启MSAA
			else
			{
				//设置像素点周围4个点坐标
				std::vector<Vector2f> msa =
				{
					{-0.25, -0.25},
					{-0.25, +0.25},
					{+0.25, -0.25},
					{+0.25, +0.25}
				};
				for (int y = minY; y < maxY; ++y)
				{
					for (int x = minX; x < maxX; ++x)
					{
						for (int i = 0; i < 4; i++)
						{	
							if (isInsideTriangle((float)x + msa[i][0], (float)y + msa[i][1], t))
							{
								//计算重心坐标
								float alpha, beta, gamma;
								std::tie(alpha, beta, gamma) = Barycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);

								alpha /= t.vertex_in_view[0].z();
								beta /= t.vertex_in_view[1].z();
								gamma /= t.vertex_in_view[2].z();

								float normalizer = 1.0f / (alpha + beta  + gamma);

								float interpolate_z = interpolate(alpha, beta, gamma, t.vertex_in_clip[0].z(), t.vertex_in_clip[1].z(), t.vertex_in_clip[2].z(), normalizer);
							
								//对深度值做反转，保证深度值越小，离相机越远
								interpolate_z *= -1;
							
								//判断深度值（数值越大代表离相机越近，记录）
								if (interpolate_z < depthBuffer[getPixelIndex(x, y)])
								{
										//插值出顶点各个信息
										Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
										Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
										Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertex_in_view[0], t.vertex_in_view[1], t.vertex_in_view[2], normalizer);
										Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

										//将插值信息记录到shader中
										shader.setColor(interpolateColor);
										shader.setNormal(interpolateNormal);
										shader.setLight(lightList);
										shader.setPosition(interpolatePosition);
										shader.setTexCoord(interploateTexcoord);

										//将颜色值赋给pixel
										Vector3f pixelColor = shader.BlinnPhongShader();
										//将深度值赋给buffer
										depthBuffer[getPixelIndex(x, y) + i] = interpolate_z;
										//将当前坐标颜色值赋给暂时的colorBuffer
										colorBuffer[getPixelIndex(x, y) + i] = pixelColor;
								}
								
							}
							
						}
						Vector3f point_color = { 0, 0, 0 };
						for(int i = 0; i < 4; i++)
						{
							point_color += colorBuffer[getPixelIndex(x, y) + i];
						}
						//最终像素点的颜色值为4个亚像素点的颜色值平均
						Vector3f pixel_color = point_color * 0.25f;
						setPixelColor({x, y}, pixel_color);
						
					}
					
				}
			}
		}
	}
}

void Rasterizer::setPixelColor(const Vector2i point, const Vector3f color)
{
	int ind = (height - point.y()) * width + point.x();
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

void Rasterizer::SetTexture(Texture tex)
{
	texture = tex;
}

void Rasterizer::SetBumpMap(Texture bTex)
{
	bumpMap = bTex;
}

void Rasterizer::SetNormalMap(Texture nTex)
{
	normalMap = nTex;
}

void Rasterizer::setMSAAState()
{
	if (msaaState == close)
	{
		msaaState = open;
		frameBuffer.resize(width * height);
		colorBuffer.resize(width * height * 4);
		depthBuffer.resize(width * height * 4);
	
	}
	else
	{
		msaaState = close;
		frameBuffer.resize(width * height);
		depthBuffer.resize(width * height);
	}
}