#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>


using namespace Eigen;

struct Camera
{
	Vector3f cameraPosition;	//相机坐标
	Vector3f lookAtDir;			//观看方向，单位
	Vector3f upDir;				//向上向量，单位
	float fov;					//视野（高）
	float near;					//近平面
	float far;					//远平面
	float aspectRatio;			//屏幕宽高比

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

	//移动相机
	void CameraTrans(Vector3f position);
	
	//更新俯仰
	void RotatePitch(float angle);
	//更新偏航
	void RotateYaw(float angle);

	//更新相机的向量
	void UpdateCameraVectors();

};


#endif // !CAMERA_H_
