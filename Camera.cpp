#include "Camera.h"
#include "Global.h"

void Camera::CameraTrans(Vector3f position)
{
	camera_position += position;
}

void Camera::RotatePitch(float angle)
{
	pitch += angle;
	//�����ǲ��ܳ���90�ȷ���������Ұ�ᵹ����
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

	//ÿ��ִ����ת������ʹ�����Ǻ�������µ�ǰ����
	lookAt_dir = front.normalized();
	Vector3f right = lookAt_dir.cross(Vector3f(0, 1, 0)).normalized();
	up_dir = right.cross(lookAt_dir).normalized();
}
