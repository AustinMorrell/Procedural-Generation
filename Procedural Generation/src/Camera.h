#pragma once
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <iostream>
#define GLM_SWIZZLE
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using namespace std;

class Camera
{
protected:
	mat4 worldTransform;
	mat4 viewTransform;
	mat4 projectionTransform;
	mat4 projectionViewTransform;
public:
	virtual void update(double deltaTime) = 0;
	void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void setLookAt(vec3 from, vec3 to, vec3 up);
	void setPosition(vec3 position);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjection();
	mat4 getProjectionView();
	void updateProjectionViewTransform();
};