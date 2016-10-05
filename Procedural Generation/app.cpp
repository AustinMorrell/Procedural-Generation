#include "app.h"

ProceduralGeneration::ProceduralGeneration()
{
	glfwInit();

	window = glfwCreateWindow(1280, 720, "Procedural Generation", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Cam.setLookAt(glm::vec3(15, 15, 15), glm::vec3(0), glm::vec3(0, 1, 0));
	Cam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	Cam.setSpeed(50);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glEnable(GL_BLEND);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
}

bool ProceduralGeneration::start()
{
	int dims = 25;
	float* perlin_data = generatePerlin(dims);

	int imageWidth = dims, imageHeight = dims, imageFormat = 0;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth, imageHeight, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	stbi_image_free(perlin_data);

	unsigned char* data;

	data = stbi_load("data/textures/rocky_ground.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_rocks);
	glBindTexture(GL_TEXTURE_2D, m_rocks);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("data/textures/dirt_grass.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_grass);
	glBindTexture(GL_TEXTURE_2D, m_grass);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("data/textures/gravel.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_gravel);
	glBindTexture(GL_TEXTURE_2D, m_gravel);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	const char* vsSource;
	std::string vs = ReadFromFile("vsInfo.txt");
	vsSource = vs.c_str();

	const char* fsSource;
	std::string fs = ReadFromFile("fsInfo.txt");
	fsSource = fs.c_str();

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	createPlane(dims, dims);

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return true;
}

bool ProceduralGeneration::update()
{
	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pastTime = newTime;
		newTime = glfwGetTime();
		DeltaTime = newTime - pastTime;
		return true;
	}
	return false;
}

void ProceduralGeneration::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "view_proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(Cam.getProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_rocks);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grass);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_gravel);

	loc = glGetUniformLocation(m_programID, "perlin_texture");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_programID, "rocky_texture");
	glUniform1i(loc, 1);

	loc = glGetUniformLocation(m_programID, "grass_texture");
	glUniform1i(loc, 2);

	loc = glGetUniformLocation(m_programID, "gravel_texture");
	glUniform1i(loc, 3);

	drawPlane();
	glBindVertexArray(0);

	Cam.update(DeltaTime);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void ProceduralGeneration::destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::string ProceduralGeneration::ReadFromFile(std::string text)
{
	std::string data;
	std::string container;
	std::ifstream file(text);

	if (file.is_open())
	{
		while (std::getline(file, data))
		{
			container += data + "\n";
		}
		file.close();
	}
	return container;
}

void ProceduralGeneration::createPlane(const int &width, const int &height)
{
	Vertex* vertices = new Vertex[width * height];
	for (int r = 0; r < width; r++)
	{
		for (int c = 0; c < height; c++)
		{
			vertices[r * height + c].position = glm::vec4(c - height * 0.5f, 0, r - width * 0.5f, 1);
			vertices[r * height + c].UV = glm::vec2(c * (1.f / height), r * (1.f / width));
		}
	}

	p_indicesCounter = (width - 1) * (height - 1) * 6;
	unsigned int* indices = new unsigned int[p_indicesCounter];
	unsigned int index = 0;
	for (int r = 0; r < (width - 1); ++r) {
		for (int c = 0; c < (height - 1); ++c) {
			//triangle 1
			indices[index++] = r * height + c;
			indices[index++] = (r + 1) * height + c;
			indices[index++] = (r + 1) * height + (c + 1);
			//triangle 2
			indices[index++] = r * height + c;
			indices[index++] = (r + 1) * height + (c + 1);
			indices[index++] = r * height + (c + 1);
		}
	}

	glGenBuffers(1, &p_vbo);
	glGenBuffers(1, &p_ibo);
	glGenVertexArrays(1, &p_vao);
	glBindVertexArray(p_vao);

	glBindBuffer(GL_ARRAY_BUFFER, p_vbo);
	glBufferData(GL_ARRAY_BUFFER, (width * height) * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indicesCounter * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ProceduralGeneration::drawPlane()
{
	glBindVertexArray(p_vao);
	glDrawElements(GL_TRIANGLES, p_indicesCounter, GL_UNSIGNED_INT, nullptr);
}

float* ProceduralGeneration::generatePerlin(const int &dims)
{
	float* perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;
	int octaves = 6;

	for (int x = 0; x < dims; ++x)
	{
		for (int y = 0; y < dims; ++y)
		{
			float amplitude = 1.f;
			float persistence = 0.3f;
			perlin_data[y * dims + x] = glm::perlin(glm::vec2(x, y) * scale) * 0.5f + 0.5f;;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}
	return perlin_data;
}