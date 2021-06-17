#include "Rasterizer.h"

Rasterizer::Rasterizer(const int& w, const int& h) : width(w), height(h)
{
	//���û����С
	frameBuffer.resize(width * height);
	depthBuffer.resize(width * height);

	//�����ӿڱ任����
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

void Rasterizer::setModelMatrix(const Object& o)
{
	Matrix4f rX, rY, rZ;		//XYZ�����ת����
	float radX, radY, radZ;		//xyz�����ת����
	Matrix4f scale;				//���ž���
	Matrix4f trans;				//λ�ƾ���

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
	//������˼����ģ�;���
	modelMatrix =  scale * rZ * rY * rX * trans;
}

void Rasterizer::setViewMatrix(const Camera& c)
{
	//��������ƶ���ԭ�㣬Ȼ��ʹ����ת����������������������
	Matrix4f t;		//�ƶ�����
	Vector3f cX;	//�������x��
	Matrix4f r;		//��ת�������ת����

	t << 1, 0, 0, -c.position.x(),
		 0, 1, 0, -c.position.y(),
		 0, 0, 1, -c.position.z(),
		 0, 0, 0, 1;

	//����x�᷽��
	cX = c.lookAt.cross(c.up);

	r << cX.x(),   cX.y(),   cX.z(), 0,
		 c.up.x(), c.up.y(), c.up.z(), 0,
		 c.lookAt.x(), c.lookAt.y(), c.lookAt.z(), 0,
		 0, 0, 0, 1;
	//������˼������ͼ����
	viewMatrix = r * t;
}

void Rasterizer::setProjectionMatrix(const Camera& c)
{
	//͸��ͶӰ����
	Matrix4f projection;							//��͸����׶ת�������ÿռ���
	Matrix4f orthoTrans, orthoScale, ortho;			//���ÿռ�ת��ΪCVV

	float t, r;			//��ƽ����ϱ߽���ұ߽�
	float radFov;		//��Ұ�Ļ�����

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

	//������˼����͸��ͶӰ����
	projectionMatrix = ortho * projection;

}

void Rasterizer::vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightLists, Camera& camera)
{
	//��ȡ�����б��е���������
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		setViewMatrix(camera);
		setProjectionMatrix(camera);

		//����MVP����
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		//��ȡÿ�������е�����������ͼԪ
		for (Triangle& tri : object.triangles)
		{
			//��ȡÿ��������ͼԪ�Ķ���
			for (auto& vec : tri.vertex)
			{
				//���CVV�ռ�������
				vec = mvpMatrix * vec;
				//�������ͶӰ����Ļ
				vec = viewPortMatrix * vec;
				//͸�ӳ�������wֵ��һ������������
				vec.x() /= vec.w();
				vec.y() /= vec.w();
				vec.z() /= vec.w();
				vec.w() /= vec.w();
			}
			for (auto& n : tri.normal)
			{
				n = viewMatrix * modelMatrix * n;
			}
		}
	}

	for (auto& l : lightLists)
	{
		l.position = viewMatrix * l.position;

		l.position.x() /= l.position.w();
		l.position.y() /= l.position.w();
		l.position.z() /= l.position.w();
		l.position.w() /= l.position.w();
	}
}

//�ж����ص��Ƿ�����������
static bool isInsideTriangle(const float x, const float y, const Triangle& t)
{
	//�������ص㣨��Σ�����
	Vector3f temPixel(x, y, 1.0f);
	//����������������������꣨��zֵ��һ��
	std::vector<Vector3f> vertexArray(3);
	vertexArray[0] = { t.vertex[0].x(), t.vertex[0].y(), 1.0f};
	vertexArray[1] = { t.vertex[1].x(), t.vertex[1].y(), 1.0f};
	vertexArray[2] = { t.vertex[2].x(), t.vertex[2].y(), 1.0f};

	//���������ߵ���������ʱ�뷽��
	Vector3f edge1, edge2, edge3;
	edge1 = vertexArray[1] - vertexArray[0];
	edge2 = vertexArray[2] - vertexArray[1];
	edge3 = vertexArray[0] - vertexArray[2];
	
	//���ص㵽�������������
	Vector3f p2Ver1, p2Ver2, p2Ver3;
	p2Ver1 = temPixel - vertexArray[0];
	p2Ver2 = temPixel - vertexArray[1];
	p2Ver3 = temPixel - vertexArray[2];

	//������ڲ������������������Ĳ��ȫ��ͬ��
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

//��һ�������������
static std::tuple<float, float, float> Barycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

//���������ֵ������ȶ�ά���꣩
static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}
//���������ֵ����ɫ����ά���꣩
static Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Vector3f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}
//���������ֵ�����꣬���ߵ����ά���꣩
static Vector4f interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

//���������ֵ�����ֵ��
static float interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

