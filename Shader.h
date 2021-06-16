#ifndef Shader_H_
#define Shader_H_

#include <Eigen/Eigen>

using namespace Eigen;

//Shader�࣬�����������Ϣ���㷵�ص���ɫֵ
class Shader
{
public:
	//�������ɫ��Ϣ
	void setColor(const Vector3f& c);
	//���ؼ������ɫ
	Vector3f BaseVertexColor()
	{
		return color;
	}

private:
	Vector3f color;
};

#endif // !Shader
