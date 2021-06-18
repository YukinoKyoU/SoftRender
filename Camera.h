#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Camera
{
	Vector3f camera_position;	//�������
	Vector3f lookAt;	//�ۿ����򣬵�λ
	Vector3f up;			//������������λ
	float fov;			//��Ұ���ߣ�
	float nNear;		//��ƽ��
	float nFar;			//Զƽ��
	float aspectRatio; //��Ļ��߱�

	Camera(Vector3f _camera_pos,
		Vector3f _lookAt,
		Vector3f _up,		
		float _fov,		
		float _nNear,		
		float _nFar,		
		float _aspectRatio) : camera_position(_camera_pos), lookAt(_lookAt), up(_up), fov(_fov), nNear(_nNear), nFar(_nFar), aspectRatio(_aspectRatio)
	{}

};


#endif // !CAMERA_H_
