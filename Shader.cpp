#include "Shader.h"

Shader::Shader()
{
	pointTexture = nullptr;
	pointBumpMap = nullptr;
	pointNormalMap = nullptr;
}

void Shader::setColor(const Vector3f& _pointColor)
{
	pointColor = _pointColor;
}
Vector3f Shader::baseVertexColor()
{
	return pointColor;
}

void Shader::setNormal(const Vector4f& _pointNormal)
{
	pointNormal =  Vector3f( _pointNormal.x(), _pointNormal.y(), _pointNormal.z() ).normalized();
}

Vector3f Shader::normalShader()
{
	Vector3f color = (pointNormal.normalized() + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
	return color * 255;
}

void Shader::setLight(const std::vector<Light>& _lightLists)
{
	lightLists = &_lightLists;
}
void  Shader::setPosition(const Vector4f& _pointPosition)
{
	pointPosition = Vector3f(_pointPosition.x(), _pointPosition.y(), _pointPosition.z());
}

Vector3f Shader::BlinnPhongShader()
{
	Vector3f ka = Vector3f(0.005, 0.005, 0.005);
	Vector3f kd = pointColor / 255;
	Vector3f ks = Vector3f(0.7937, 0.7937, 0.7937);
	float p = 150;

	Vector3f point_position = Vector3f( pointPosition.x(), pointPosition.y(), pointPosition.z() );

	Vector3f resultColor = { 0,0,0 };

	for (const Light& light : *lightLists)
	{

		Vector3f light_position = Vector3f(light.position.x(), light.position.y(), light.position.z());
		
		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (light_position - point_position).dot(light_position - point_position);
		//光照方向
		Vector3f light_dir = (light_position - point_position).normalized();
		//观察方向
		Vector3f eye_dir = - point_position.normalized();
		//半程向量
		Vector3f h = (eye_dir + light_dir).normalized();

		diffuse = kd.cwiseProduct(light.intensity / r) * std::max(0.0f, pointNormal.dot(light_dir));
		specular = ks.cwiseProduct(light.intensity / r) * std::pow(std::max(0.0f, pointNormal.dot(h)), p);
		ambient = ka.cwiseProduct(light.amb_light_intensity);

		resultColor += (diffuse + specular + ambient);
	}

	return resultColor * 255.f;
}

Vector3f Shader::TextureShader()
{
	Vector3f diffColor = pointColor;
	if (pointTexture != nullptr)
	{
		diffColor = pointTexture->GetColor(pointTexCoord.x(), pointTexCoord.y());
	}

	//环境光、漫反射和高光系数
	Vector3f ka = Vector3f(0.005, 0.005, 0.005);
	Vector3f kd = diffColor / 255;
	Vector3f ks = Vector3f(0.7937, 0.7937, 0.7937);
	float p = 100;

	Vector3f point_position = Vector3f(pointPosition.x(), pointPosition.y(), pointPosition.z());

	Vector3f resultColor = { 0, 0, 0 };
	for (const auto& light : *lightLists)
	{
		Vector3f light_position = Vector3f(light.position.x(), light.position.y(), light.position.z());

		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (light_position - point_position).dot(light_position - point_position);
		//光照方向
		Vector3f light_dir = (light_position - point_position).normalized();
		//观察方向
		Vector3f eye_dir = -point_position.normalized();
		//半程向量
		Vector3f h = (eye_dir + light_dir).normalized();

		//计算环境光、漫反射和高光颜色
		diffuse = kd.cwiseProduct(light.intensity / r) * std::max(0.0f, pointNormal.dot(light_dir));
		specular = ks.cwiseProduct(light.intensity / r) * std::pow(std::max(0.0f, pointNormal.dot(h)), p);
		ambient = ka.cwiseProduct(light.amb_light_intensity);

		resultColor += ambient + diffuse + specular;
	}
	return resultColor * 255.f;
}
Vector3f Shader::TextureWithBump()
{
	//处理法线
	if (pointBumpMap != nullptr)
	{
		float kh = 0.2, kn = 0.1;
		Vector3f n = pointNormal;
		Vector3f t = Vector3f(n.x() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()), sqrt(n.x() * n.x() + n.z() * n.z()), n.z() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()));
		Vector3f b = n.cross(t);
		Matrix3f TBN;
		TBN << t.x(), b.x(), n.x(),
			t.y(), b.y(), n.y(),
			t.z(), b.z(), n.z();
		float u = pointTexCoord.x();
		float v = pointTexCoord.y();
		float h = pointBumpMap->GetHeight();
		float w = pointBumpMap->GetWidth();
		float dU = kh * kn * (pointBumpMap->GetColor(u + 1 / w, v).norm() - pointBumpMap->GetColor(u, v).norm());
		float dV = kh * kn * (pointBumpMap->GetColor(u, v + 1 / h).norm() - pointBumpMap->GetColor(u, v).norm());
		Vector3f ln{ -dU,-dV,1 };
		pointNormal = (TBN * ln).normalized();
	}
	return TextureShader();
}

Vector3f Shader::TextureWithNormal()
{
	//处理法线
	if (pointNormalMap != nullptr)
	{
		float kh = 0.2, kn = 0.1;
		Vector3f n = pointNormal;
		Vector3f t = Vector3f(n.x() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()), sqrt(n.x() * n.x() + n.z() * n.z()), n.z() * n.y() / sqrt(n.x() * n.x() + n.z() * n.z()));
		Vector3f b = n.cross(t);
		Matrix3f TBN;
		TBN << t.x(), b.x(), n.x(),
			t.y(), b.y(), n.y(),
			t.z(), b.z(), n.z();
		float u = pointTexCoord.x();
		float v = pointTexCoord.y();
		Vector3f ln = ((pointNormalMap->GetColor(u, v)) / 255.f) * 2.f - Vector3f(1, 1, 1);
		pointNormal = (TBN * ln).normalized();
	}
	return TextureShader();
}
