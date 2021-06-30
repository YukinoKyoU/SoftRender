#include "Shader.h"

Shader::Shader()
{
	shaderType = ShaderType::TextureWithNormal;

	pointTexture = nullptr;
	pointBumpMap = nullptr;
	pointNormalMap = nullptr;
}

Vector3f Shader::setShaderType()
{
	switch (shaderType)
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
	pointColor = _pointColor;
}
void Shader::setNormal(const Vector4f& _pointNormal)
{
	pointNormal =  Vector3f( _pointNormal.x(), _pointNormal.y(), _pointNormal.z() ).normalized();
}
void Shader::setTexCoord(const Vector2f& tC)
{
	pointTexCoord = tC;
}
void Shader::setTexture(Texture* t)
{
	pointTexture = t;
}
void Shader::setBumpMap(Texture* bM)
{
	pointBumpMap = bM;
}
void Shader::setNormalMap(Texture* nM)
{
	pointNormalMap = nM;
}
void Shader::setLight(const std::vector<Light>& _lightLists)
{
	lightLists = &_lightLists;
}
void  Shader::setPosition(const Vector4f& _pointPosition)
{
	pointPosition = Vector3f(_pointPosition.x(), _pointPosition.y(), _pointPosition.z());
}

Vector3f Shader::normalShader()
{
	Vector3f color = (pointNormal.normalized() + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
	return color * 255;
}

Vector3f Shader::baseVertexColor()
{
	return pointColor;
}

Vector3f Shader::blinnPhongShader()
{
	Vector3f ka = Vector3f(0.005, 0.005, 0.005);
	Vector3f kd = pointColor / 255;
	Vector3f ks = Vector3f(0.7937, 0.7937, 0.7937);
	float p = 150;

	Vector3f tempPointPosition = Vector3f( pointPosition.x(), pointPosition.y(), pointPosition.z() );

	Vector3f resultColor = { 0,0,0 };

	for (const Light& light : *lightLists)
	{

		Vector3f tempLightPosition = Vector3f(light.lightPosition.x(), light.lightPosition.y(), light.lightPosition.z());
		
		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (tempLightPosition - tempPointPosition).dot(tempLightPosition - tempPointPosition);
		//光照方向
		Vector3f lightDir = (tempLightPosition - tempPointPosition).normalized();
		//观察方向
		Vector3f eyeDir = -tempPointPosition.normalized();
		//半程向量
		Vector3f h = (eyeDir.normalized() + lightDir.normalized()).normalized();

		diffuse = kd.cwiseProduct(light.lightIntensity / r) * std::max(0.0f, pointNormal.dot(lightDir));
		specular = ks.cwiseProduct(light.lightIntensity / r) * std::pow(std::max(0.0f, pointNormal.dot(h)), p);
		ambient = ka.cwiseProduct(light.ambLightIntensity);

		resultColor += (diffuse + specular + ambient);
	}

	return resultColor * 255.f;
}

Vector3f Shader::textureShader()
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
	float p = 150;

	Vector3f tempPointPosition = Vector3f(pointPosition.x(), pointPosition.y(), pointPosition.z());

	Vector3f resultColor = { 0, 0, 0 };
	for (const auto& light : *lightLists)
	{
		Vector3f tempLightPosition = Vector3f(light.lightPosition.x(), light.lightPosition.y(), light.lightPosition.z());

		Vector3f diffuse, specular, ambient;
		//点距光源的距离
		float r = (tempLightPosition - tempPointPosition).dot(tempLightPosition - tempPointPosition);
		//光照方向
		Vector3f lightDir = (tempLightPosition - tempPointPosition).normalized();
		//观察方向
		Vector3f eyeDir = -tempPointPosition.normalized();
		//半程向量
		Vector3f h = (eyeDir.normalized() + lightDir.normalized()).normalized();

		//计算环境光、漫反射和高光颜色
		diffuse = kd.cwiseProduct(light.lightIntensity / r) * std::max(0.0f, pointNormal.dot(lightDir));
		specular = ks.cwiseProduct(light.lightIntensity / r) * std::pow(std::max(0.0f, pointNormal.dot(h)), p);
		ambient = ka.cwiseProduct(light.ambLightIntensity);

		resultColor += ambient + diffuse + specular;
	}
	return resultColor * 255.f;
}
Vector3f Shader::textureWithBump()
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
	return textureShader();
}

Vector3f Shader::textureWithNormal()
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
	return textureShader();
}
