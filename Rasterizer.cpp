#include "Rasterizer.h"

Rasterizer::Rasterizer(const int& w, const int& h) : width(w), height(h)
{
	//设置缓冲大小
	frameBuffer.resize(width * height);
	depthBuffer.resize(width * height);

	viewPlanes.resize(6, Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

	//设置视口变换矩阵
	viewportMatrix << width / 2, 0, 0, width / 2,
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

void Rasterizer::UpdateViewPlanes()
{
	ViewingFrustumPlanes(viewPlanes, projectionMatrix * viewportMatrix);
}

void Rasterizer::setModelMatrix(const Object& obj)
{
	Matrix4f rotation_in_X, rotation_in_Y, rotation_in_Z;		//XYZ轴的旋转矩阵
	float radX, radY, radZ;										//xyz轴的旋转弧度
	Matrix4f model_scale;												//缩放矩阵
	Matrix4f model_trans;												//位移矩阵

	radX = angleToRadian(obj.objectRotation.x());
	radY = angleToRadian(obj.objectRotation.y());
	radZ = angleToRadian(obj.objectRotation.z());

	rotation_in_X << 1, 0, 0, 0,
		0, cos(radX), -sin(radX), 0,
		0, sin(radX), cos(radX), 0,
		0, 0, 0, 1;
	rotation_in_Y << cos(radY), 0, sin(radY), 0,
		0, 1, 0, 0,
		-sin(radY), 0, cos(radY), 0,
		0, 0, 0, 1;
	rotation_in_Z << cos(radZ), -sin(radZ), 0, 0,
		sin(radZ), cos(radZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	model_scale << obj.objectScale.x(), 0, 0, 0,
		0, obj.objectScale.y(), 0, 0,
		0, 0, obj.objectScale.z(), 0,
		0, 0, 0, 1;

	model_trans << 1, 0, 0, -obj.objectPosition.x(),
		0, 1, 0, -obj.objectPosition.y(),
		0, 0, 1, -obj.objectPosition.z(),
		0, 0, 0, 1;

	//矩阵左乘计算出模型矩阵
	modelMatrix = model_scale * model_trans * rotation_in_Z * rotation_in_Y * rotation_in_X ;
}

void Rasterizer::setViewMatrix(const Camera& c)
{
	Matrix4f camera_trans;			//移动矩阵
	Matrix4f camera_rotation;		//旋转矩阵

	Vector3f w = -c.lookAtDir.normalized();	//摄像机的z轴
	Vector3f v = c.upDir;						//摄像机的y轴
	Vector3f u = v.cross(w);					//摄像机的x轴

	camera_trans << 1, 0, 0, -c.cameraPosition.dot(u),
				    0, 1, 0, -c.cameraPosition.dot(v),
					0, 0, 1, -c.cameraPosition.dot(w),
					0, 0, 0, 1;

	camera_rotation << u.x(), u.y(), u.z(), 0,
					   v.x(), v.y(), v.z(), 0,
					   w.x(), w.y(), w.z(), 0,
					   0, 0, 0, 1;

	//矩阵左乘计算出视图矩阵
	viewMatrix = camera_trans * camera_rotation;
}

void Rasterizer::setProjectionMatrix(const Camera& c)
{
	//透视投影矩阵
	Matrix4f perspective;							//将透视视锥转化到长方体空间中（透视投影）
	Matrix4f orthoTrans, orthoScale, ortho;			//将长方体空间投影到齐次剪裁空间中

	float t, r;			//近平面的上边界和右边界

	float radFov = angleToRadian(c.fov);
	t = tan(radFov / 2) * c.near;
	r = c.aspectRatio * t;

	float b = -t;


	float n = c.near;
	float f = c.far;

	perspective << n, 0, 0, 0,
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

	ortho = orthoScale * orthoTrans ;

	//矩阵左乘计算出透视投影矩阵
	projectionMatrix = ortho * perspective ;
}


void Rasterizer::vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightList, Camera& camera)
{
	
	//读取物体列表中的所有物体
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		setViewMatrix(camera);
		setProjectionMatrix(camera);
		UpdateViewPlanes();
		//设置MVP函数
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		//读取每个物体中的所有三角形图元
		for (Triangle& tri : object.trianglesInObject)
		{
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp0 = tri.vertex[i];
				tri.vertexInWorld[i] = modelMatrix * temp0;

				tri.vertexInWorld[i].x() /= tri.vertexInWorld[i].w();
				tri.vertexInWorld[i].y() /= tri.vertexInWorld[i].w();
				tri.vertexInWorld[i].z() /= tri.vertexInWorld[i].w();
				tri.vertexInWorld[i].w() /= tri.vertexInWorld[i].w();
			}
			//记录顶点的view空间中坐标
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp1 = tri.vertex[i];
				tri.vertexInView[i] = viewMatrix * modelMatrix * temp1;
			}
			//记录顶点的clip空间中坐标
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp2 = tri.vertexInView[i];
				tri.vertexInClip[i] = projectionMatrix * temp2;

			}
			
			//读取每个三角形图元的顶点
			for (auto& ver : tri.vertex)
			{
				//求出clip空间下坐标
				ver = mvpMatrix * ver;

				//透视除法，将w值归一，获得齐次坐标
				ver.x() /= ver.w();
				ver.y() /= ver.w();
				ver.z() /= ver.w();
				ver.w() /= ver.w();

				//将坐标点投影到屏幕(此时三角形中的vertex为屏幕坐标)
				ver = viewportMatrix * ver;
			}
			
			//将物体的法线投影到view空间
			for (auto& nor : tri.normal)
			{
				nor = viewMatrix * modelMatrix * nor;
				nor = nor.normalized();
			}
			
		}
		
	}

	for (auto& l : lightList)
	{
		//将光线从世界空间转化为view空间
		l.lightPosition = viewMatrix * l.lightPosition;

		l.lightPosition.x() /= l.lightPosition.w();
		l.lightPosition.y() /= l.lightPosition.w();
		l.lightPosition.z() /= l.lightPosition.w();
		l.lightPosition.w() /= l.lightPosition.w();
	}
}

