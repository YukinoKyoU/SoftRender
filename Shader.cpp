#include "Shader.h"

Shader::Shader()
{
	shader_type = ShaderType::BaseVertexColor;

	point_texture = nullptr;
	point_bumpMap = nullptr;
	point_normalMap = nullptr;
}

Vector3f Shader::setShaderType()
{
	switch (shader_type)
	{ 
	case BaseVertexColor:
		return baseVertexColor();
		break;
	case NormalColor:
		return normalShader();
		break;
	case BlinnPhongShading:
		return blinnPhongShader();
		break;
	case TextureShading:
		return textureShader();
		break;
	case TextureWithBump:
		return textureWithBump();
		break;
	case TextureWithNormal:
		return textureWithNormal();
		break;
	}
}

void Shader::setColor(const Vector3f& _pointColor)
{
	point_color = _pointColor;
}
void Shader::setNormal(const Vector4f& _pointNormal)
{
	point_normal =  Vector3f( _pointNormal.x(), _pointNormal.y(), _pointNormal.z() ).normalized();
}
void Shader::setTexCoord(const Vector2f& tC)
{
	point_texCoord = tC;
}
void Shader::setTexture(Texture* t)
{
	point_texture = t;
}
void Shader::setBumpMap(Texture* bM)
{
	point_bumpMap = bM;
}
void Shader::setNormalMap(Texture* nM)
{
	point_normalMap = nM;
}
void Shader::setLight(const std::vector<Light>& _lightLists)
{
	lightLists = &_lightLists;
}
void  Shader::setPosition(const Vector4f& _pointPosition)
{
	point_position = Vector3f(_pointPosition.x(), _pointPosition.y(), _pointPosition.z());
}

Vector3f Shader::normalShader()
{
	Vector3f color = (point_normal.normalized() + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
	return color * 255;
}

Vector3f Shader::baseVertexColor()
{
	return point_color;
}

Vector3f Shader::blinnPhongShader()
{
	Vector3f ka = Vector3f(0.005, 0.005, 0.005);
	Vector3f kd = point_color / 255;
	Vector3f ks = Vector3f(0.7937, 0.7937, 0.7937);
	float p = 150;

	Vector3f temp_point_position = Vector3f( point_position.x(), point_position.y(), point_position.z() );

	Vector3f resultColor = { 0,0,0 };

	for (const Light& light : *lightLists)
	{

		Vector3f light_position = Vector3f(light.light_position.x(), light.light_position.y(), light.light_position.z());
		
		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (light_position - temp_point_position).dot(light_position - temp_point_position);
		//光照方向
		Vector3f light_dir = (light_position - temp_point_position).normalized();
		//观察方向
		Vector3f eye_dir = -temp_point_position.normalized();
		//半程向量
		Vector3f h = (eye_dir.normalized() + light_dir.normalized()).normalized();

		diffuse = kd.cwiseProduct(light.light_intensity / r) * std::max(0.0f, point_normal.dot(light_dir));
		specular = ks.cwiseProduct(light.light_intensity / r) * std::pow(std::max(0.0f, point_normal.dot(h)), p);
		ambient = ka.cwiseProduct(light.amb_light_intensity);

		resultColor += (diffuse + specular + ambient);
	}

	return resultColor * 255.f;
}

Vector3f Shader::textureShader()
{
	Vector3f diffColor = point_color;
	if (point_texture != nullptr)
	{
		diffColor = point_texture->GetColor(point_texCoord.x(), point_texCoord.y());
	}

	//环境光、漫反射和高光系数
	Vector3f ka = Vector3f(0.005, 0.005, 0.005);
	Vector3f kd = diffColor / 255;
	Vector3f ks = Vector3f(0.7937, 0.7937, 0.7937);
	float p = 150;

	Vector3f temp_point_position = Vector3f(point_position.x(), point_position.y(), point_position.z());

	Vector3f resultColor = { 0, 0, 0 };
	for (const auto& light : *lightLists)
	{
		Vector3f light_position = Vector3f(light.light_position.x(), light.light_position.y(), light.light_position.z());

		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (light_position - temp_point_position).dot(light_position - temp_point_position);
		//光照方向
		Vector3f light_dir = (light_position - temp_point_position).normalized();
		//观察方向
		Vector3f eye_dir = -temp_point_position.normalized();
		//半程向量
		Vector3f h = (eye_dir.normalized() + light_dir.normalized()).normalized();

		//计算环境光、漫反射和高光颜色
		diffuse = kd.cwiseProduct(light.light_intensity / r) * std::max(0.0f, point_normal.dot(light_dir));
		specular = ks.cwiseProduct(light.light_intensity / r) * std::pow(std::max(0.0f, point_normal.dot(h)), p);
		ambient = ka.cwiseProduct(light.amb_light_intensity);

		resultColor += ambient + diffuse + specular;
	}
	return resultColor * 255.f;
}
Vector3f Shader::textureWithBump()
{
	//处理法线
	if (point_bumpMap != nullptr)
	{
		float kh = 0.2, kn = 0.1;
		Vector3f n = point_normal;
		Vector3f t = Vector3f(n.x() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()), sqrt(n.x() * n.x() + n.z() * n.z()), n.z() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()));
		Vector3f b = n.cross(t);
		Matrix3f TBN;
		TBN << t.x(), b.x(), n.x(),
			t.y(), b.y(), n.y(),
			t.z(), b.z(), n.z();
		float u = point_texCoord.x();
		float v = point_texCoord.y();
		float h = point_bumpMap->GetHeight();
		float w = point_bumpMap->GetWidth();
		float dU = kh * kn * (point_bumpMap->GetColor(u + 1 / w, v).norm() - point_bumpMap->GetColor(u, v).norm());
		float dV = kh * kn * (point_bumpMap->GetColor(u, v + 1 / h).norm() - point_bumpMap->GetColor(u, v).norm());
		Vector3f ln{ -dU,-dV,1 };
		point_normal = (TBN * ln).normalized();
	}
	return textureShader();
}

Vector3f Shader::textureWithNormal()
{
	//处理法线
	if (point_normalMap != nullptr)
	{
		float kh = 0.2, kn = 0.1;
		Vector3f n = point_normal;
		Vector3f t = Vector3f(n.x() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()), sqrt(n.x() * n.x() + n.z() * n.z()), n.z() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()));
		Vector3f b = n.cross(t);
		Matrix3f TBN;
		TBN << t.x(), b.x(), n.x(),
			t.y(), b.y(), n.y(),
			t.z(), b.z(), n.z();
		float u = point_texCoord.x();
		float v = point_texCoord.y();
		Vector3f ln = ((point_normalMap->GetColor(u, v)) / 255.f) * 2.f - Vector3f(1, 1, 1);
		point_normal = (TBN * ln).normalized();
	}
	return textureShader();
}
