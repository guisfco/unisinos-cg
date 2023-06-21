#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "Shader.h"

using namespace std;

const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;
int verticesSize = 0;
string objFile = "../models/SuzanneTriTextured.obj", mtlFile = "../materials/SuzanneTriTextured.mtl";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupGeometry();
void readMaterialsFile(string filename, map<string, string>& properties);
float stofOrElse(string value, float def);

struct Vertex {
	float x, y, z, r = 0.4f, g = 0.1f, b = 0.4f;
};

struct Texture {
	float s, t;
};

struct Normal {
	float x, y, z;
};

vector<string> splitString(const string& input, char delimiter) {
	vector<string> tokens;
	istringstream iss(input);
	string token;

	while (getline(iss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

vector<float> parseObjToVertices(const string& filename) {
	ifstream file(filename);

	vector<Vertex> uniqueVertices;
	vector<Texture> uniqueTextures;
	vector<Normal> uniqueNormals;

	vector<float> vertices;

	if (file.is_open()) {
		string line;

		while (getline(file, line)) {
			vector<string> row = splitString(line, ' ');

			bool isVertice = row[0] == "v";
			bool isFace = row[0] == "f";
			bool isTexture = row[0] == "vt";
			bool isNormal = row[0] == "vn";

			if (isVertice) {
				float x = stof(row[1]), y = stof(row[2]), z = stof(row[3]);

				Vertex vertex;

				vertex.x = x;
				vertex.y = y;
				vertex.z = z;

				uniqueVertices.push_back(vertex);
			}

			if (isTexture) {
				float x = stof(row[1]), y = stof(row[2]);

				Texture texture;

				texture.s = x;
				texture.t = y;

				uniqueTextures.push_back(texture);
			}

			if (isNormal) {
				float x = stof(row[1]), y = stof(row[2]), z = stof(row[3]);

				Normal normal;

				normal.x = x;
				normal.y = y;
				normal.z = z;

				uniqueNormals.push_back(normal);
			}

			if (isFace) {
				vector<string> x = splitString(row[1], '/'), y = splitString(row[2], '/'), z = splitString(row[3], '/');

				int v1 = stoi(x[0]) - 1, v2 = stoi(y[0]) - 1, v3 = stoi(z[0]) - 1;
				int t1 = stoi(x[1]) - 1, t2 = stoi(y[1]) - 1, t3 = stoi(z[1]) - 1;
				int n1 = stoi(x[2]) - 1, n2 = stoi(y[2]) - 1, n3 = stoi(z[2]) - 1;

				vertices.push_back(uniqueVertices[v1].x);
				vertices.push_back(uniqueVertices[v1].y);
				vertices.push_back(uniqueVertices[v1].z);
				vertices.push_back(uniqueVertices[v1].r);
				vertices.push_back(uniqueVertices[v1].g);
				vertices.push_back(uniqueVertices[v1].b);
				vertices.push_back(uniqueTextures[t1].s);
				vertices.push_back(uniqueTextures[t1].t);
				vertices.push_back(uniqueNormals[n1].x);
				vertices.push_back(uniqueNormals[n1].y);
				vertices.push_back(uniqueNormals[n1].z);

				vertices.push_back(uniqueVertices[v2].x);
				vertices.push_back(uniqueVertices[v2].y);
				vertices.push_back(uniqueVertices[v2].z);
				vertices.push_back(uniqueVertices[v2].r);
				vertices.push_back(uniqueVertices[v2].g);
				vertices.push_back(uniqueVertices[v2].b);
				vertices.push_back(uniqueTextures[t2].s);
				vertices.push_back(uniqueTextures[t2].t);
				vertices.push_back(uniqueNormals[n2].x);
				vertices.push_back(uniqueNormals[n2].y);
				vertices.push_back(uniqueNormals[n2].z);

				vertices.push_back(uniqueVertices[v3].x);
				vertices.push_back(uniqueVertices[v3].y);
				vertices.push_back(uniqueVertices[v3].z);
				vertices.push_back(uniqueVertices[v3].r);
				vertices.push_back(uniqueVertices[v3].g);
				vertices.push_back(uniqueVertices[v3].b);
				vertices.push_back(uniqueTextures[t3].s);
				vertices.push_back(uniqueTextures[t3].t);
				vertices.push_back(uniqueNormals[n3].x);
				vertices.push_back(uniqueNormals[n3].y);
				vertices.push_back(uniqueNormals[n3].z);
			}
		}

		file.close();

		return vertices;
	}
	else {
		cout << "Unable to open the file: " << filename << endl;
	}
}

void readMaterialsFile(string filename, map<string, string>& properties) {
	ifstream file(filename);

	if (file.is_open()) {
		string line;

		while (getline(file, line)) {
			vector<string> row = splitString(line, ' ');

			if (row.size() == 0) {
				continue;
			}

			properties[row[0]] = row[1];
		}

		file.close();
	}
	else {
		cout << "Unable to open the file: " << filename << endl;
	}
}

float stofOrElse(string value, float def) {
	if (value.empty()) {
		return def;
	}

	return stof(value);
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa M4 -- Guilherme", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../shaders/shaders.vs", "../shaders/shaders.fs");

	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1);

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", glm::value_ptr(model));

	map<string, string> properties;
	readMaterialsFile(mtlFile, properties);

	shader.setFloat("ka", stofOrElse(properties["Ka"], 0));
	shader.setFloat("kd", stofOrElse(properties["Kd"], 1.5));
	shader.setFloat("ks", stofOrElse(properties["Ks"], 0));
	shader.setFloat("q", stofOrElse(properties["Ns"], 0));

	shader.setVec3("lightPos", -2.0f, 10.0f, 3.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		shader.setMat4("model", glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesSize);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}
}

int setupGeometry()
{
	vector<float> vertices = parseObjToVertices(objFile);

	verticesSize = vertices.size();

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo textura
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	//Atributo normal
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}