//ƬԪ��ɫ��
void Rasterizer::fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightLists)
{
	shader.SetTexture(texture ? &*texture : nullptr);
	shader.SetBumpMap(bumpMap ? &*bumpMap : nullptr);
	shader.SetNormalMap(normalMap ? &*normalMap : nullptr);
	//���������б��е���������
	for (Object& object : objectList)
	{
		//����ÿ�������е�������ͼԪ
		for (Triangle& t : object.triangles)
		{
			//�����Χ��
			float minXf, maxXf, minYf, maxYf;
			minXf = width;
			maxXf = 0;
			minYf = height;
			maxYf = 0;
			//���������ζ���
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
			//ȡ��
			int minX, maxX, minY, maxY;
			minX = floor(minXf);
			maxX = ceil(maxXf);
			minY = floor(minYf);
			maxY = ceil(maxYf);

			if (msaaState == close)
			{
				//�԰�Χ���е�ÿһ������
				for (int j = minY; j <= maxY; ++j)
				{
					for (int i = minX; i <= maxX; ++i)
					{
						float x = static_cast<float>(i) + 0.5f;
						float y = static_cast<float>(j) + 0.5f;
						//�ж����������Ƿ�����������(���ص����������м�)
						if (isInsideTriangle(x, y, t))
						{
							
							//������������
							float alpha, beta, gamma;
							std::tie(alpha, beta, gamma) = Barycentric2D(x, y, t.vertex);
							//͸�ӽ�����ֵ
							float Z = 1.0f/(alpha / t.vertex[0].w() + beta / t.vertex[1].w() + gamma / t.vertex[2].w());
							//��ֵ����������ص����
							float interpolate_z = interpolate(alpha, beta, gamma, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());
							//͸�ӽ����������ȷ�����ֵ
							interpolate_z *= Z;

							//�������λ��ת����view�����£���֤��ֵ����ȷ���
							Vector4f position[3] =
							{
								projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[0],
								projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[1],
								projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[2],
							};
							
							//�����ֵ����ת����֤���ֵԽС�������ԽԶ
							interpolate_z *= -1;
							//�ж����ֵ����ֵԽ����������Խ������¼��
							if (interpolate_z < depthBuffer[getPixelIndex(x, y)])
							{
								//��ֵ����ɫ
								Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2]);
								//��ֵ������
								Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2]).normalized();
								interpolateNormal *= Z;
								//��ֵ������
								Vector4f interpolatePosition = interpolate(alpha, beta, gamma, position[0], position[1], position[2]);
								Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2]);
								
								//����ֵ��Ϣ��¼��shader��
								shader.setColor(interpolateColor);
								shader.setNormal(interpolateNormal);
								shader.setLight(lightLists);
								shader.setPosition(interpolatePosition);
								shader.setTexCoord(interploateTexcoord);

								//����ɫֵ����pixel
								Vector3f pixelColor = shader.TextureShader();
								setPixelColor(Vector2i(x, y), pixelColor);
								//�����ֵ����buffer
								depthBuffer[getPixelIndex(x, y)] = interpolate_z;
							}
						}
					}
				}
			}
			//����MSAA
			//else
			{
				//�������ص���Χ4��������
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
						{	/*
							if (isInsideTriangle((float)x + msa[i][0], (float)y + msa[i][1], t))
							{
								//������������
								float alpha, beta, gamma;
								std::tie(alpha, beta, gamma) = computeBarycentric2D((float)x + msa[i][0], (float)y + msa[i][1], t.vertex);
								
								//͸�ӽ�����ֵ
								float Z = 1.0 / (alpha / t.vertex[0].w() + beta / t.vertex[1].w() + gamma / t.vertex[2].w());
								//��ֵ����������ص����
								float interpolate_z = interpolate(alpha, beta, gamma, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());
								//͸�ӽ����������ȷ�����ֵ
								interpolate_z /= Z;
								//�����ֵ����ת����֤���ֵԽС�������ԽԶ
								interpolate_z *= -1;

								//�������λ��ת����view�����£���֤��ֵ����ȷ���
								Vector4f position[3] =
								{
									projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[0],
									projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[1],
									projectionMatrix.inverse() * viewPortMatrix.inverse() * t.vertex[2],
								};
								
								//�ж����ֵ����ļ�¼��
								if (interpolate_z < depthBuffer[getPixelIndex(x, y) + i])
								{
									//��ȸ����Ļ���ֵ����ɫ��Ȼ����������Ϣ
									Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2]);
									//��ֵ������
									Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2]).normalized();
									//��ֵ������
									Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertex[0], t.vertex[1], t.vertex[2]);
									
									//����ֵ��Ϣ��¼��shader��
									shader.setColor(interpolateColor);
									shader.setNormal(interpolateNormal);
									shader.setLight(lightLists);
									shader.setPosition(interpolatePosition);

									//����ɫֵ����pixel
									Vector3f pixelColor = shader.BlinnPhongShader(camera);

									//�����ֵ����buffer
									depthBuffer[getPixelIndex(x, y) + i] = interpolate_z;
									//����ǰ������ɫֵ������ʱ��colorBuffer
									colorBuffer[getPixelIndex(x, y) + i] = pixelColor;
								}
								
							}
							*/
						}
						/*
						Vector3f point_color = { 0, 0, 0 };
						for(int i = 0; i < 4; i++)
						{
							point_color += colorBuffer[getPixelIndex(x, y) + i];
						}
						//�������ص����ɫֵΪ4�������ص����ɫֵƽ��
						Vector3f pixel_color = point_color * 0.25f;
						setPixelColor({x, y}, pixel_color);
						*/
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