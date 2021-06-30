#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>


using namespace Eigen;

struct Camera
{
	Vector3f cameraPosition;	//�������
	Vector3f lookAtDir;			//�ۿ����򣬵�λ
	Vector3f upDir;				//������������λ
	float fov;					//��Ұ���ߣ�
	float near;					//��ƽ��
	float far;					//Զƽ��
	float aspectRatio;			//��Ļ��߱�

	float pitch;
	float yaw;

	Camera(Vector3f _cameraPos,
		Vector3f _lookAt,
		Vector3f _up,
		float _fov,
		float _nNear,
		float _nFar,
		float _aspectRatio):cameraPosition(_cameraPos), lookAtDir(_lookAt), upDir(_up),
		fov(_fov), near(_nNear), far(_nFar), aspectRatio(_aspectRatio), pitch(0.0f), yaw(0.0f){}

	//�ƶ����
	void CameraTrans(Vector3f position);
	
	//���¸���
	void RotatePitch(float angle);
	//����ƫ��
	void RotateYaw(float angle);

	//�������������
	void UpdateCameraVectors();

};


#endif // !CAMERA_H_
