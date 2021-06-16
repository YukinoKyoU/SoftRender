#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>

using namespace Eigen;

//Shader�࣬�����������Ϣ���㷵�ص���ɫֵ
class Shader
{
public:
	//����shader����ɫ��Ϣ
	void setColor(const Vector3f& c);
	//���ؼ������ɫ
	Vector3f baseVertexColor();
	//����shader�ķ��ߣ�4άת3ά��
	void setNormal(const Vector4f& n);
	//���ؼ���ķ���
	Vector3f normalShader();

private:
	Vector3f color;
	Vector3f normal;
};

#endif // !Shader
