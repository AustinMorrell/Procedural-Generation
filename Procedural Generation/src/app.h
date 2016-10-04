#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "FlyCamera.h"
#include <stb_image.h>
#define PI 3.14159265358979323846

struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
};

class App
{
public:
	virtual bool start() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void destroy() = 0;
};

class ProceduralGeneration : public App {
public:
	ProceduralGeneration();
	bool start() override;
	bool update() override;
	void draw() override;
	void destroy() override;
	std::string ReadFromFile(std::string text);

	double DeltaTime;
private:
	glm::mat4 m_projectionViewMatrix;
	GLFWwindow* window;
	FlyCamera Cam;
	double pastTime = 0;
	double newTime;
	unsigned int m_programID;
};