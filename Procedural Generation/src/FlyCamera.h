#pragma once
#include "Camera.h"
class FlyCamera : public Camera
{
private:
	double thata;
	float speed;
	vec3 up;
	bool sbMouseButtonDown;
	double siPrevMouseX;
	double siPrevMouseY;
	glm::mat4 camRot;
	mat4 camTrans;
	mat4 camScale;
public:
	FlyCamera();
	void setSpeed(float speed);
	void update(double deltaTime);
};