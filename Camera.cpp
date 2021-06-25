#include "Camera.h"
#include "Global.h"

void Camera::CameraTrans(Vector3f position)
{
	camera_position += position;
}

void Camera::RotatePitch(float angle)
{
	pitch += angle;
	//俯仰角不能超过90度否则整个视野会倒过来
	if (pitch > 89.0)
		pitch = 89.0;
	if (pitch < -89.0)
		pitch = -89.0;
	UpdateCameraVectors();
}

void Camera::RotateYaw(float angle)
{
	yaw += angle;
	if (yaw > 360)
		yaw = 0;
	if (yaw < 0)
		yaw = 360;
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	Vector3f front;
	front.x() = -sin(angleToRadian(yaw)) * cos(angleToRadian(pitch));
	front.y() = sin(angleToRadian(pitch));
	front.z() = -cos(angleToRadian(yaw)) * cos(angleToRadian(pitch));

	//每次执行旋转操作后，使用三角函数求得新的前向量
	lookAt_dir = front.normalized();
	Vector3f right = lookAt_dir.cross(Vector3f(0, 1, 0)).normalized();
	up_dir = right.cross(lookAt_dir).normalized();
}
