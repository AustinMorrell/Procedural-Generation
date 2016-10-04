#include "FlyCamera.h"

void Camera::setPerspective(float fieldOfView, float aspectRatio, float n, float f)
{
	projectionTransform = glm::perspective(fieldOfView, aspectRatio, n, f);
}

void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	viewTransform = glm::lookAt(from, to, up);
}

void Camera::setPosition(vec3 position)
{
	worldTransform = worldTransform * glm::translate(position);
}

mat4 Camera::getWorldTransform()
{
	return worldTransform;
}

mat4 Camera::getView()
{
	return viewTransform;
}

mat4 Camera::getProjection()
{
	return projectionTransform;
}

mat4 Camera::getProjectionView()
{
	updateProjectionViewTransform();
	return projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	projectionViewTransform = projectionTransform * viewTransform * worldTransform;
}

FlyCamera::FlyCamera()
{
	sbMouseButtonDown = true;
	siPrevMouseX = 0;
	siPrevMouseY = 0;
}

void FlyCamera::setSpeed(float speed)
{
	this->speed = speed;
}

void FlyCamera::update(double deltaTime)
{
	GLFWwindow* wind = glfwGetCurrentContext();

	camRot, camScale, camTrans = mat4(1);

	double f_speed = glfwGetKey(wind, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? deltaTime * speed * 2 : deltaTime * speed;

	if (glfwGetKey(wind, 'W') == GLFW_PRESS)
	{
		camTrans = glm::translate(vec3(0, 0, (-0.10f * deltaTime) * 10));
	}
	if (glfwGetKey(wind, 'S') == GLFW_PRESS)
	{
		camTrans = glm::translate(vec3(0, 0, (0.10f * deltaTime) * 10));
	}
	if (glfwGetKey(wind, 'D') == GLFW_PRESS)
	{
		camTrans = glm::translate(vec3((0.10f * deltaTime) * 10, 0, 0));
	}
	if (glfwGetKey(wind, 'A') == GLFW_PRESS)
	{
		camTrans = glm::translate(vec3((-0.10f * deltaTime) * 10, 0, 0));
	}

	if (glfwGetMouseButton(wind, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {

		double mouseX = 0, mouseY = 0;
		float theta = 0.0f;

		glfwGetCursorPos(wind, &mouseX, &mouseY);

		if (sbMouseButtonDown)
		{
			siPrevMouseX = mouseX;
			siPrevMouseY = mouseY;
			sbMouseButtonDown = false;
		}

		double iDeltaX = siPrevMouseX - mouseX;
		double iDeltaY = siPrevMouseY - mouseY;

		iDeltaX *= 0.005f;
		iDeltaY *= 0.005f;

		if (iDeltaX < 1 || iDeltaX > 1)
		{
			theta = iDeltaX;
			camRot[0][0] = cos(theta);
			camRot[0][2] = sin(theta);
			camRot[2][0] = -1.f * sin(theta);
			camRot[2][2] = cos(theta);
		}

		if (iDeltaY < 1 || iDeltaY > 1)
		{
			theta = iDeltaY;
			camRot[1][1] = cos(theta);
			camRot[1][2] = -1.f * sin(theta);
			camRot[2][1] = sin(theta);
			camRot[2][2] = cos(theta);
		}

		siPrevMouseX = mouseX;
		siPrevMouseY = mouseY;
	}
	else {
		sbMouseButtonDown = true;
	}

	viewTransform *= glm::inverse(camTrans * camRot * camScale);
	getProjectionView();
}