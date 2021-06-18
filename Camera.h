#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Eigen>

using namespace Eigen;

struct Camera
{
	Vector3f camera_position;	//相机坐标
	Vector3f lookAt;	//观看方向，单位
	Vector3f up;			//向上向量，单位
	float fov;			//视野（高）
	float nNear;		//近平面
	float nFar;			//远平面
	float aspectRatio; //屏幕宽高比

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
