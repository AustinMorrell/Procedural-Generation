#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "FlyCamera.h"
#include <stb_image.h>
#include <FBXFile.h>
#define PI 3.14159265358979323846

struct Vertex
{
	glm::vec4 position;
	glm::vec2 UV;
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

	float* generatePerlin(const int &dims);
	void createPlane(const int &width, const int &height);
	void drawPlane();

	double DeltaTime;
private:
	glm::mat4 m_projectionViewMatrix;
	GLFWwindow* window;
	FlyCamera Cam;
	double pastTime = 0;
	double newTime;
	unsigned int m_programID;

	unsigned int m_texture;
	unsigned int m_rocks;
	unsigned int m_grass;
	unsigned int m_gravel;
	unsigned int m_perlin_texture;
	unsigned int p_vao;
	unsigned int p_vbo;
	unsigned int p_ibo;
	int p_indicesCounter;
};