void Rasterizer::fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightList)
{
	shader.setTexture(texture ? &*texture : nullptr);
	shader.setBumpMap(bumpMap ? &*bumpMap : nullptr);
	shader.setNormalMap(normalMap ? &*normalMap : nullptr);

	//遍历物体列表中的所有物体
	for (Object& object : objectList)
	{
		//遍历每个物体中的三角形图元
		for (Triangle& t : object.trianglesInObject)
		{
			
			//if (!ViewCull(t.vertex_in_world[0], t.vertex_in_world[1], t.vertex_in_world[2]))
			//{
				//std::cout << " >" << std::endl;
				//continue;
			//}
			
			//计算包围盒
			float min_X, max_X, min_Y, max_Y;
			min_X = width + 1;
			max_X = -1;
			min_Y = height + 1;
			max_Y = -1;
			//遍历三角形顶点
			for (const auto& ver : t.vertex)
			{
				if (ver.x() < min_X) min_X = ver.x();
				if (ver.x() > max_X) max_X = ver.x();
				if (ver.y() < min_Y) min_Y = ver.y();
				if (ver.y() > max_Y) max_Y = ver.y();
			}
			if (min_X < 0) min_X = 0;
			if (max_X > width) max_X = width;
			if (min_Y < 0) min_Y = 0;
			if (max_Y > height) max_Y = height;

			//取整
			int left_x, right_x, bottom_y, top_y;
			left_x = std::floor(min_X);
			right_x = std::ceil(max_X);
			bottom_y = std::floor(min_Y);
			top_y = std::ceil(max_Y);

			//默认光栅化
			if (ssaaState == SSAAState::close)
			{
				//对包围盒中的每一个像素
				for (int y = bottom_y; y <= top_y; ++y)
				{
					for (int x = left_x; x <= right_x; ++x)
					{
						//判断像素中心是否在三角形内(像素点在正方形中间)
						if (isInsideTriangle((float)(x + 0.5f), (float)(y + 0.5f), t))
						{
							//计算重心坐标
							float alpha, beta, gamma;
							std::tie(alpha, beta, gamma) = Barycentric2D((float)(x + 0.5f), (float)(y + 0.5f), t.vertex);

							//透视矫正插值
							//屏幕三角形内任一一点P的任意属性插值公式为：
							//I(p) = z * (alpha * I(v0) / z0 + beta * I(v1) / z1 + gamma * I(v2) / z2)
							alpha /= t.vertexInClip[0].w();
							beta /= t.vertexInClip[1].w();
							gamma /= t.vertexInClip[2].w();

							float normalizer = 1.0f / (alpha + beta  + gamma);

							//计算CVV下的z值深度
							float interpolateZ = interpolate(alpha, beta, gamma, t.vertexInClip[0].z(), t.vertexInClip[1].z(), t.vertexInClip[2].z(), normalizer);
							
							//判断深度值（数值越大代表离相机越近，记录）
							if (interpolateZ < depthBuffer[getPixelIndex(x, y)])
							{
								
								//插值出顶点各个信息
								Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
								Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
								Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertexInView[0], t.vertexInView[1], t.vertexInView[2], normalizer);
								Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

								//将插值信息记录到shader中
								shader.setColor(interpolateColor);
								shader.setNormal(interpolateNormal);
								shader.setLight(lightList);
								shader.setPosition(interpolatePosition);
								shader.setTexCoord(interploateTexcoord);

								//将颜色值赋给pixel
								setPixelColor(Vector2i(x, y), shader.setShaderType());
								//将深度值赋给buffer
								depthBuffer[getPixelIndex(x, y)] = interpolateZ;
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
				for (int y = min_Y; y < max_Y; ++y)
				{
					for (int x = min_X; x < max_X; ++x)
					{
						for (int i = 0; i < 4; i++)
						{	
							if (isInsideTriangle((float)x + msa[i][0], (float)y + msa[i][1], t))
							{
								//计算重心坐标
								float alpha, beta, gamma;
								std::tie(alpha, beta, gamma) = Barycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);
								
								alpha /= t.vertexInClip[0].w();
								beta /= t.vertexInClip[1].w();
								gamma /= t.vertexInClip[2].w();

								float normalizer = 1.0f / (alpha + beta  + gamma);

								float interpolateZ = interpolate(alpha, beta, gamma, t.vertexInClip[0].z(), t.vertexInClip[1].z(), t.vertexInClip[2].z(), normalizer);

								//判断深度值（数值越大代表离相机越近，记录）
								if (interpolateZ < depthBuffer[getPixelIndex(x, y)])
								{
										//插值出顶点各个信息
										Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
										Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
										Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertexInView[0], t.vertexInView[1], t.vertexInView[2], normalizer);
										Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

										//将插值信息记录到shader中
										shader.setColor(interpolateColor);
										shader.setNormal(interpolateNormal);
										shader.setLight(lightList);
										shader.setPosition(interpolatePosition);
										shader.setTexCoord(interploateTexcoord);

										//将颜色值赋给pixel
										Vector3f pixelColor = shader.setShaderType();
										//将深度值赋给buffer
										depthBuffer[getPixelIndex(x, y) + i] = interpolateZ;
										//将当前坐标颜色值赋给暂时的colorBuffer
										colorBuffer[getPixelIndex(x, y) + i] = pixelColor;
								}
								
							}
							
						}
						Vector3f tempPointColor = { 0, 0, 0 };
						for(int i = 0; i < 4; i++)
						{
							tempPointColor += colorBuffer[getPixelIndex(x, y) + i];
						}
						//最终像素点的颜色值为4个亚像素点的颜色值平均
						Vector3f pixelColor = tempPointColor * 0.25f;
						setPixelColor({x, y}, pixelColor);
						
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

void Rasterizer::setSSAAState()
{
	if (ssaaState == SSAAState::close)
	{
		ssaaState = SSAAState::open;
		frameBuffer.resize(width * height);
		colorBuffer.resize(width * height * 4);
		depthBuffer.resize(width * height * 4);
	
	}
	else
	{
		ssaaState = SSAAState::close;
		frameBuffer.resize(width * height);
		depthBuffer.resize(width * height);
	}
}

bool Rasterizer::ViewCull(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
{
	Vector3f minPoint, maxPoint;

	//最左边为三个点中最小值
	minPoint.x() = std::min(v1.x(), std::min(v2.x(), v3.x()));
	minPoint.y() = std::min(v1.y(), std::min(v2.y(), v3.y()));
	minPoint.z() = std::min(v1.z(), std::min(v2.z(), v3.z()));
	//最右边为三个点中最大值的向上取整
	maxPoint.x() = std::max(v1.x(), std::max(v2.x(), v3.x()));
	maxPoint.y() = std::max(v1.y(), std::max(v2.y(), v3.y()));
	maxPoint.z() = std::max(v1.z(), std::max(v2.z(), v3.z()));

	// Near 和 Far 剔除时只保留完全在内的
	if (!Point2Plane(minPoint, viewPlanes[4]) || !Point2Plane(maxPoint, viewPlanes[4])) {
		return false;
	}
	if (!Point2Plane(minPoint, viewPlanes[5]) || !Point2Plane(maxPoint, viewPlanes[5])) {
		return false;
	}

	if (!Point2Plane(minPoint, viewPlanes[0]) && !Point2Plane(maxPoint, viewPlanes[0])) {
		return false;
	}
	if (!Point2Plane(minPoint, viewPlanes[1]) && !Point2Plane(maxPoint, viewPlanes[1])) {
		return false;
	}
	if (!Point2Plane(minPoint, viewPlanes[2]) && !Point2Plane(maxPoint, viewPlanes[2])) {
		return false;
	}
	if (!Point2Plane(minPoint, viewPlanes[3]) && !Point2Plane(maxPoint, viewPlanes[3])) {
		return false;
	}
	return true;
}