#include "Rasterizer.h"

Rasterizer::Rasterizer(const int& w, const int& h) : width(w), height(h)
{
	//���û����С
	frameBuffer.resize(width * height);
	depthBuffer.resize(width * height);

	//�����ӿڱ任����
	viewport_matrix << width / 2, 0, 0, width / 2,
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
	Matrix4f rotation_in_X, rotation_in_Y, rotation_in_Z;		//XYZ�����ת����
	float radX, radY, radZ;										//xyz�����ת����
	Matrix4f model_scale;												//���ž���
	Matrix4f model_trans;												//λ�ƾ���

	radX = angleToRadian(obj.object_rotation.x());
	radY = angleToRadian(obj.object_rotation.y());
	radZ = angleToRadian(obj.object_rotation.z());

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

	model_scale << obj.object_scale.x(), 0, 0, 0,
		0, obj.object_scale.y(), 0, 0,
		0, 0, obj.object_scale.z(), 0,
		0, 0, 0, 1;

	model_trans << 1, 0, 0, -obj.object_position.x(),
		0, 1, 0, -obj.object_position.y(),
		0, 0, 1, -obj.object_position.z(),
		0, 0, 0, 1;

	//������˼����ģ�;���
	model_matrix = model_scale * rotation_in_Z * rotation_in_Y * rotation_in_X * model_trans;
}
/*
void Rasterizer::setViewMatrix(const Camera& c)
{
	Matrix4f camera_trans;			//�ƶ�����
	Matrix4f camera_rotation;		//��ת����

	Vector3f w = -c.lookAt_dir.normalized();	//�������z��
	Vector3f v = c.up_dir;						//�������y��
	Vector3f u = v.cross(w);					//�������x��

	camera_trans << 1, 0, 0, -c.camera_position.x(),
		0, 1, 0, -c.camera_position.y(),
		0, 0, 1, -c.camera_position.z(),
		0, 0, 0, 1;

	camera_rotation << u.x(), u.y(), u.z(), 0,
		v.x(), v.y(), v.z(), 0,
		w.x(), w.y(), w.z(), 0,
		0, 0, 0, 1;

	//������˼������ͼ����
	view_matrix = camera_rotation * camera_trans;
}

void Rasterizer::setProjectionMatrix(const Camera& c)
{
	//͸��ͶӰ����
	Matrix4f perspective;							//��͸����׶ת����������ռ��У�͸��ͶӰ��
	Matrix4f orthoTrans, orthoScale, ortho;			//��������ռ�ͶӰ����μ��ÿռ���

	float t, r;			//��ƽ����ϱ߽���ұ߽�

	float radFov = angleToRadian(c.fov);
	t = tan(radFov / 2) * c.near;
	r = c.aspectRatio * t;

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

	ortho = orthoScale * orthoTrans;

	//������˼����͸��ͶӰ����
	projection_matrix = ortho * perspective ;
}
*/
void Rasterizer::vertexShader(std::vector<Object>& objectList, std::vector<Light>& lightList, Camera& camera)
{
	//��ȡ�����б��е���������
	for (Object& object : objectList)
	{	
		setModelMatrix(object);
		//setViewMatrix(camera);
		//setProjectionMatrix(camera);

		//����MVP����
		mvp_matrix = projection_matrix * view_matrix * model_matrix;

		//��ȡÿ�������е�����������ͼԪ
		for (Triangle& tri : object.triangles_in_object)
		{
			//��¼�����view�ռ�������
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp1 = tri.vertex[i];
				tri.vertex_in_view[i] = view_matrix * model_matrix * temp1;
			}
			//��¼�����clip�ռ�������
			for (int i = 0; i < 3; i++)
			{
				Vector4f temp2 = tri.vertex_in_view[i];
				tri.vertex_in_clip[i] = projection_matrix * temp2;

			}
			
			//��ȡÿ��������ͼԪ�Ķ���
			for (auto& ver : tri.vertex)
			{
				//���clip�ռ�������
				ver = mvp_matrix * ver;

				//͸�ӳ�������wֵ��һ������������
				ver.x() /= ver.w();
				ver.y() /= ver.w();
				ver.z() /= ver.w();
				ver.w() /= ver.w();

				//�������ͶӰ����Ļ(��ʱ�������е�vertexΪ��Ļ����)
				ver = viewport_matrix * ver;
			}
			
			//������ķ���ͶӰ��view�ռ�
			for (auto& nor : tri.normal)
			{
				nor = view_matrix * model_matrix * nor;
				nor = nor.normalized();
			}
			
		}
	}

	for (auto& l : lightList)
	{
		//�����ߴ�����ռ�ת��Ϊview�ռ�
		l.light_position = view_matrix * l.light_position;

		l.light_position.x() /= l.light_position.w();
		l.light_position.y() /= l.light_position.w();
		l.light_position.z() /= l.light_position.w();
		l.light_position.w() /= l.light_position.w();
	}
}

