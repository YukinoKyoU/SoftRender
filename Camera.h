#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Camera
{
	Vector3f position;	//�������
	Vector3f lookAt;	//�ۿ����򣬵�λ
	Vector3f up;			//������������λ
	float fov;			//��Ұ���ߣ�
	float nNear;		//��ƽ��
	float nFar;			//Զƽ��
	float aspectRatio; //��Ļ��߱�

};


#endif // !CAMERA_H_
