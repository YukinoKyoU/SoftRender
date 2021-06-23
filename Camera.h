#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Camera
{
	Vector3f camera_position;	//相机坐标
	Vector3f lookAt_dir;	//观看方向，单位
	Vector3f up_dir;			//向上向量，单位
	float fov;			//视野（高）
	float near;		//近平面
	float far;			//远平面
	float aspectRatio; //屏幕宽高比

	Camera(Vector3f _camera_pos,
		Vector3f _lookAt,
		Vector3f _up,		
		float _fov,		
		float _nNear,		
		float _nFar,		
		float _aspectRatio) : camera_position(_camera_pos), lookAt_dir(_lookAt), up_dir(_up), fov(_fov), near(_nNear), far(_nFar), aspectRatio(_aspectRatio)
	{}

};


#endif // !CAMERA_H_