void Rasterizer::fragmentShader(std::vector<Object>& objectList, std::vector<Light>& lightList)
{
	shader.setTexture(texture ? &*texture : nullptr);
	shader.setBumpMap(bumpMap ? &*bumpMap : nullptr);
	shader.setNormalMap(normalMap ? &*normalMap : nullptr);

	//���������б��е���������
	for (Object& object : objectList)
	{
		//����ÿ�������е�������ͼԪ
		for (Triangle& t : object.triangles_in_object)
		{
			//�����Χ��
			float min_X, max_X, min_Y, max_Y;
			min_X = width + 1;
			max_X = -1;
			min_Y = height + 1;
			max_Y = -1;
			//���������ζ���
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

			//ȡ��
			int left_x, right_x, bottom_y, top_y;
			left_x = std::floor(min_X);
			right_x = std::ceil(max_X);
			bottom_y = std::floor(min_Y);
			top_y = std::ceil(max_Y);

			//Ĭ�Ϲ�դ��
			if (ssaaState == SSAAState::close)
			{
				//�԰�Χ���е�ÿһ������
				for (int y = bottom_y; y <= top_y; ++y)
				{
					for (int x = left_x; x <= right_x; ++x)
					{
						//�ж����������Ƿ�����������(���ص����������м�)
						if (isInsideTriangle((float)(x + 0.5f), (float)(y + 0.5f), t))
						{
							//������������
							float alpha, beta, gamma;
							std::tie(alpha, beta, gamma) = Barycentric2D((float)(x + 0.5f), (float)(y + 0.5f), t.vertex);

							//͸�ӽ�����ֵ
							//��Ļ����������һһ��P���������Բ�ֵ��ʽΪ��
							//I(p) = z * (alpha * I(v0) / z0 + beta * I(v1) / z1 + gamma * I(v2) / z2)
							alpha /= t.vertex_in_clip[0].w();
							beta /= t.vertex_in_clip[1].w();
							gamma /= t.vertex_in_clip[2].w();

							//std::cout << t.vertex_in_view[0].z() << "=" << t.vertex_in_clip[0].w() << std::endl;

							float normalizer = 1.0f / (alpha + beta  + gamma);

							//����CVV�µ�zֵ���
							float interpolate_z = interpolate(alpha, beta, gamma, t.vertex_in_clip[0].z(), t.vertex_in_clip[1].z(), t.vertex_in_clip[2].z(), normalizer);
							
							//�����ֵ����ת����֤���ֵԽС�������ԽԶ
							//interpolate_z *= -1;
							
							//�ж����ֵ����ֵԽ����������Խ������¼��
							if (interpolate_z < depthBuffer[getPixelIndex(x, y)])
							{
								
								//��ֵ�����������Ϣ
								Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
								Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
								Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertex_in_view[0], t.vertex_in_view[1], t.vertex_in_view[2], normalizer);
								Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

								//����ֵ��Ϣ��¼��shader��
								shader.setColor(interpolateColor);
								shader.setNormal(interpolateNormal);
								shader.setLight(lightList);
								shader.setPosition(interpolatePosition);
								shader.setTexCoord(interploateTexcoord);

								//����ɫֵ����pixel
								setPixelColor(Vector2i(x, y), shader.setShaderType());
								//�����ֵ����buffer
								depthBuffer[getPixelIndex(x, y)] = interpolate_z;
							}
						}
					}
				}
			}
			//����MSAA
			else
			{
				//�������ص���Χ4��������
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
								//������������
								float alpha, beta, gamma;
								std::tie(alpha, beta, gamma) = Barycentric2D((float)x + 0.5f, (float)y + 0.5f, t.vertex);

								alpha /= t.vertex_in_view[0].z();
								beta /= t.vertex_in_view[1].z();
								gamma /= t.vertex_in_view[2].z();

								float normalizer = 1.0f / (alpha + beta  + gamma);

								float interpolate_z = interpolate(alpha, beta, gamma, t.vertex_in_clip[0].z(), t.vertex_in_clip[1].z(), t.vertex_in_clip[2].z(), normalizer);
							
								//�����ֵ����ת����֤���ֵԽС�������ԽԶ
								interpolate_z *= -1;
							
								//�ж����ֵ����ֵԽ����������Խ������¼��
								if (interpolate_z < depthBuffer[getPixelIndex(x, y)])
								{
										//��ֵ�����������Ϣ
										Vector3f interpolateColor = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], normalizer);
										Vector4f interpolateNormal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], normalizer).normalized();
										Vector4f interpolatePosition = interpolate(alpha, beta, gamma, t.vertex_in_view[0], t.vertex_in_view[1], t.vertex_in_view[2], normalizer);
										Vector2f interploateTexcoord = interpolate(alpha, beta, gamma, t.texCoord[0], t.texCoord[1], t.texCoord[2], normalizer);

										//����ֵ��Ϣ��¼��shader��
										shader.setColor(interpolateColor);
										shader.setNormal(interpolateNormal);
										shader.setLight(lightList);
										shader.setPosition(interpolatePosition);
										shader.setTexCoord(interploateTexcoord);

										//����ɫֵ����pixel
										Vector3f pixelColor = shader.setShaderType();
										//�����ֵ����buffer
										depthBuffer[getPixelIndex(x, y) + i] = interpolate_z;
										//����ǰ������ɫֵ������ʱ��colorBuffer
										colorBuffer[getPixelIndex(x, y) + i] = pixelColor;
								}
								
							}
							
						}
						Vector3f point_color = { 0, 0, 0 };
						for(int i = 0; i < 4; i++)
						{
							point_color += colorBuffer[getPixelIndex(x, y) + i];
						}
						//�������ص����ɫֵΪ4�������ص����ɫֵƽ��
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

void Rasterizer::setViewMatrix(const Matrix4f& v)
{
	view_matrix = v;
}
void Rasterizer::setProjectionMatrix(const Matrix4f& p)
{
	projection_matrix = p;
}

